/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_a11y_sdnAccessible_inl_h_
#define mozilla_a11y_sdnAccessible_inl_h_

#include "sdnAccessible.h"

#include "DocAccessible.h"
#include "nsAccessibilityService.h"

namespace mozilla {
namespace a11y {

inline DocAccessible* sdnAccessible::GetDocument() const {
  return GetExistingDocAccessible(mNode->OwnerDoc());
}

inline AccessibleWrap* sdnAccessible::GetAccessible() {
  if (mWrap) {
    return mWrap;
  }

  DocAccessible* document = GetDocument();
  if (!document) {
    return nullptr;
  }

  // Once we have an accessible, we should hold a reference to it so that we
  // may preserve object identity.
  mWrap = static_cast<AccessibleWrap*>(
      document->GetAccessibleEvenIfNotInMap(mNode));
  return mWrap;
}

}  // namespace a11y
}  // namespace mozilla

#endif  // mozilla_a11y_sdnAccessible_inl_h_
