/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AccessibleOrProxy.h"

namespace mozilla {
namespace a11y {

AccessibleOrProxy AccessibleOrProxy::Parent() const {
  if (IsAccessible()) {
    return AsAccessible()->Parent();
  }

  ProxyAccessible* proxy = AsProxy();
  if (!proxy) {
    return nullptr;
  }

  if (ProxyAccessible* parent = proxy->Parent()) {
    return parent;
  }

  // Otherwise this should be the proxy for the tab's top level document.
  return proxy->OuterDocOfRemoteBrowser();
}

}  // namespace a11y
}  // namespace mozilla
