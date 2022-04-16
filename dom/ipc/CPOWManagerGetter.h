/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_CPOWManagerGetter_h
#define mozilla_dom_CPOWManagerGetter_h

namespace mozilla {

namespace jsipc {
class CPOWManager;
} /* namespace jsipc */

namespace dom {

class CPOWManagerGetter {
 public:
  virtual mozilla::jsipc::CPOWManager* GetCPOWManager() = 0;
};

} /* namespace dom */
} /* namespace mozilla */

#endif /* mozilla_dom_CPOWManagerGetter_h */
