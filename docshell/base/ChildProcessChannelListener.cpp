/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/ChildProcessChannelListener.h"

#include "nsDocShellLoadState.h"

namespace mozilla {
namespace dom {

static StaticRefPtr<ChildProcessChannelListener> sCPCLSingleton;

void ChildProcessChannelListener::RegisterCallback(uint64_t aIdentifier,
                                                   Callback&& aCallback) {
  if (auto channel = mChannels.Extract(aIdentifier)) {
    aCallback(*channel);
  } else {
    mCallbacks.InsertOrUpdate(aIdentifier, std::move(aCallback));
  }
}

void ChildProcessChannelListener::OnChannelReady(
    nsIChildChannel* aChannel, uint64_t aIdentifier) {
  if (auto callback = mCallbacks.Extract(aIdentifier)) {
    (*callback)(aChannel);
  } else {
    mChannels.InsertOrUpdate(aIdentifier, aChannel);
  }
}

already_AddRefed<ChildProcessChannelListener>
ChildProcessChannelListener::GetSingleton() {
  if (!sCPCLSingleton) {
    sCPCLSingleton = new ChildProcessChannelListener();
    ClearOnShutdown(&sCPCLSingleton);
  }
  RefPtr<ChildProcessChannelListener> cpcl = sCPCLSingleton;
  return cpcl.forget();
}

}  // namespace dom
}  // namespace mozilla
