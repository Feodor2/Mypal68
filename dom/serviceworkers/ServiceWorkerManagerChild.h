/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_ServiceWorkerManagerChild_h
#define mozilla_dom_ServiceWorkerManagerChild_h

#include "mozilla/dom/PServiceWorkerManagerChild.h"
#include "mozilla/ipc/BackgroundUtils.h"

namespace mozilla {

class OriginAttributes;

namespace ipc {
class BackgroundChildImpl;
}  // namespace ipc

namespace dom {

class ServiceWorkerManagerChild final : public PServiceWorkerManagerChild {
  friend class mozilla::ipc::BackgroundChildImpl;

 public:
  NS_INLINE_DECL_REFCOUNTING(ServiceWorkerManagerChild)

  void ManagerShuttingDown() { mShuttingDown = true; }

  mozilla::ipc::IPCResult RecvNotifyRegister(
      const ServiceWorkerRegistrationData& aData);

  mozilla::ipc::IPCResult RecvNotifySoftUpdate(
      const OriginAttributes& aOriginAttributes, const nsString& aScope);

  mozilla::ipc::IPCResult RecvNotifyUnregister(
      const PrincipalInfo& aPrincipalInfo, const nsString& aScope);

  mozilla::ipc::IPCResult RecvNotifyRemove(const nsCString& aHost);

  mozilla::ipc::IPCResult RecvNotifyRemoveAll();

  PServiceWorkerUpdaterChild* AllocPServiceWorkerUpdaterChild(
      const OriginAttributes& originAttributes, const nsCString& scope);

  bool DeallocPServiceWorkerUpdaterChild(PServiceWorkerUpdaterChild* aActor);

 private:
  ServiceWorkerManagerChild() : mShuttingDown(false) {}

  ~ServiceWorkerManagerChild() = default;

  bool mShuttingDown;
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_ServiceWorkerManagerChild_h
