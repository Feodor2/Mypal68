/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_layers_APZCTreeManagerParent_h
#define mozilla_layers_APZCTreeManagerParent_h

#include "mozilla/layers/PAPZCTreeManagerParent.h"

namespace mozilla {
namespace layers {

class APZCTreeManager;
class APZUpdater;

class APZCTreeManagerParent : public PAPZCTreeManagerParent {
 public:
  APZCTreeManagerParent(
#ifdef MOZ_BUILD_WEBRENDER
      WRRootId aWrRootId,
#else
      LayersId aLayersId,
#endif
      RefPtr<APZCTreeManager> aAPZCTreeManager, RefPtr<APZUpdater> mAPZUpdater);
  virtual ~APZCTreeManagerParent();

  LayersId GetLayersId() const {
#ifdef MOZ_BUILD_WEBRENDER
    return mWrRootId.mLayersId;
#else
    return mLayersId;
#endif
  }

  /**
   * Called when the layer tree that this protocol is connected to
   * is adopted by another compositor, and we need to switch APZCTreeManagers.
   */
  void ChildAdopted(RefPtr<APZCTreeManager> aAPZCTreeManager,
                    RefPtr<APZUpdater> aAPZUpdater);

  mozilla::ipc::IPCResult RecvSetKeyboardMap(const KeyboardMap& aKeyboardMap);

  mozilla::ipc::IPCResult RecvZoomToRect(
#ifdef MOZ_BUILD_WEBRENDER
      const SLGuidAndRenderRoot& aGuid,
#else
      const ScrollableLayerGuid& aGuid,
#endif
      const CSSRect& aRect, const uint32_t& aFlags);

  mozilla::ipc::IPCResult RecvContentReceivedInputBlock(
      const uint64_t& aInputBlockId, const bool& aPreventDefault);

  mozilla::ipc::IPCResult RecvSetTargetAPZC(
      const uint64_t& aInputBlockId,
#ifdef MOZ_BUILD_WEBRENDER
      nsTArray<SLGuidAndRenderRoot>&& aTargets
#else
      nsTArray<ScrollableLayerGuid>&& aTargets
#endif
  );

  mozilla::ipc::IPCResult RecvUpdateZoomConstraints(
#ifdef MOZ_BUILD_WEBRENDER
      const SLGuidAndRenderRoot& aGuid,
#else
      const ScrollableLayerGuid& aGuid,
#endif
      const MaybeZoomConstraints& aConstraints);

  mozilla::ipc::IPCResult RecvSetDPI(const float& aDpiValue);

  mozilla::ipc::IPCResult RecvSetAllowedTouchBehavior(
      const uint64_t& aInputBlockId, nsTArray<TouchBehaviorFlags>&& aValues);

  mozilla::ipc::IPCResult RecvStartScrollbarDrag(
#ifdef MOZ_BUILD_WEBRENDER
      const SLGuidAndRenderRoot& aGuid,
#else
      const ScrollableLayerGuid& aGuid,
#endif
      const AsyncDragMetrics& aDragMetrics);

  mozilla::ipc::IPCResult RecvStartAutoscroll(
#ifdef MOZ_BUILD_WEBRENDER
      const SLGuidAndRenderRoot& aGuid,
#else
      const ScrollableLayerGuid& aGuid,
#endif
      const ScreenPoint& aAnchorLocation);

  mozilla::ipc::IPCResult RecvStopAutoscroll(
#ifdef MOZ_BUILD_WEBRENDER
      const SLGuidAndRenderRoot& aGuid
#else
      const ScrollableLayerGuid& aGuid
#endif
  );

  mozilla::ipc::IPCResult RecvSetLongTapEnabled(const bool& aTapGestureEnabled);

  void ActorDestroy(ActorDestroyReason aWhy) override {}

 private:
#ifdef MOZ_BUILD_WEBRENDER
  bool IsGuidValid(const SLGuidAndRenderRoot& aGuid);

  WRRootId mWrRootId;
#else
  bool IsGuidValid(const ScrollableLayerGuid& aGuid);
  LayersId mLayersId;
#endif
  RefPtr<APZCTreeManager> mTreeManager;
  RefPtr<APZUpdater> mUpdater;
};

}  // namespace layers
}  // namespace mozilla

#endif  // mozilla_layers_APZCTreeManagerParent_h
