/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_RemoteWorkerServiceChild_h
#define mozilla_dom_RemoteWorkerServiceChild_h

#include "mozilla/dom/PRemoteWorkerServiceChild.h"
#include "nsISupportsImpl.h"

namespace mozilla {
namespace dom {

class RemoteWorkerController;
class RemoteWorkerData;

class RemoteWorkerServiceChild final : public PRemoteWorkerServiceChild {
 public:
  NS_INLINE_DECL_REFCOUNTING(RemoteWorkerServiceChild)

  RemoteWorkerServiceChild();

 private:
  ~RemoteWorkerServiceChild();
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_RemoteWorkerServiceChild_h
