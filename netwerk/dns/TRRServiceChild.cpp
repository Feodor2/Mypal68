/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/net/TRRServiceChild.h"
#include "mozilla/ClearOnShutdown.h"
#include "mozilla/DataStorage.h"
#include "mozilla/StaticPtr.h"
#include "nsIDNService.h"
#include "nsIObserverService.h"
#include "TRRService.h"

namespace mozilla {
namespace net {

static StaticRefPtr<nsIDNSService> sDNSService;

void TRRServiceChild::Init() {
  nsCOMPtr<nsIDNSService> dns =
      do_GetService("@mozilla.org/network/dns-service;1");
  sDNSService = dns;
  ClearOnShutdown(&sDNSService);
  MOZ_ASSERT(sDNSService);
  MOZ_ASSERT(gTRRService);
}

mozilla::ipc::IPCResult TRRServiceChild::RecvInitTRRBLStorage(
    const psm::DataStorageEntry& aEntry, const FileDescriptor& aWriteFd) {
  RefPtr<DataStorage> storage =
      DataStorage::Get(DataStorageClass::TRRBlacklist);
  if (storage) {
    if (NS_SUCCEEDED(storage->Init(&aEntry.items(), aWriteFd))) {
      gTRRService->InitTRRBLStorage(storage);
    }
  }
  return IPC_OK();
}

mozilla::ipc::IPCResult TRRServiceChild::RecvClearDNSCache(
    const bool& aTrrToo) {
  Unused << sDNSService->ClearCache(aTrrToo);
  return IPC_OK();
}

}  // namespace net
}  // namespace mozilla
