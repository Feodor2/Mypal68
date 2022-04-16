/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/BorrowedAttrInfo.h"

namespace mozilla {
namespace dom {

BorrowedAttrInfo::BorrowedAttrInfo(const nsAttrName* aName,
                                   const nsAttrValue* aValue)
    : mName(aName), mValue(aValue) {
  MOZ_ASSERT_IF(mName, mValue);
}

BorrowedAttrInfo::BorrowedAttrInfo(const BorrowedAttrInfo& aOther)
    : mName(aOther.mName), mValue(aOther.mValue) {
  MOZ_ASSERT_IF(mName, mValue);
}

}  // namespace dom
}  // namespace mozilla
