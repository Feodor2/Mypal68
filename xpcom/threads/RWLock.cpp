/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/RWLock.h"

#ifdef XP_WIN
#  include <windows.h>

/*static_assert(sizeof(SRWLOCK) <= sizeof(void*), "SRWLOCK is too big!");

#  define NativeHandle(m) (reinterpret_cast<SRWLOCK*>(&m))*/
#else
#  define NativeHandle(m) (&m)
#endif

namespace mozilla {

RWLock::RWLock(const char* aName)
    : BlockingResourceBase(aName, eMutex),
#ifdef DEBUG
      mOwningThread(nullptr),
#endif
      hModule(NULL),
      RtlDelete(NULL),
      RtlRelease(NULL),
      RtlAcquireExclusive(NULL),
      RtlAcquireShared(NULL),
      rtlRWLock()
{
#ifdef XP_WIN
  hModule = LoadLibraryW(L"NTDLL.DLL");

  RtlManagePtr Initialize = (RtlManagePtr)GetProcAddress(hModule, "RtlInitializeResource");
  RtlDelete = (RtlManagePtr)GetProcAddress(hModule, "RtlDeleteResource");
  RtlAcquireExclusive = (RtlOperatePtr)GetProcAddress(hModule, "RtlAcquireResourceExclusive");
  RtlAcquireShared = (RtlOperatePtr)GetProcAddress(hModule, "RtlAcquireResourceShared");
  RtlRelease = (RtlManagePtr)GetProcAddress(hModule, "RtlReleaseResource");
  MOZ_RELEASE_ASSERT(Initialize,
                     "RtlInitializeResource failed");
  Initialize(&rtlRWLock);
#else
  MOZ_RELEASE_ASSERT(pthread_rwlock_init(NativeHandle(mRWLock), nullptr) == 0,
                     "pthread_rwlock_init failed");
#endif
}

#ifdef DEBUG
bool RWLock::LockedForWritingByCurrentThread() {
  return mOwningThread == PR_GetCurrentThread();
}
#endif

RWLock::~RWLock() {
#ifndef XP_WIN
  MOZ_RELEASE_ASSERT(pthread_rwlock_destroy(NativeHandle(mRWLock)) == 0,
                     "pthread_rwlock_destroy failed");
#else
  RtlDelete(&rtlRWLock);
  FreeModule(hModule);
#endif
}


void RWLock::ReadLockInternal() {
#ifdef XP_WIN
  RtlAcquireShared(&rtlRWLock, TRUE);
#else
  MOZ_RELEASE_ASSERT(pthread_rwlock_rdlock(NativeHandle(mRWLock)) == 0,
                     "pthread_rwlock_rdlock failed");
#endif
}

void RWLock::ReadUnlockInternal() {
#ifdef XP_WIN
  RtlRelease(&rtlRWLock);
#else
  MOZ_RELEASE_ASSERT(pthread_rwlock_unlock(NativeHandle(mRWLock)) == 0,
                     "pthread_rwlock_unlock failed");
#endif
}

void RWLock::WriteLockInternal() {
#ifdef XP_WIN
  RtlAcquireExclusive(&rtlRWLock, TRUE);
#else
  MOZ_RELEASE_ASSERT(pthread_rwlock_wrlock(NativeHandle(mRWLock)) == 0,
                     "pthread_rwlock_wrlock failed");
#endif
}

void RWLock::WriteUnlockInternal() {
#ifdef XP_WIN
  RtlRelease(&rtlRWLock);
#else
  MOZ_RELEASE_ASSERT(pthread_rwlock_unlock(NativeHandle(mRWLock)) == 0,
                     "pthread_rwlock_unlock failed");
#endif
}

}  // namespace mozilla

#undef NativeHandle
