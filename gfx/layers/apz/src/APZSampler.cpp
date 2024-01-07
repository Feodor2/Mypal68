/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/layers/APZSampler.h"

#include "APZCTreeManager.h"
#include "AsyncPanZoomController.h"
#include "mozilla/ClearOnShutdown.h"
#include "mozilla/layers/APZThreadUtils.h"
#include "mozilla/layers/APZUtils.h"
#include "mozilla/layers/CompositorThread.h"
#include "mozilla/layers/LayerMetricsWrapper.h"
#include "mozilla/layers/SynchronousTask.h"
#include "TreeTraversal.h"
#ifdef MOZ_BUILD_WEBRENDER
#  include "mozilla/webrender/WebRenderAPI.h"
#endif

namespace mozilla {
namespace layers {

#ifdef MOZ_BUILD_WEBRENDER
StaticMutex APZSampler::sWindowIdLock;
StaticAutoPtr<std::unordered_map<uint64_t, RefPtr<APZSampler>>>
    APZSampler::sWindowIdMap;
#endif

APZSampler::APZSampler(const RefPtr<APZCTreeManager>& aApz
#ifdef MOZ_BUILD_WEBRENDER
                       ,
                       bool aIsUsingWebRender
#endif
                       )
    : mApz(aApz)
#ifdef MOZ_BUILD_WEBRENDER
      ,
      mIsUsingWebRender(aIsUsingWebRender),
      mThreadIdLock("APZSampler::mThreadIdLock"),
      mSampleTimeLock("APZSampler::mSampleTimeLock")
#endif
{
  MOZ_ASSERT(aApz);
  mApz->SetSampler(this);
}

APZSampler::~APZSampler() { mApz->SetSampler(nullptr); }

#ifdef MOZ_BUILD_WEBRENDER
void APZSampler::Destroy() {
  StaticMutexAutoLock lock(sWindowIdLock);
  if (mWindowId) {
    MOZ_ASSERT(sWindowIdMap);
    sWindowIdMap->erase(wr::AsUint64(*mWindowId));
  }
}

void APZSampler::SetWebRenderWindowId(const wr::WindowId& aWindowId) {
  StaticMutexAutoLock lock(sWindowIdLock);
  MOZ_ASSERT(!mWindowId);
  mWindowId = Some(aWindowId);
  if (!sWindowIdMap) {
    sWindowIdMap = new std::unordered_map<uint64_t, RefPtr<APZSampler>>();
    NS_DispatchToMainThread(NS_NewRunnableFunction(
        "APZUpdater::ClearOnShutdown", [] { ClearOnShutdown(&sWindowIdMap); }));
  }
  (*sWindowIdMap)[wr::AsUint64(aWindowId)] = this;
}

/*static*/
void APZSampler::SetSamplerThread(const wr::WrWindowId& aWindowId) {
  if (RefPtr<APZSampler> sampler = GetSampler(aWindowId)) {
    MutexAutoLock lock(sampler->mThreadIdLock);
    sampler->mSamplerThreadId = Some(PlatformThread::CurrentId());
  }
}

/*static*/
void APZSampler::SampleForWebRender(const wr::WrWindowId& aWindowId,
                                    wr::Transaction* aTransaction,
                                    const wr::DocumentId& aRenderRootId) {
  if (RefPtr<APZSampler> sampler = GetSampler(aWindowId)) {
    wr::TransactionWrapper txn(aTransaction);
    sampler->SampleForWebRender(txn, wr::RenderRootFromId(aRenderRootId));
  }
}

void APZSampler::SetSampleTime(const TimeStamp& aSampleTime) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  MutexAutoLock lock(mSampleTimeLock);
  mSampleTime = aSampleTime;
}

void APZSampler::SampleForWebRender(wr::TransactionWrapper& aTxn,
                                    wr::RenderRoot aRenderRoot) {
  AssertOnSamplerThread();
  TimeStamp sampleTime;
  {  // scope lock
    MutexAutoLock lock(mSampleTimeLock);

    // If mSampleTime is null we're in a startup phase where the
    // WebRenderBridgeParent hasn't yet provided us with a sample time.
    // If we're that early there probably aren't any APZ animations happening
    // anyway, so using Timestamp::Now() should be fine.
    sampleTime = mSampleTime.IsNull() ? TimeStamp::Now() : mSampleTime;
  }
  mApz->SampleForWebRender(aTxn, sampleTime, aRenderRoot);
}
#endif  // MOZ_BUILD_WEBRENDER

bool APZSampler::SampleAnimations(const LayerMetricsWrapper& aLayer,
                                  const TimeStamp& aSampleTime) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  // TODO: eventually we can drop the aLayer argument and just walk the APZ
  // tree directly in mApz.

  bool activeAnimations = false;

  ForEachNodePostOrder<ForwardIterator>(
      aLayer,
      [&activeAnimations, &aSampleTime](LayerMetricsWrapper aLayerMetrics) {
        if (AsyncPanZoomController* apzc = aLayerMetrics.GetApzc()) {
          apzc->ReportCheckerboard(aSampleTime);
          activeAnimations |= apzc->AdvanceAnimations(aSampleTime);
        }
      });

  return activeAnimations;
}

