/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MemMapSnapshot.h"

#include "mozilla/AutoMemMap.h"
#include "mozilla/ResultExtensions.h"

namespace mozilla {

namespace ipc {

Result<Ok, nsresult> MemMapSnapshot::Init(size_t aSize, const char* aName) {
  MOZ_ASSERT(!mInitialized);

  if (NS_WARN_IF(!mMem.CreateFreezeable(aSize, aName))) {
    return Err(NS_ERROR_FAILURE);
  }
  if (NS_WARN_IF(!mMem.Map(aSize))) {
    return Err(NS_ERROR_FAILURE);
  }

  mInitialized = true;
  return Ok();
}

Result<Ok, nsresult> MemMapSnapshot::Finalize(loader::AutoMemMap& aMem) {
  MOZ_ASSERT(mInitialized);

  if (NS_WARN_IF(!mMem.Freeze())) {
    return Err(NS_ERROR_FAILURE);
  }
  // TakeHandle resets mMem, so call max_size first.
  size_t size = mMem.max_size();
  FileDescriptor memHandle(mMem.TakeHandle());
  MOZ_TRY(aMem.initWithHandle(memHandle, size));

  mInitialized = false;
  return Ok();
}

}  // namespace ipc
}  // namespace mozilla
