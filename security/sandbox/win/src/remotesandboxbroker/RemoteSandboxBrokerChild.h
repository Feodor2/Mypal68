/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef RemoteSandboxBrokerChild_h_
#define RemoteSandboxBrokerChild_h_

#include "mozilla/PRemoteSandboxBrokerChild.h"
#include "sandboxBroker.h"

namespace mozilla {

class RemoteSandboxBrokerChild : public PRemoteSandboxBrokerChild {
  friend class PRemoteSandboxBrokerChild;

 public:
  RemoteSandboxBrokerChild();
  virtual ~RemoteSandboxBrokerChild();
  bool Init(base::ProcessId aParentPid, MessageLoop* aIOLoop,
            UniquePtr<IPC::Channel> aChannel);

 private:
  mozilla::ipc::IPCResult AnswerLaunchApp(LaunchParameters&& aParams,
                                          bool* aOutOk, uint64_t* aOutHandle);

  void ActorDestroy(ActorDestroyReason aWhy);
  SandboxBroker mSandboxBroker;
};

}  // namespace mozilla

#endif
