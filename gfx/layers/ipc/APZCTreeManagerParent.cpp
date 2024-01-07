/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/layers/APZCTreeManagerParent.h"

#include "apz/src/APZCTreeManager.h"
#include "mozilla/layers/APZThreadUtils.h"
#include "mozilla/layers/APZUpdater.h"

namespace mozilla {
namespace layers {

APZCTreeManagerParent::APZCTreeManagerParent(
#ifdef MOZ_BUILD_WEBRENDER
    WRRootId aWrRootId,
#else
    LayersId aLayersId,
#endif
    RefPtr<APZCTreeManager> aAPZCTreeManager, RefPtr<APZUpdater> aAPZUpdater)
    :
#ifdef MOZ_BUILD_WEBRENDER
      mWrRootId(aWrRootId),
#else
      mLayersId(aLayersId),
#endif
      mTreeManager(std::move(aAPZCTreeManager)),
      mUpdater(std::move(aAPZUpdater)) {
  MOZ_ASSERT(mTreeManager != nullptr);
  MOZ_ASSERT(mUpdater != nullptr);
  MOZ_ASSERT(mUpdater->HasTreeManager(mTreeManager));
}

APZCTreeManagerParent::~APZCTreeManagerParent() {}

void APZCTreeManagerParent::ChildAdopted(
    RefPtr<APZCTreeManager> aAPZCTreeManager, RefPtr<APZUpdater> aAPZUpdater) {
  MOZ_ASSERT(aAPZCTreeManager != nullptr);
  MOZ_ASSERT(aAPZUpdater != nullptr);
  MOZ_ASSERT(aAPZUpdater->HasTreeManager(aAPZCTreeManager));
  mTreeManager = std::move(aAPZCTreeManager);
  mUpdater = std::move(aAPZUpdater);
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvSetKeyboardMap(
    const KeyboardMap& aKeyboardMap) {
  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(mWrRootId),
#endif
      NewRunnableMethod<KeyboardMap>(
          "layers::IAPZCTreeManager::SetKeyboardMap", mTreeManager,
          &IAPZCTreeManager::SetKeyboardMap, aKeyboardMap));

  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvZoomToRect(
#ifdef MOZ_BUILD_WEBRENDER
    const SLGuidAndRenderRoot& aGuid,
#else
    const ScrollableLayerGuid& aGuid,
#endif
    const CSSRect& aRect, const uint32_t& aFlags) {
  if (!IsGuidValid(aGuid)) {
    return IPC_FAIL_NO_REASON(this);
  }

  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(aGuid.GetWRRootId()),
      NewRunnableMethod<SLGuidAndRenderRoot,
#else
      NewRunnableMethod<ScrollableLayerGuid,
#endif
                        CSSRect, uint32_t>(
          "layers::IAPZCTreeManager::ZoomToRect", mTreeManager,
          &IAPZCTreeManager::ZoomToRect, aGuid, aRect, aFlags));
  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvContentReceivedInputBlock(
    const uint64_t& aInputBlockId, const bool& aPreventDefault) {
  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(mWrRootId),
#endif
      NewRunnableMethod<uint64_t, bool>(
          "layers::IAPZCTreeManager::ContentReceivedInputBlock", mTreeManager,
          &IAPZCTreeManager::ContentReceivedInputBlock, aInputBlockId,
          aPreventDefault));

  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvSetTargetAPZC(
    const uint64_t& aInputBlockId,
#ifdef MOZ_BUILD_WEBRENDER
    nsTArray<SLGuidAndRenderRoot>&& aTargets
#else
    nsTArray<ScrollableLayerGuid>&& aTargets
#endif
) {
#ifdef MOZ_BUILD_WEBRENDER
  UpdaterQueueSelector selector(mWrRootId.mLayersId);
#endif
  for (size_t i = 0; i < aTargets.Length(); i++) {
    if (!IsGuidValid(aTargets[i])) {
      return IPC_FAIL_NO_REASON(this);
    }
#ifdef MOZ_BUILD_WEBRENDER
    selector.mRenderRoots += aTargets[i].mRenderRoot;
#endif
  }
  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      selector,
#endif
      NewRunnableMethod<uint64_t, StoreCopyPassByRRef<nsTArray<
#ifdef MOZ_BUILD_WEBRENDER
                                      SLGuidAndRenderRoot
#else
                                      ScrollableLayerGuid
#endif
                                      >>>(
          "layers::IAPZCTreeManager::SetTargetAPZC", mTreeManager,
          &IAPZCTreeManager::SetTargetAPZC, aInputBlockId, aTargets));

  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvUpdateZoomConstraints(
#ifdef MOZ_BUILD_WEBRENDER
    const SLGuidAndRenderRoot& aGuid,
#else
    const ScrollableLayerGuid& aGuid,
#endif
    const MaybeZoomConstraints& aConstraints) {
  if (!IsGuidValid(aGuid)) {
    return IPC_FAIL_NO_REASON(this);
  }
  mTreeManager->UpdateZoomConstraints(aGuid, aConstraints);
  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvSetDPI(
    const float& aDpiValue) {
  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(mWrRootId),
#endif
      NewRunnableMethod<float>("layers::IAPZCTreeManager::SetDPI", mTreeManager,
                               &IAPZCTreeManager::SetDPI, aDpiValue));
  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvSetAllowedTouchBehavior(
    const uint64_t& aInputBlockId, nsTArray<TouchBehaviorFlags>&& aValues) {
  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(mWrRootId),
#endif
      NewRunnableMethod<uint64_t,
                        StoreCopyPassByRRef<nsTArray<TouchBehaviorFlags>>>(
          "layers::IAPZCTreeManager::SetAllowedTouchBehavior", mTreeManager,
          &IAPZCTreeManager::SetAllowedTouchBehavior, aInputBlockId,
          std::move(aValues)));

  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvStartScrollbarDrag(
#ifdef MOZ_BUILD_WEBRENDER
    const SLGuidAndRenderRoot& aGuid,
#else
    const ScrollableLayerGuid& aGuid,
#endif
    const AsyncDragMetrics& aDragMetrics) {
  if (!IsGuidValid(aGuid)) {
    return IPC_FAIL_NO_REASON(this);
  }

  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(aGuid.GetWRRootId()),
#endif
      NewRunnableMethod<
#ifdef MOZ_BUILD_WEBRENDER
          SLGuidAndRenderRoot,
#else
          ScrollableLayerGuid,
#endif
          AsyncDragMetrics>("layers::IAPZCTreeManager::StartScrollbarDrag",
                            mTreeManager, &IAPZCTreeManager::StartScrollbarDrag,
                            aGuid, aDragMetrics));

  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvStartAutoscroll(
#ifdef MOZ_BUILD_WEBRENDER
    const SLGuidAndRenderRoot& aGuid,
#else
    const ScrollableLayerGuid& aGuid,
#endif
    const ScreenPoint& aAnchorLocation) {
  // Unlike RecvStartScrollbarDrag(), this message comes from the parent
  // process (via nsBaseWidget::mAPZC) rather than from the child process
  // (via BrowserChild::mApzcTreeManager), so there is no need to check the
  // layers id against mWrRootId (and in any case, it wouldn't match, because
  // mWrRootId stores the parent process's layers id, while nsBaseWidget is
  // sending the child process's layers id).

  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(mWrRootId),
#endif
      NewRunnableMethod<
#ifdef MOZ_BUILD_WEBRENDER
          SLGuidAndRenderRoot,
#else
          ScrollableLayerGuid,
#endif
          ScreenPoint>("layers::IAPZCTreeManager::StartAutoscroll",
                       mTreeManager, &IAPZCTreeManager::StartAutoscroll, aGuid,
                       aAnchorLocation));

  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvStopAutoscroll(
#ifdef MOZ_BUILD_WEBRENDER
    const SLGuidAndRenderRoot& aGuid
#else
    const ScrollableLayerGuid& aGuid
#endif
) {
  // See RecvStartAutoscroll() for why we don't check the layers id.

  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(mWrRootId),
#endif
      NewRunnableMethod<
#ifdef MOZ_BUILD_WEBRENDER
          SLGuidAndRenderRoot
#else
          ScrollableLayerGuid
#endif
          >("layers::IAPZCTreeManager::StopAutoscroll", mTreeManager,
            &IAPZCTreeManager::StopAutoscroll, aGuid));

  return IPC_OK();
}

mozilla::ipc::IPCResult APZCTreeManagerParent::RecvSetLongTapEnabled(
    const bool& aLongTapEnabled) {
  mUpdater->RunOnControllerThread(
#ifdef MOZ_BUILD_WEBRENDER
      UpdaterQueueSelector(mWrRootId),
#endif
      NewRunnableMethod<bool>(
          "layers::IAPZCTreeManager::SetLongTapEnabled", mTreeManager,
          &IAPZCTreeManager::SetLongTapEnabled, aLongTapEnabled));

  return IPC_OK();
}

bool APZCTreeManagerParent::IsGuidValid(
#ifdef MOZ_BUILD_WEBRENDER
    const SLGuidAndRenderRoot& aGuid) {
  if (aGuid.mScrollableLayerGuid.mLayersId != mWrRootId.mLayersId) {
    NS_ERROR("Unexpected layers id");
    return false;
  }
  if (mWrRootId.mRenderRoot == wr::RenderRoot::Content) {
    // If this APZCTreeManagerParent is for a content process IPDL bridge, then
    // all the render root references that come over the bridge must be for
    // the content render root.
    if (aGuid.mRenderRoot != wr::RenderRoot::Content) {
      NS_ERROR("Unexpected render root");
      return false;
    }
  }
#else
    const ScrollableLayerGuid& aGuid) {
  if (aGuid.mLayersId != mLayersId) {
    NS_ERROR("Unexpected layers id");
    return false;
  }
#endif
  return true;
}

}  // namespace layers
}  // namespace mozilla
