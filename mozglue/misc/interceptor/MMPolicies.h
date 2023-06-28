/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef mozilla_interceptor_MMPolicies_h
#define mozilla_interceptor_MMPolicies_h

#include "mozilla/Assertions.h"
#include "mozilla/TypedEnumBits.h"
#include "mozilla/Types.h"

#include <windows.h>

// _CRT_RAND_S is not defined everywhere, but we need it.
#if !defined(_CRT_RAND_S)
extern "C" errno_t rand_s(unsigned int* randomValue);
#endif  // !defined(_CRT_RAND_S)

namespace mozilla {
namespace interceptor {

enum class ReservationFlags : uint32_t {
  eDefault = 0,
  eForceFirst2GB = 1,
};

MOZ_MAKE_ENUM_CLASS_BITWISE_OPERATORS(ReservationFlags)

class MMPolicyBase {
 public:
  static DWORD ComputeAllocationSize(const uint32_t aRequestedSize) {
    MOZ_ASSERT(aRequestedSize);
    DWORD result = aRequestedSize;

    const uint32_t granularity = GetAllocGranularity();

    uint32_t mod = aRequestedSize % granularity;
    if (mod) {
      result += (granularity - mod);
    }

    return result;
  }

  static DWORD GetAllocGranularity() {
    static const DWORD kAllocGranularity = []() -> DWORD {
      SYSTEM_INFO sysInfo;
      ::GetSystemInfo(&sysInfo);
      return sysInfo.dwAllocationGranularity;
    }();

    return kAllocGranularity;
  }

  static DWORD GetPageSize() {
    static const DWORD kPageSize = []() -> DWORD {
      SYSTEM_INFO sysInfo;
      ::GetSystemInfo(&sysInfo);
      return sysInfo.dwPageSize;
    }();

    return kPageSize;
  }

#if defined(_M_X64)

  /**
   * This function locates a virtual memory region of |aDesiredBytesLen| that
   * resides in the lowest 2GB of address space. We do this by scanning the
   * virtual memory space for a block of unallocated memory that is sufficiently
   * large. We must stay below the 2GB mark because a 10-byte patch uses movsxd
   * (ie, sign extension) to expand the pointer to 64-bits, so bit 31 of the
   * found region must be 0.
   */
  static PVOID FindLowRegion(HANDLE aProcess, const size_t aDesiredBytesLen) {
    const DWORD granularity = GetAllocGranularity();

    MOZ_ASSERT(aDesiredBytesLen / granularity > 0);
    if (!aDesiredBytesLen) {
      return nullptr;
    }

    // Generate a randomized base address that falls within the interval
    // [1MiB, 2GiB - aDesiredBytesLen]
    unsigned int rnd = 0;
    rand_s(&rnd);

    // Reduce rnd to a value that falls within the acceptable range
    const uint64_t kMinAddress = 0x0000000000100000ULL;
    const uint64_t kMaxAddress = 0x0000000080000000ULL;
    uint64_t maxOffset =
        (kMaxAddress - kMinAddress - aDesiredBytesLen) / granularity;
    uint64_t offset = (uint64_t(rnd) % maxOffset) * granularity;

    // Start searching at this address
    char* address = reinterpret_cast<char*>(kMinAddress) + offset;
    // The max address needs to incorporate the desired length
    char* const kMaxPtr =
        reinterpret_cast<char*>(kMaxAddress) - aDesiredBytesLen;

    MOZ_DIAGNOSTIC_ASSERT(address <= kMaxPtr);

    MEMORY_BASIC_INFORMATION mbi = {};
    SIZE_T len = sizeof(mbi);

    // Scan the range for a free chunk that is at least as large as
    // aDesiredBytesLen
    while (address <= kMaxPtr &&
           ::VirtualQueryEx(aProcess, address, &mbi, len)) {
      if (mbi.State == MEM_FREE && mbi.RegionSize >= aDesiredBytesLen) {
        return mbi.BaseAddress;
      }

      address = reinterpret_cast<char*>(mbi.BaseAddress) + mbi.RegionSize;
    }

    return nullptr;
  }

#endif  // defined(_M_X64)

  template <typename ReserveFnT>
  static PVOID Reserve(HANDLE aProcess, const uint32_t aSize,
                       const ReserveFnT& aReserveFn,
                       const ReservationFlags aFlags) {
#if defined(_M_X64)
    if (aFlags & ReservationFlags::eForceFirst2GB) {
      size_t curAttempt = 0;
      const size_t kMaxAttempts = 8;

      // We loop here because |FindLowRegion| may return a base address that
      // is reserved elsewhere before we have had a chance to reserve it
      // ourselves.
      while (curAttempt < kMaxAttempts) {
        PVOID base = FindLowRegion(aProcess, aSize);
        if (!base) {
          return nullptr;
        }

        PVOID result = aReserveFn(aProcess, base, aSize);
        if (result) {
          return result;
        }

        ++curAttempt;
      }

      // If we run out of attempts, we fall through to the default case where
      // the system chooses any base address it wants. In that case, the hook
      // will be set on a best-effort basis.
    }
#endif  // defined(_M_X64)

    return aReserveFn(aProcess, nullptr, aSize);
  }
};

class MMPolicyInProcess : public MMPolicyBase {
 public:
  typedef MMPolicyInProcess MMPolicyT;

