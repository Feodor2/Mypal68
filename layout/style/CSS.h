/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* DOM object holding utility CSS functions */

#ifndef mozilla_dom_CSS_h_
#define mozilla_dom_CSS_h_

#include "mozilla/Attributes.h"
#include "mozilla/Preferences.h"

namespace mozilla {

class ErrorResult;

namespace dom {

class GlobalObject;

class CSS {
 private:
  CSS() = delete;

 public:
  static bool Supports(const GlobalObject& aGlobal, const nsAString& aProperty,
                       const nsAString& aValue, ErrorResult& aRv);

  static bool Supports(const GlobalObject& aGlobal,
                       const nsAString& aDeclaration, ErrorResult& aRv);

  static void Escape(const GlobalObject& aGlobal, const nsAString& aIdent,
                     nsAString& aReturn);
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_CSS_h_
