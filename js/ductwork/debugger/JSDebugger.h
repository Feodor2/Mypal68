/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef JSDebugger_h
#define JSDebugger_h

#include "IJSDebugger.h"
#include "mozilla/Attributes.h"

namespace mozilla {
namespace jsdebugger {

class JSDebugger final : public IJSDebugger {
 public:
  NS_DECL_ISUPPORTS
  NS_DECL_IJSDEBUGGER

  JSDebugger();

 private:
  ~JSDebugger();
};

}  // namespace jsdebugger
}  // namespace mozilla

#endif /* JSDebugger_h */
