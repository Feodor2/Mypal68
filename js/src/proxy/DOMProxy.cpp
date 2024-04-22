/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* DOM proxy-related functionality, including expando support. */

#include "js/friend/DOMProxy.h"  // JS::DOMProxyShadowsCheck
#include "proxy/DOMProxy.h"

#include "js/Proxy.h"  // js::GetProxyHandler, js::IsProxy

using JS::DOMProxyShadowsCheck;

static const void* gDOMProxyHandlerFamily = nullptr;
static DOMProxyShadowsCheck gDOMProxyShadowsCheck = nullptr;

void JS::SetDOMProxyInformation(const void* domProxyHandlerFamily,
                                DOMProxyShadowsCheck domProxyShadowsCheck) {
  gDOMProxyHandlerFamily = domProxyHandlerFamily;
  gDOMProxyShadowsCheck = domProxyShadowsCheck;
}

const void* js::GetDOMProxyHandlerFamily() { return gDOMProxyHandlerFamily; }

DOMProxyShadowsCheck js::GetDOMProxyShadowsCheck() {
  return gDOMProxyShadowsCheck;
}
