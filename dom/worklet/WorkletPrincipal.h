/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_worklet_WorkletPrincipal_h
#define mozilla_dom_worklet_WorkletPrincipal_h

namespace mozilla {
namespace dom {

namespace WorkletPrincipal {

JSPrincipals* GetWorkletPrincipal();

static const uint32_t kJSPrincipalsDebugToken = 0x7e2df9f4;

}  // namespace WorkletPrincipal

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_worklet_WorkletPrincipal_h
