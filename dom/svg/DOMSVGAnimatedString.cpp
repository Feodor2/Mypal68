/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "DOMSVGAnimatedString.h"

#include "mozilla/dom/SVGAnimatedStringBinding.h"

namespace mozilla {
namespace dom {

JSObject* DOMSVGAnimatedString::WrapObject(JSContext* aCx,
                                           JS::Handle<JSObject*> aGivenProto) {
  return SVGAnimatedString_Binding::Wrap(aCx, this, aGivenProto);
}

}  // namespace dom
}  // namespace mozilla
