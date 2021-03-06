/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PaymentRequestUtils_h
#define mozilla_dom_PaymentRequestUtils_h

#include "nsIArray.h"
#include "nsTArray.h"

namespace mozilla {
namespace dom {

nsresult SerializeFromJSObject(JSContext* aCx, JS::HandleObject aObject,
                               nsAString& aSerializedObject);

nsresult SerializeFromJSVal(JSContext* aCx, JS::HandleValue aValue,
                            nsAString& aSerializedValue);

nsresult DeserializeToJSObject(const nsAString& aSerializedObject,
                               JSContext* aCx, JS::MutableHandleObject aObject);

nsresult DeserializeToJSValue(const nsAString& aSerializedObject,
                              JSContext* aCx, JS::MutableHandleValue aValue);

}  // end of namespace dom
}  // end of namespace mozilla

#endif
