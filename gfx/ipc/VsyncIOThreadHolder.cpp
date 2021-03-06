/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VsyncIOThreadHolder.h"

#include "mozilla/SystemGroup.h"

namespace mozilla {
namespace gfx {

VsyncIOThreadHolder::VsyncIOThreadHolder() {
  MOZ_COUNT_CTOR(VsyncIOThreadHolder);
}

VsyncIOThreadHolder::~VsyncIOThreadHolder() {
  MOZ_COUNT_DTOR(VsyncIOThreadHolder);

  if (!mThread) {
    return;
  }

  if (NS_IsMainThread()) {
    mThread->AsyncShutdown();
  } else {
    SystemGroup::Dispatch(TaskCategory::Other,
                          NewRunnableMethod("nsIThread::AsyncShutdown", mThread,
                                            &nsIThread::AsyncShutdown));
  }
}

bool VsyncIOThreadHolder::Start() {
  nsresult rv = NS_NewNamedThread("VsyncIOThread", getter_AddRefs(mThread));
  return NS_SUCCEEDED(rv);
}

RefPtr<nsIThread> VsyncIOThreadHolder::GetThread() const { return mThread; }

}  // namespace gfx
}  // namespace mozilla
