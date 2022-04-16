/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ScreenLuminance.h"
#include "nsScreen.h"

namespace mozilla {
namespace dom {

NS_IMPL_CYCLE_COLLECTION_ROOT_NATIVE(ScreenLuminance, AddRef)
NS_IMPL_CYCLE_COLLECTION_UNROOT_NATIVE(ScreenLuminance, Release)
NS_IMPL_CYCLE_COLLECTION_WRAPPERCACHE(ScreenLuminance, mScreen)

JSObject* ScreenLuminance::WrapObject(JSContext* aCx,
                                      JS::Handle<JSObject*> aGivenProto) {
  return ScreenLuminance_Binding::Wrap(aCx, this, aGivenProto);
}

}  // namespace dom
}  // namespace mozilla
