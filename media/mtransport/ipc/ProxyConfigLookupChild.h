/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_net_ProxyConfigLookupChild_h
#define mozilla_net_ProxyConfigLookupChild_h

#include "mozilla/net/PProxyConfigLookup.h"
#include "mozilla/net/PProxyConfigLookupChild.h"
#include <functional>

namespace mozilla {

namespace net {

class ProxyConfigLookupChild final : public PProxyConfigLookupChild {
 public:
  explicit ProxyConfigLookupChild(std::function<void(bool)>&& aCallback);
  ~ProxyConfigLookupChild();

  mozilla::ipc::IPCResult Recv__delete__(const bool& aProxied) override;

 private:
  std::function<void(bool)> mCallback;
};

}  // namespace net
}  // namespace mozilla

#endif  // mozilla_net_ProxyConfigLookupChild_h
