/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "LSValue.h"

namespace mozilla {
namespace dom {

const LSValue& VoidLSValue() {
  static const LSValue sVoidLSValue(VoidCString(), 0, false);

  return sVoidLSValue;
}

}  // namespace dom
}  // namespace mozilla