  explicit MMPolicyInProcess()
      : mBase(nullptr), mReservationSize(0), mCommitOffset(0) {}

  MMPolicyInProcess(const MMPolicyInProcess&) = delete;
  MMPolicyInProcess& operator=(const MMPolicyInProcess&) = delete;

  MMPolicyInProcess(MMPolicyInProcess&& aOther)
      : mBase(nullptr), mReservationSize(0), mCommitOffset(0) {
    *this = std::move(aOther);
  }

  MMPolicyInProcess& operator=(MMPolicyInProcess&& aOther) {
    mBase = aOther.mBase;
    aOther.mBase = nullptr;

    mCommitOffset = aOther.mCommitOffset;
    aOther.mCommitOffset = 0;

    mReservationSize = aOther.mReservationSize;
    aOther.mReservationSize = 0;

    return *this;
  }

  // We always leak mBase
  ~MMPolicyInProcess() = default;

  explicit operator bool() const { return !!mBase; }

  /**
   * Should we unhook everything upon destruction?
   */
  bool ShouldUnhookUponDestruction() const { return true; }

  bool Read(void* aToPtr, const void* aFromPtr, size_t aLen) const {
    ::memcpy(aToPtr, aFromPtr, aLen);
    return true;
  }

  bool Write(void* aToPtr, const void* aFromPtr, size_t aLen) const {
    ::memcpy(aToPtr, aFromPtr, aLen);
    return true;
  }

#if defined(_M_IX86)
  bool WriteAtomic(void* aDestPtr, const uint16_t aValue) const {
    *static_cast<uint16_t*>(aDestPtr) = aValue;
    return true;
  }
#endif  // defined(_M_IX86)

  bool Protect(void* aVAddress, size_t aSize, uint32_t aProtFlags,
               uint32_t* aPrevProtFlags) const {
    MOZ_ASSERT(aPrevProtFlags);
    BOOL ok = ::VirtualProtect(aVAddress, aSize, aProtFlags,
                               reinterpret_cast<PDWORD>(aPrevProtFlags));
    MOZ_ASSERT(ok);
    return !!ok;
  }

  /**
   * @return true if the page that hosts aVAddress is accessible.
   */
  bool IsPageAccessible(void* aVAddress) const {
    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T result = ::VirtualQuery(aVAddress, &mbi, sizeof(mbi));

    return result && mbi.AllocationProtect && (mbi.Type & MEM_IMAGE) &&
           mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS;
  }

  bool FlushInstructionCache() const {
    return !!::FlushInstructionCache(::GetCurrentProcess(), nullptr, 0);
  }

  static DWORD GetTrampWriteProtFlags() { return PAGE_EXECUTE_READWRITE; }

#if defined(_M_X64)
  bool IsTrampolineSpaceInLowest2GB() const {
    return (mBase + mReservationSize) <=
           reinterpret_cast<uint8_t*>(0x0000000080000000ULL);
  }
#endif  // defined(_M_X64)

 protected:
  uint8_t* GetLocalView() const { return mBase; }

  uintptr_t GetRemoteView() const {
    // Same as local view for in-process
    return reinterpret_cast<uintptr_t>(mBase);
  }

  /**
   * @return the effective number of bytes reserved, or 0 on failure
   */
  uint32_t Reserve(const uint32_t aSize, const ReservationFlags aFlags) {
    if (!aSize) {
      return 0;
    }

    if (mBase) {
      MOZ_ASSERT(mReservationSize >= aSize);
      return mReservationSize;
    }

    mReservationSize = ComputeAllocationSize(aSize);

    auto reserveFn = [](HANDLE aProcess, PVOID aBase, uint32_t aSize) -> PVOID {
      return ::VirtualAlloc(aBase, aSize, MEM_RESERVE, PAGE_NOACCESS);
    };

    mBase = static_cast<uint8_t*>(MMPolicyBase::Reserve(
        ::GetCurrentProcess(), mReservationSize, reserveFn, aFlags));

    if (!mBase) {
      return 0;
    }

    return mReservationSize;
  }

  bool MaybeCommitNextPage(const uint32_t aRequestedOffset,
                           const uint32_t aRequestedLength) {
    if (!(*this)) {
      return false;
    }

    uint32_t limit = aRequestedOffset + aRequestedLength - 1;
    if (limit < mCommitOffset) {
      // No commit required
      return true;
    }

    MOZ_DIAGNOSTIC_ASSERT(mCommitOffset < mReservationSize);
    if (mCommitOffset >= mReservationSize) {
      return false;
    }

    PVOID local = ::VirtualAlloc(mBase + mCommitOffset, GetPageSize(),
                                 MEM_COMMIT, PAGE_EXECUTE_READ);
    if (!local) {
      return false;
    }

    mCommitOffset += GetPageSize();
    return true;
  }

 private:
  uint8_t* mBase;
  uint32_t mReservationSize;
  uint32_t mCommitOffset;
};

}  // namespace interceptor
}  // namespace mozilla

#endif  // mozilla_interceptor_MMPolicies_h
