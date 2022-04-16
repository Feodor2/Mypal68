/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SeekJob.h"

namespace mozilla {

SeekJob::~SeekJob() {
  MOZ_DIAGNOSTIC_ASSERT(mTarget.isNothing());
  MOZ_DIAGNOSTIC_ASSERT(mPromise.IsEmpty());
}

bool SeekJob::Exists() const {
  MOZ_ASSERT(mTarget.isSome() == !mPromise.IsEmpty());
  return mTarget.isSome();
}

void SeekJob::Resolve(const char* aCallSite) {
  mPromise.Resolve(true, aCallSite);
  mTarget.reset();
}

void SeekJob::RejectIfExists(const char* aCallSite) {
  mTarget.reset();
  mPromise.RejectIfExists(true, aCallSite);
}

}  // namespace mozilla
