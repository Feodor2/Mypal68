/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/net/TRRServiceParent.h"

#include "mozilla/ipc/FileDescriptor.h"
#include "mozilla/net/SocketProcessParent.h"
#include "mozilla/psm/PSMIPCTypes.h"
#include "mozilla/Unused.h"
#include "nsINetworkLinkService.h"
#include "nsIObserverService.h"
#include "nsIOService.h"
#include "TRRService.h"

namespace mozilla {
namespace net {

NS_IMPL_ISUPPORTS(TRRServiceParent, nsIObserver, nsISupportsWeakReference)

void TRRServiceParent::Init() {
  MOZ_ASSERT(gIOService);

  if (!gIOService->SocketProcessReady()) {
    RefPtr<TRRServiceParent> self = this;
    gIOService->CallOrWaitForSocketProcess([self]() { self->Init(); });
    return;
  }

  SocketProcessParent* socketParent = SocketProcessParent::GetSingleton();
  if (!socketParent) {
    return;
  }

  nsCOMPtr<nsIObserverService> obs =
      static_cast<nsIObserverService*>(gIOService);
  TRRService::AddObserver(this, obs);

  Unused << socketParent->SendPTRRServiceConstructor(
      this);

  RefPtr<DataStorage> storage =
      DataStorage::Get(DataStorageClass::TRRBlacklist);
  if (!storage) {
    return;
  }

  if (NS_FAILED(storage->Init(nullptr))) {
    return;
  }

  psm::DataStorageEntry entry;
  storage->GetAll(&entry.items());

  RefPtr<TRRServiceParent> self = this;
  storage->AsyncTakeFileDesc([self, entry](ipc::FileDescriptor&& aWriteFd) {
    Unused << self->SendInitTRRBLStorage(entry, aWriteFd);
  });
}

NS_IMETHODIMP
TRRServiceParent::Observe(nsISupports* aSubject, const char* aTopic,
                          const char16_t* aData) {
  return NS_OK;
}

}  // namespace net
}  // namespace mozilla
