/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PendingIPCBlobParent_h
#define mozilla_dom_PendingIPCBlobParent_h

#include "mozilla/dom/PPendingIPCBlobParent.h"

namespace mozilla {

namespace ipc {
class PBackgroundParent;
}

namespace dom {

class BlobImpl;

class PendingIPCBlobParent final : public PPendingIPCBlobParent {
 public:
  static PendingIPCBlobParent* Create(PBackgroundParent* aManager,
                                      BlobImpl* aBlobImpl);

  void ActorDestroy(ActorDestroyReason aWhy) override {}

  mozilla::ipc::IPCResult Recv__delete__(
      const PendingIPCBlobData& aData) override;

 private:
  explicit PendingIPCBlobParent(BlobImpl* aBlobImpl);

  RefPtr<BlobImpl> mBlobImpl;
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_PendingIPCBlobParent_h
