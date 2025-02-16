/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* rendering object for the HTML <canvas> element */

#ifndef nsHTMLCanvasFrame_h___
#define nsHTMLCanvasFrame_h___

#include "mozilla/Attributes.h"
#include "nsContainerFrame.h"
#include "nsStringFwd.h"
#include "FrameLayerBuilder.h"

namespace mozilla {
class PresShell;
namespace layers {
class Layer;
class LayerManager;
#ifdef MOZ_BUILD_WEBRENDER
class WebRenderCanvasData;
#endif
}  // namespace layers
}  // namespace mozilla

class nsPresContext;
class nsDisplayItem;

nsIFrame* NS_NewHTMLCanvasFrame(mozilla::PresShell* aPresShell,
                                mozilla::ComputedStyle* aStyle);

class nsHTMLCanvasFrame final : public nsContainerFrame {
 public:
  typedef mozilla::layers::CanvasRenderer CanvasRenderer;
  typedef mozilla::layers::Layer Layer;
  typedef mozilla::layers::LayerManager LayerManager;
#ifdef MOZ_BUILD_WEBRENDER
  typedef mozilla::layers::WebRenderCanvasData WebRenderCanvasData;
#endif
  typedef mozilla::ContainerLayerParameters ContainerLayerParameters;

  NS_DECL_QUERYFRAME
  NS_DECL_FRAMEARENA_HELPERS(nsHTMLCanvasFrame)

  explicit nsHTMLCanvasFrame(ComputedStyle* aStyle, nsPresContext* aPresContext)
      : nsContainerFrame(aStyle, aPresContext, kClassID),
        mBorderPadding(GetWritingMode()) {}

  virtual void Init(nsIContent* aContent, nsContainerFrame* aParent,
                    nsIFrame* aPrevInFlow) override;

  virtual void BuildDisplayList(nsDisplayListBuilder* aBuilder,
                                const nsDisplayListSet& aLists) override;

  already_AddRefed<Layer> BuildLayer(
      nsDisplayListBuilder* aBuilder, LayerManager* aManager,
      nsDisplayItem* aItem,
      const ContainerLayerParameters& aContainerParameters);

#ifdef MOZ_BUILD_WEBRENDER
  bool UpdateWebRenderCanvasData(nsDisplayListBuilder* aBuilder,
                                 WebRenderCanvasData* aCanvasData);
#endif

  /* get the size of the canvas's image */
  nsIntSize GetCanvasSize() const;

  virtual nscoord GetMinISize(gfxContext* aRenderingContext) override;
  virtual nscoord GetPrefISize(gfxContext* aRenderingContext) override;
  virtual mozilla::IntrinsicSize GetIntrinsicSize() override;
  mozilla::AspectRatio GetIntrinsicRatio() const override;

  SizeComputationResult ComputeSize(
      gfxContext* aRenderingContext, mozilla::WritingMode aWM,
      const mozilla::LogicalSize& aCBSize, nscoord aAvailableISize,
      const mozilla::LogicalSize& aMargin,
      const mozilla::LogicalSize& aBorderPadding,
      const mozilla::StyleSizeOverrides& aSizeOverrides,
      mozilla::ComputeSizeFlags aFlags) override;

  virtual void Reflow(nsPresContext* aPresContext, ReflowOutput& aDesiredSize,
                      const ReflowInput& aReflowInput,
                      nsReflowStatus& aStatus) override;

  nsRect GetInnerArea() const;

#ifdef ACCESSIBILITY
  virtual mozilla::a11y::AccType AccessibleType() override;
#endif

  virtual bool IsFrameOfType(uint32_t aFlags) const override {
    return nsSplittableFrame::IsFrameOfType(
        aFlags & ~(nsIFrame::eReplaced | nsIFrame::eReplacedSizing));
  }

#ifdef DEBUG_FRAME_DUMP
  virtual nsresult GetFrameName(nsAString& aResult) const override;
#endif

  // Inserted child content gets its frames parented by our child block
  virtual nsContainerFrame* GetContentInsertionFrame() override {
    return PrincipalChildList().FirstChild()->GetContentInsertionFrame();
  }

  // Return the ::-moz-html-canvas-content anonymous box.
  void AppendDirectlyOwnedAnonBoxes(nsTArray<OwnedAnonBox>& aResult) override;

 protected:
  virtual ~nsHTMLCanvasFrame();

  nscoord GetContinuationOffset(nscoord* aWidth = 0) const;

  mozilla::LogicalMargin mBorderPadding;
};

#endif /* nsHTMLCanvasFrame_h___ */
