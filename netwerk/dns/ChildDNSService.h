/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_net_ChildDNSService_h
#define mozilla_net_ChildDNSService_h

#include "nsPIDNSService.h"
#include "nsIObserver.h"
#include "mozilla/Attributes.h"
#include "base/lock.h"
#include "DNSRequestChild.h"
#include "nsHashKeys.h"
#include "nsClassHashtable.h"

namespace mozilla {
namespace net {

class ChildDNSService final : public nsPIDNSService, public nsIObserver {
 public:
  // AsyncResolve (and CancelAsyncResolve) can be called off-main
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSPIDNSSERVICE
  NS_DECL_NSIDNSSERVICE
  NS_DECL_NSIOBSERVER

  ChildDNSService();

  static already_AddRefed<ChildDNSService> GetSingleton();

  void NotifyRequestDone(DNSRequestChild* aDnsRequest);

 private:
  virtual ~ChildDNSService() = default;

  void MOZ_ALWAYS_INLINE GetDNSRecordHashKey(
      const nsACString& aHost, uint16_t aType,
      const OriginAttributes& aOriginAttributes, uint32_t aFlags,
      nsIDNSListener* aListener, nsACString& aHashKey);
  nsresult AsyncResolveInternal(const nsACString& hostname, uint16_t type,
                                uint32_t flags, nsIDNSListener* listener,
                                nsIEventTarget* target_,
                                const OriginAttributes& aOriginAttributes,
                                nsICancelable** result);
  nsresult CancelAsyncResolveInternal(
      const nsACString& aHostname, uint16_t aType, uint32_t aFlags,
      nsIDNSListener* aListener, nsresult aReason,
      const OriginAttributes& aOriginAttributes);

  bool mFirstTime;
  bool mDisablePrefetch;

  // We need to remember pending dns requests to be able to cancel them.
  nsClassHashtable<nsCStringHashKey, nsTArray<RefPtr<DNSRequestChild>>>
      mPendingRequests;
  Lock mPendingRequestsLock;
};

}  // namespace net
}  // namespace mozilla

#endif  // mozilla_net_ChildDNSService_h
