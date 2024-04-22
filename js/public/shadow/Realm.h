/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Shadow definition of |JS::Realm| innards.  Do not use this directly! */

#ifndef js_shadow_Realm_h
#define js_shadow_Realm_h

#include "jstypes.h"  // JS_PUBLIC_API

namespace JS {

class JS_PUBLIC_API Compartment;
class JS_PUBLIC_API Realm;

namespace shadow {

class Realm {
 protected:
  JS::Compartment* compartment_;

  explicit Realm(JS::Compartment* comp) : compartment_(comp) {}

 public:
  JS::Compartment* compartment() { return compartment_; }
  static shadow::Realm* get(JS::Realm* realm) {
    return reinterpret_cast<shadow::Realm*>(realm);
  }
};

}  // namespace shadow

}  // namespace JS

#endif  // js_shadow_Realm_h
