/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/ServoElementSnapshot.h"
#include "mozilla/GeckoBindings.h"
#include "mozilla/dom/Element.h"
#include "nsIContentInlines.h"
#include "nsContentUtils.h"

namespace mozilla {

ServoElementSnapshot::ServoElementSnapshot(const Element& aElement)
    : mState(0),
      mContains(Flags(0)),
      mIsTableBorderNonzero(false),
      mIsMozBrowserFrame(false),
      mIsSelectListBox(false),
      mClassAttributeChanged(false),
      mIdAttributeChanged(false) {
  MOZ_COUNT_CTOR(ServoElementSnapshot);
  MOZ_ASSERT(NS_IsMainThread());
  mIsInChromeDocument = nsContentUtils::IsChromeDoc(aElement.OwnerDoc());
  mSupportsLangAttr = aElement.SupportsLangAttr();
}

void ServoElementSnapshot::AddOtherPseudoClassState(const Element& aElement) {
  if (HasOtherPseudoClassState()) {
    return;
  }

  mIsTableBorderNonzero = Gecko_IsTableBorderNonzero(&aElement);
  mIsMozBrowserFrame = Gecko_IsBrowserFrame(&aElement);
  mIsSelectListBox = Gecko_IsSelectListBox(&aElement);

  mContains |= Flags::OtherPseudoClassState;
}

}  // namespace mozilla
