/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "jsapi.h"
#include "js/PropertyAndElement.h"  // JS_DefineProperty
#include "nsContentUtils.h"
#include "nsNetUtil.h"
#include "mozilla/CycleCollectedJSContext.h"
#include "mozilla/dom/ScriptSettings.h"
#include "mozilla/dom/SimpleGlobalObject.h"

TEST(DOM_Base_ContentUtils, StringifyJSON_EmptyValue)
{
  JS::Rooted<JSObject*> globalObject(
      mozilla::dom::RootingCx(),
      mozilla::dom::SimpleGlobalObject::Create(
          mozilla::dom::SimpleGlobalObject::GlobalType::BindingDetail));
  mozilla::dom::AutoJSAPI jsAPI;
  ASSERT_TRUE(jsAPI.Init(globalObject));
  JSContext* cx = jsAPI.cx();
  nsAutoString serializedValue;

  JS::Rooted<JS::Value> jsValue(cx);
  ASSERT_TRUE(nsContentUtils::StringifyJSON(cx, &jsValue, serializedValue));

  ASSERT_TRUE(serializedValue.EqualsLiteral("null"));
}

TEST(DOM_Base_ContentUtils, StringifyJSON_Object)
{
  JS::Rooted<JSObject*> globalObject(
      mozilla::dom::RootingCx(),
      mozilla::dom::SimpleGlobalObject::Create(
          mozilla::dom::SimpleGlobalObject::GlobalType::BindingDetail));
  mozilla::dom::AutoJSAPI jsAPI;
  ASSERT_TRUE(jsAPI.Init(globalObject));
  JSContext* cx = jsAPI.cx();
  nsAutoString serializedValue;

  JS::Rooted<JSObject*> jsObj(cx, JS_NewPlainObject(cx));
  JS::Rooted<JSString*> valueStr(cx, JS_NewStringCopyZ(cx, "Hello World!"));
  ASSERT_TRUE(JS_DefineProperty(cx, jsObj, "key1", valueStr, JSPROP_ENUMERATE));
  JS::Rooted<JS::Value> jsValue(cx, JS::ObjectValue(*jsObj));

  ASSERT_TRUE(nsContentUtils::StringifyJSON(cx, &jsValue, serializedValue));

  ASSERT_TRUE(serializedValue.EqualsLiteral("{\"key1\":\"Hello World!\"}"));
}
