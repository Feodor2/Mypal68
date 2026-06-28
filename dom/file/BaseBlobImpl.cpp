/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/BaseBlobImpl.h"
#include "mozilla/dom/BindingDeclarations.h"
#include "nsRFPService.h"
#include "prtime.h"

namespace mozilla::dom {

void BaseBlobImpl::GetName(nsAString& aName) const {
  MOZ_ASSERT(mIsFile, "Should only be called on files");
  aName = mName;
}

void BaseBlobImpl::GetDOMPath(nsAString& aPath) const {
  MOZ_ASSERT(mIsFile, "Should only be called on files");
  aPath = mPath;
}

void BaseBlobImpl::SetDOMPath(const nsAString& aPath) {
  MOZ_ASSERT(mIsFile, "Should only be called on files");
  mPath = aPath;
}

void BaseBlobImpl::GetMozFullPath(nsAString& aFileName,
                                  SystemCallerGuarantee /* unused */,
                                  ErrorResult& aRv) {
  MOZ_ASSERT(mIsFile, "Should only be called on files");

  GetMozFullPathInternal(aFileName, aRv);
}

void BaseBlobImpl::GetMozFullPathInternal(nsAString& aFileName,
                                          ErrorResult& aRv) {
  if (!mIsFile) {
    aRv.Throw(NS_ERROR_FAILURE);
    return;
  }

  aFileName.Truncate();
}

void BaseBlobImpl::GetType(nsAString& aType) { aType = mContentType; }

int64_t BaseBlobImpl::GetLastModified(ErrorResult& aRv) {
  MOZ_ASSERT(mIsFile, "Should only be called on files");
  return mLastModificationDate / PR_USEC_PER_MSEC;
}

int64_t BaseBlobImpl::GetFileId() const { return -1; }

/* static */
uint64_t BaseBlobImpl::NextSerialNumber() {
  static Atomic<uint64_t> nextSerialNumber;
  return nextSerialNumber++;
}

}  // namespace mozilla::dom