LayerToParentLayerMatrix4x4 APZSampler::ComputeTransformForScrollThumb(
    const LayerToParentLayerMatrix4x4& aCurrentTransform,
    const LayerMetricsWrapper& aContent, const ScrollbarData& aThumbData,
    bool aScrollbarIsDescendant,
    AsyncTransformComponentMatrix* aOutClipTransform) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  return mApz->ComputeTransformForScrollThumb(
      aCurrentTransform, aContent.GetTransform(), aContent.GetApzc(),
      aContent.Metrics(), aThumbData, aScrollbarIsDescendant,
      aOutClipTransform);
}

CSSRect APZSampler::GetCurrentAsyncLayoutViewport(
    const LayerMetricsWrapper& aLayer) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  MOZ_ASSERT(aLayer.GetApzc());
  return aLayer.GetApzc()->GetCurrentAsyncLayoutViewport(
      AsyncPanZoomController::eForCompositing);
}

ParentLayerPoint APZSampler::GetCurrentAsyncScrollOffset(
    const LayerMetricsWrapper& aLayer) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  MOZ_ASSERT(aLayer.GetApzc());
  return aLayer.GetApzc()->GetCurrentAsyncScrollOffset(
      AsyncPanZoomController::eForCompositing);
}

AsyncTransform APZSampler::GetCurrentAsyncTransform(
    const LayerMetricsWrapper& aLayer, AsyncTransformComponents aComponents) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  MOZ_ASSERT(aLayer.GetApzc());
  return aLayer.GetApzc()->GetCurrentAsyncTransform(
      AsyncPanZoomController::eForCompositing, aComponents);
}

AsyncTransformComponentMatrix APZSampler::GetOverscrollTransform(
    const LayerMetricsWrapper& aLayer) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  MOZ_ASSERT(aLayer.GetApzc());
  return aLayer.GetApzc()->GetOverscrollTransform(
      AsyncPanZoomController::eForCompositing);
}

AsyncTransformComponentMatrix
APZSampler::GetCurrentAsyncTransformWithOverscroll(
    const LayerMetricsWrapper& aLayer) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  MOZ_ASSERT(aLayer.GetApzc());
  return aLayer.GetApzc()->GetCurrentAsyncTransformWithOverscroll(
      AsyncPanZoomController::eForCompositing);
}

void APZSampler::MarkAsyncTransformAppliedToContent(
    const LayerMetricsWrapper& aLayer) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  MOZ_ASSERT(aLayer.GetApzc());
  aLayer.GetApzc()->MarkAsyncTransformAppliedToContent();
}

bool APZSampler::HasUnusedAsyncTransform(const LayerMetricsWrapper& aLayer) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  AsyncPanZoomController* apzc = aLayer.GetApzc();
  return apzc && !apzc->GetAsyncTransformAppliedToContent() &&
         !AsyncTransformComponentMatrix(
              apzc->GetCurrentAsyncTransform(
                  AsyncPanZoomController::eForCompositing))
              .IsIdentity();
}

ScrollableLayerGuid APZSampler::GetGuid(const LayerMetricsWrapper& aLayer) {
  MOZ_ASSERT(CompositorThreadHolder::IsInCompositorThread());
  AssertOnSamplerThread();

  MOZ_ASSERT(aLayer.GetApzc());
  return aLayer.GetApzc()->GetGuid();
}

void APZSampler::AssertOnSamplerThread() const {
  if (APZThreadUtils::GetThreadAssertionsEnabled()) {
    MOZ_ASSERT(IsSamplerThread());
  }
}

bool APZSampler::IsSamplerThread() const {
#ifdef MOZ_BUILD_WEBRENDER
  if (mIsUsingWebRender) {
    // If the sampler thread id isn't set yet then we cannot be running on the
    // sampler thread (because we will have the thread id before we run any
    // other C++ code on it, and this function is only ever invoked from C++
    // code), so return false in that scenario.
    MutexAutoLock lock(mThreadIdLock);
    return mSamplerThreadId && PlatformThread::CurrentId() == *mSamplerThreadId;
  }
#endif
  return CompositorThreadHolder::IsInCompositorThread();
}

#ifdef MOZ_BUILD_WEBRENDER
/*static*/
already_AddRefed<APZSampler> APZSampler::GetSampler(
    const wr::WrWindowId& aWindowId) {
  RefPtr<APZSampler> sampler;
  StaticMutexAutoLock lock(sWindowIdLock);
  if (sWindowIdMap) {
    auto it = sWindowIdMap->find(wr::AsUint64(aWindowId));
    if (it != sWindowIdMap->end()) {
      sampler = it->second;
    }
  }
  return sampler.forget();
}
#endif

}  // namespace layers
}  // namespace mozilla

#ifdef MOZ_BUILD_WEBRENDER
void apz_register_sampler(mozilla::wr::WrWindowId aWindowId) {
  mozilla::layers::APZSampler::SetSamplerThread(aWindowId);
}

void apz_sample_transforms(mozilla::wr::WrWindowId aWindowId,
                           mozilla::wr::Transaction* aTransaction,
                           mozilla::wr::DocumentId aDocumentId) {
  mozilla::layers::APZSampler::SampleForWebRender(aWindowId, aTransaction,
                                                  aDocumentId);
}

void apz_deregister_sampler(mozilla::wr::WrWindowId aWindowId) {}
#endif
