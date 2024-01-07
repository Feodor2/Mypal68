/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A type that can be sent without needing to make a copy during
 * serialization. In addition the receiver can take ownership of the
 * data to avoid having to make an additional copy. */

#ifndef mozilla_ipc_ByteBufUtils_h
#define mozilla_ipc_ByteBufUtils_h

#include "mozilla/ipc/ByteBuf.h"
#include "ipc/IPCMessageUtils.h"

namespace mozilla {
namespace ipc {

struct ByteBuffer {
  ByteBuffer(size_t aLength, uint8_t* aData)
      : mLength(aLength), mData(aData), mOwned(false) {}

  ByteBuffer(ByteBuffer&& aFrom)
      : mLength(aFrom.mLength), mData(aFrom.mData), mOwned(aFrom.mOwned) {
    aFrom.mLength = 0;
    aFrom.mData = nullptr;
    aFrom.mOwned = false;
  }

  ByteBuffer(ByteBuffer& aFrom)
      : mLength(aFrom.mLength), mData(aFrom.mData), mOwned(aFrom.mOwned) {
    aFrom.mLength = 0;
    aFrom.mData = nullptr;
    aFrom.mOwned = false;
  }

  ByteBuffer() : mLength(0), mData(nullptr), mOwned(false) {}

  bool Allocate(size_t aLength) {
    MOZ_ASSERT(mData == nullptr);
    mData = (uint8_t*)malloc(aLength);
    if (!mData) {
      return false;
    }
    mLength = aLength;
    mOwned = true;
    return true;
  }

  ~ByteBuffer() {
    if (mData && mOwned) {
      free(mData);
    }
  }

  bool operator==(const ByteBuffer& other) const {
    return mLength == other.mLength && !(memcmp(mData, other.mData, mLength));
  }

  size_t mLength;
  uint8_t* mData;
  bool mOwned;
};

}  // namespace ipc
}  // namespace mozilla

namespace IPC {

template <>
struct ParamTraits<mozilla::ipc::ByteBuf> {
  typedef mozilla::ipc::ByteBuf paramType;

  // this is where we transfer the memory from the ByteBuf to IPDL, avoiding a
  // copy
  static void Write(Message* aMsg, paramType&& aParam) {
    WriteParam(aMsg, aParam.mLen);
    // hand over ownership of the buffer to the Message
    aMsg->WriteBytesZeroCopy(aParam.mData, aParam.mLen, aParam.mCapacity);
    aParam.mData = nullptr;
    aParam.mCapacity = 0;
    aParam.mLen = 0;
  }

  static bool Read(const Message* aMsg, PickleIterator* aIter,
                   paramType* aResult) {
    // We make a copy from the BufferList so that we get a contigous result.
    // For users the can handle a non-contiguous result using ExtractBuffers
    // is an option, alternatively if the users don't need to take ownership of
    // the data they can use the removed FlattenBytes (bug 1297981)
    size_t length;
    return ReadParam(aMsg, aIter, &length) && aResult->Allocate(length) &&
           aMsg->ReadBytesInto(aIter, aResult->mData, length);
  }

  static void Log(const paramType& aParam, std::wstring* aLog) {
    aLog->append(L"(byte buf)");
  }
};

template <>
struct ParamTraits<mozilla::ipc::ByteBuffer> {
  typedef mozilla::ipc::ByteBuffer paramType;

  static void Write(Message* aMsg, const paramType& aParam) {
    WriteParam(aMsg, aParam.mLength);
    aMsg->WriteBytes(aParam.mData, aParam.mLength);
  }

  static bool Read(const Message* aMsg, PickleIterator* aIter,
                   paramType* aResult) {
    size_t length;
    return ReadParam(aMsg, aIter, &length) && aResult->Allocate(length) &&
           aMsg->ReadBytesInto(aIter, aResult->mData, length);
  }
};

}  // namespace IPC

#endif  // ifndef mozilla_ipc_ByteBufUtils_h
