/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DOM_SVG_SVGANIMATEDSTRING_H_
#define DOM_SVG_SVGANIMATEDSTRING_H_

#include "DOMSVGAnimatedString.h"
#include "nsError.h"
#include "mozilla/Attributes.h"
#include "mozilla/SVGAnimatedClassOrString.h"
#include "mozilla/UniquePtr.h"

namespace mozilla {

class SMILValue;

namespace dom {
class SVGElement;
}

class SVGAnimatedString final : public SVGAnimatedClassOrString {
 public:
  using SVGElement = dom::SVGElement;

  void Init(uint8_t aAttrEnum) {
    mAnimVal = nullptr;
    mAttrEnum = aAttrEnum;
    mIsBaseSet = false;
  }

  void SetBaseValue(const nsAString& aValue, SVGElement* aSVGElement,
                    bool aDoSetAttr) override;
  void GetBaseValue(nsAString& aValue,
                    const SVGElement* aSVGElement) const override {
    aSVGElement->GetStringBaseValue(mAttrEnum, aValue);
  }

  void SetAnimValue(const nsAString& aValue, SVGElement* aSVGElement);
  void GetAnimValue(nsAString& aResult,
                    const SVGElement* aSVGElement) const override;

  // Returns true if the animated value of this string has been explicitly
  // set (either by animation, or by taking on the base value which has been
  // explicitly set by markup or a DOM call), false otherwise.
  // If this returns false, the animated value is still valid, that is,
  // usable, and represents the default base value of the attribute.
  bool IsExplicitlySet() const { return !!mAnimVal || mIsBaseSet; }

  UniquePtr<SMILAttr> ToSMILAttr(SVGElement* aSVGElement);

 private:
  UniquePtr<nsString> mAnimVal;
  uint8_t mAttrEnum;  // element specified tracking for attribute
  bool mIsBaseSet;

 public:
  struct SMILString : public SMILAttr {
   public:
    SMILString(SVGAnimatedString* aVal, SVGElement* aSVGElement)
        : mVal(aVal), mSVGElement(aSVGElement) {}

    // These will stay alive because a SMILAttr only lives as long
    // as the Compositing step, and DOM elements don't get a chance to
    // die during that.
    SVGAnimatedString* mVal;
    SVGElement* mSVGElement;

    // SMILAttr methods
    virtual nsresult ValueFromString(
        const nsAString& aStr, const dom::SVGAnimationElement* aSrcElement,
        SMILValue& aValue, bool& aPreventCachingOfSandwich) const override;
    virtual SMILValue GetBaseValue() const override;
    virtual void ClearAnimValue() override;
    virtual nsresult SetAnimValue(const SMILValue& aValue) override;
  };
};

}  // namespace mozilla

#endif  // DOM_SVG_SVGANIMATEDSTRING_H_
