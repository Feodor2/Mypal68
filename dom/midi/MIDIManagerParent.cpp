/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/MIDIManagerParent.h"
#include "mozilla/dom/MIDIPlatformService.h"

namespace mozilla {
namespace dom {

void MIDIManagerParent::ActorDestroy(ActorDestroyReason aWhy) {}

void MIDIManagerParent::Teardown() {
  if (MIDIPlatformService::IsRunning()) {
    MIDIPlatformService::Get()->RemoveManager(this);
  }
}

mozilla::ipc::IPCResult MIDIManagerParent::RecvShutdown() {
  Teardown();
  Unused << Send__delete__(this);
  return IPC_OK();
}

}  // namespace dom
}  // namespace mozilla
