/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "InternalResponse.h"

#include "mozilla/Assertions.h"
#include "mozilla/dom/InternalHeaders.h"
#include "mozilla/dom/cache/CacheTypes.h"
#include "mozilla/ipc/PBackgroundSharedTypes.h"
#include "mozilla/ipc/IPCStreamUtils.h"
#include "mozilla/RandomNum.h"
#include "nsIRandomGenerator.h"
#include "nsStreamUtils.h"

namespace mozilla::dom {

namespace {

// Const variable for generate padding size
// XXX This will be tweaked to something more meaningful in Bug 1383656.
const uint32_t kMaxRandomNumber = 102400;

}  // namespace

InternalResponse::InternalResponse(uint16_t aStatus,
                                   const nsACString& aStatusText,
                                   RequestCredentials aCredentialsMode)
    : mType(ResponseType::Default),
      mStatus(aStatus),
      mStatusText(aStatusText),
      mHeaders(new InternalHeaders(HeadersGuardEnum::Response)),
      mBodySize(UNKNOWN_BODY_SIZE),
      mPaddingSize(UNKNOWN_PADDING_SIZE),
      mErrorCode(NS_OK),
      mCredentialsMode(aCredentialsMode) {}

InternalResponse::~InternalResponse() = default;

SafeRefPtr<InternalResponse> InternalResponse::Clone(CloneType aCloneType) {
  SafeRefPtr<InternalResponse> clone = CreateIncompleteCopy();

  clone->mHeaders = new InternalHeaders(*mHeaders);

  // Make sure the clone response will have the same padding size.
  clone->mPaddingInfo = mPaddingInfo;
  clone->mPaddingSize = mPaddingSize;

  clone->mCacheInfoChannel = mCacheInfoChannel;

  if (mWrappedResponse) {
    clone->mWrappedResponse = mWrappedResponse->Clone(aCloneType);
    MOZ_ASSERT(!mBody);
    return clone;
  }

  if (!mBody || aCloneType == eDontCloneInputStream) {
    return clone;
  }

  nsCOMPtr<nsIInputStream> clonedBody;
  nsCOMPtr<nsIInputStream> replacementBody;

  nsresult rv = NS_CloneInputStream(mBody, getter_AddRefs(clonedBody),
                                    getter_AddRefs(replacementBody));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return nullptr;
  }

  clone->mBody.swap(clonedBody);
  if (replacementBody) {
    mBody.swap(replacementBody);
  }

  return clone;
}

SafeRefPtr<InternalResponse> InternalResponse::BasicResponse() {
  MOZ_ASSERT(!mWrappedResponse,
             "Can't BasicResponse a already wrapped response");
  SafeRefPtr<InternalResponse> basic = CreateIncompleteCopy();
  basic->mType = ResponseType::Basic;
  basic->mHeaders = InternalHeaders::BasicHeaders(Headers());
  basic->mWrappedResponse = SafeRefPtrFromThis();
  return basic;
}

SafeRefPtr<InternalResponse> InternalResponse::CORSResponse() {
  MOZ_ASSERT(!mWrappedResponse,
             "Can't CORSResponse a already wrapped response");
  SafeRefPtr<InternalResponse> cors = CreateIncompleteCopy();
  cors->mType = ResponseType::Cors;
  cors->mHeaders = InternalHeaders::CORSHeaders(Headers(), mCredentialsMode);
  cors->mWrappedResponse = SafeRefPtrFromThis();
  return cors;
}

uint32_t InternalResponse::GetPaddingInfo() {
  // If it's an opaque response, the paddingInfo should be generated only when
  // paddingSize is unknown size.
  // If it's not, the paddingInfo should be nothing and the paddingSize should
  // be unknown size.
  MOZ_DIAGNOSTIC_ASSERT(
      (mType == ResponseType::Opaque && mPaddingSize == UNKNOWN_PADDING_SIZE &&
       mPaddingInfo.isSome()) ||
      (mType == ResponseType::Opaque && mPaddingSize != UNKNOWN_PADDING_SIZE &&
       mPaddingInfo.isNothing()) ||
      (mType != ResponseType::Opaque && mPaddingSize == UNKNOWN_PADDING_SIZE &&
       mPaddingInfo.isNothing()));
  return mPaddingInfo.isSome() ? mPaddingInfo.ref() : 0;
}

