/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SVGUseFrame.h"

#include "mozilla/PresShell.h"
#include "mozilla/SVGObserverUtils.h"
#include "mozilla/SVGUtils.h"
#include "mozilla/dom/MutationEvent.h"
#include "mozilla/dom/SVGUseElement.h"
#include "nsLayoutUtils.h"

using namespace mozilla::dom;

//----------------------------------------------------------------------
// Implementation

nsIFrame* NS_NewSVGUseFrame(mozilla::PresShell* aPresShell,
                            mozilla::ComputedStyle* aStyle) {
  return new (aPresShell)
      mozilla::SVGUseFrame(aStyle, aPresShell->GetPresContext());
}

namespace mozilla {

NS_IMPL_FRAMEARENA_HELPERS(SVGUseFrame)

//----------------------------------------------------------------------
// nsIFrame methods:

void SVGUseFrame::Init(nsIContent* aContent, nsContainerFrame* aParent,
                       nsIFrame* aPrevInFlow) {
  NS_ASSERTION(aContent->IsSVGElement(nsGkAtoms::use),
               "Content is not an SVG use!");

  mHasValidDimensions =
      static_cast<SVGUseElement*>(aContent)->HasValidDimensions();

  SVGGFrame::Init(aContent, aParent, aPrevInFlow);
}

nsresult SVGUseFrame::AttributeChanged(int32_t aNamespaceID, nsAtom* aAttribute,
                                       int32_t aModType) {
  // Currently our SMIL implementation does not modify the DOM attributes. Once
  // we implement the SVG 2 SMIL behaviour this can be removed
  // SVGUseElement::AfterSetAttr's implementation will be sufficient.
  if (aModType == MutationEvent_Binding::SMIL) {
    auto* content = SVGUseElement::FromNode(GetContent());
    content->ProcessAttributeChange(aNamespaceID, aAttribute);
  }

  return SVGGFrame::AttributeChanged(aNamespaceID, aAttribute, aModType);
}

void SVGUseFrame::PositionAttributeChanged() {
  // make sure our cached transform matrix gets (lazily) updated
  mCanvasTM = nullptr;
  nsLayoutUtils::PostRestyleEvent(GetContent()->AsElement(), RestyleHint{0},
                                  nsChangeHint_InvalidateRenderingObservers);
  SVGUtils::ScheduleReflowSVG(this);
  SVGUtils::NotifyChildrenOfSVGChange(this, TRANSFORM_CHANGED);
}

void SVGUseFrame::DimensionAttributeChanged(bool aHadValidDimensions,
                                            bool aAttributeIsUsed) {
  bool invalidate = aAttributeIsUsed;
  if (mHasValidDimensions != aHadValidDimensions) {
    mHasValidDimensions = !mHasValidDimensions;
    invalidate = true;
  }

  if (invalidate) {
    nsLayoutUtils::PostRestyleEvent(GetContent()->AsElement(), RestyleHint{0},
                                    nsChangeHint_InvalidateRenderingObservers);
    SVGUtils::ScheduleReflowSVG(this);
  }
}

void SVGUseFrame::HrefChanged() {
  nsLayoutUtils::PostRestyleEvent(GetContent()->AsElement(), RestyleHint{0},
                                  nsChangeHint_InvalidateRenderingObservers);
  SVGUtils::ScheduleReflowSVG(this);
}

//----------------------------------------------------------------------
// ISVGDisplayableFrame methods

void SVGUseFrame::ReflowSVG() {
  // We only handle x/y offset here, since any width/height that is in force is
  // handled by the SVGOuterSVGFrame for the anonymous <svg> that will be
  // created for that purpose.
  float x, y;
  static_cast<SVGUseElement*>(GetContent())
      ->GetAnimatedLengthValues(&x, &y, nullptr);
  mRect.MoveTo(nsLayoutUtils::RoundGfxRectToAppRect(gfxRect(x, y, 0.0, 0.0),
                                                    AppUnitsPerCSSPixel())
                   .TopLeft());

  // If we have a filter, we need to invalidate ourselves because filter
  // output can change even if none of our descendants need repainting.
  if (StyleEffects()->HasFilters()) {
    InvalidateFrame();
  }

  SVGGFrame::ReflowSVG();
}

void SVGUseFrame::NotifySVGChanged(uint32_t aFlags) {
  if (aFlags & COORD_CONTEXT_CHANGED && !(aFlags & TRANSFORM_CHANGED)) {
    // Coordinate context changes affect mCanvasTM if we have a
    // percentage 'x' or 'y'
    SVGUseElement* use = static_cast<SVGUseElement*>(GetContent());
    if (use->mLengthAttributes[SVGUseElement::ATTR_X].IsPercentage() ||
        use->mLengthAttributes[SVGUseElement::ATTR_Y].IsPercentage()) {
      aFlags |= TRANSFORM_CHANGED;
      // Ancestor changes can't affect how we render from the perspective of
      // any rendering observers that we may have, so we don't need to
      // invalidate them. We also don't need to invalidate ourself, since our
      // changed ancestor will have invalidated its entire area, which includes
      // our area.
      // For perf reasons we call this before calling NotifySVGChanged() below.
      SVGUtils::ScheduleReflowSVG(this);
    }
  }

  // We don't remove the TRANSFORM_CHANGED flag here if we have a viewBox or
  // non-percentage width/height, since if they're set then they are cloned to
  // an anonymous child <svg>, and its SVGInnerSVGFrame will do that.

  SVGGFrame::NotifySVGChanged(aFlags);
}

}  // namespace mozilla