nsresult InternalResponse::GeneratePaddingInfo() {
  MOZ_DIAGNOSTIC_ASSERT(mType == ResponseType::Opaque);
  MOZ_DIAGNOSTIC_ASSERT(mPaddingSize == UNKNOWN_PADDING_SIZE);

  // Utilize random generator to generator a random number
  nsresult rv;
  uint32_t randomNumber = 0;
  nsCOMPtr<nsIRandomGenerator> randomGenerator =
      do_GetService("@mozilla.org/security/random-generator;1", &rv);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    Maybe<uint64_t> maybeRandomNum = RandomUint64();
    if (maybeRandomNum.isSome()) {
      mPaddingInfo.emplace(uint32_t(maybeRandomNum.value() % kMaxRandomNumber));
      return NS_OK;
    }
    return rv;
  }

  MOZ_DIAGNOSTIC_ASSERT(randomGenerator);

  uint8_t* buffer;
  rv = randomGenerator->GenerateRandomBytes(sizeof(randomNumber), &buffer);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    Maybe<uint64_t> maybeRandomNum = RandomUint64();
    if (maybeRandomNum.isSome()) {
      mPaddingInfo.emplace(uint32_t(maybeRandomNum.value() % kMaxRandomNumber));
      return NS_OK;
    }
    return rv;
  }

  memcpy(&randomNumber, buffer, sizeof(randomNumber));
  free(buffer);

  mPaddingInfo.emplace(randomNumber % kMaxRandomNumber);

  return rv;
}

int64_t InternalResponse::GetPaddingSize() {
  // We initialize padding size to an unknown size (-1). After cached, we only
  // pad opaque response. Opaque response's padding size might be unknown before
  // cached.
  MOZ_DIAGNOSTIC_ASSERT(mType == ResponseType::Opaque ||
                        mPaddingSize == UNKNOWN_PADDING_SIZE);
  MOZ_DIAGNOSTIC_ASSERT(mPaddingSize == UNKNOWN_PADDING_SIZE ||
                        mPaddingSize >= 0);

  return mPaddingSize;
}

void InternalResponse::SetPaddingSize(int64_t aPaddingSize) {
  // We should only pad the opaque response.
  MOZ_DIAGNOSTIC_ASSERT(
      (mType == ResponseType::Opaque) !=
      (aPaddingSize == InternalResponse::UNKNOWN_PADDING_SIZE));
  MOZ_DIAGNOSTIC_ASSERT(aPaddingSize == UNKNOWN_PADDING_SIZE ||
                        aPaddingSize >= 0);

  mPaddingSize = aPaddingSize;
}

void InternalResponse::SetPrincipalInfo(
    UniquePtr<mozilla::ipc::PrincipalInfo> aPrincipalInfo) {
  mPrincipalInfo = std::move(aPrincipalInfo);
}

LoadTainting InternalResponse::GetTainting() const {
  switch (mType) {
    case ResponseType::Cors:
      return LoadTainting::CORS;
    case ResponseType::Opaque:
      return LoadTainting::Opaque;
    default:
      return LoadTainting::Basic;
  }
}

SafeRefPtr<InternalResponse> InternalResponse::Unfiltered() {
  SafeRefPtr<InternalResponse> ref = mWrappedResponse.clonePtr();
  if (!ref) {
    ref = SafeRefPtrFromThis();
  }
  return ref;
}

SafeRefPtr<InternalResponse> InternalResponse::OpaqueResponse() {
  MOZ_ASSERT(!mWrappedResponse,
             "Can't OpaqueResponse a already wrapped response");
  SafeRefPtr<InternalResponse> response =
      MakeSafeRefPtr<InternalResponse>(0, ""_ns);
  response->mType = ResponseType::Opaque;
  response->mChannelInfo = mChannelInfo;
  if (mPrincipalInfo) {
    response->mPrincipalInfo =
        MakeUnique<mozilla::ipc::PrincipalInfo>(*mPrincipalInfo);
  }
  response->mWrappedResponse = SafeRefPtrFromThis();
  return response;
}

SafeRefPtr<InternalResponse> InternalResponse::OpaqueRedirectResponse() {
  MOZ_ASSERT(!mWrappedResponse,
             "Can't OpaqueRedirectResponse a already wrapped response");
  MOZ_ASSERT(!mURLList.IsEmpty(),
             "URLList should not be emtpy for internalResponse");
  SafeRefPtr<InternalResponse> response = OpaqueResponse();
  response->mType = ResponseType::Opaqueredirect;
  response->mURLList = mURLList;
  return response;
}

SafeRefPtr<InternalResponse> InternalResponse::CreateIncompleteCopy() {
  SafeRefPtr<InternalResponse> copy =
      MakeSafeRefPtr<InternalResponse>(mStatus, mStatusText);
  copy->mType = mType;
  copy->mURLList = mURLList;
  copy->mChannelInfo = mChannelInfo;
  if (mPrincipalInfo) {
    copy->mPrincipalInfo =
        MakeUnique<mozilla::ipc::PrincipalInfo>(*mPrincipalInfo);
  }
  return copy;
}

}  // namespace mozilla::dom
