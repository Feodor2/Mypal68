/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_layers_CompositorBridgeParent_h
#define mozilla_layers_CompositorBridgeParent_h

// Enable this pref to turn on compositor performance warning.
// This will print warnings if the compositor isn't meeting
// its responsiveness objectives:
//    1) Compose a frame within 15ms of receiving a ScheduleCompositeCall
//    2) Unless a frame was composited within the throttle threshold in
//       which the deadline will be 15ms + throttle threshold
//#define COMPOSITOR_PERFORMANCE_WARNING

#include <stdint.h>              // for uint64_t
#include "Layers.h"              // for Layer
#include "mozilla/Assertions.h"  // for MOZ_ASSERT_HELPER2
#include "mozilla/Attributes.h"  // for override
#include "mozilla/Maybe.h"
#include "mozilla/Monitor2.h"   // for Monitor
#include "mozilla/RefPtr.h"     // for RefPtr
#include "mozilla/TimeStamp.h"  // for TimeStamp
#include "mozilla/dom/ipc/IdType.h"
#include "mozilla/gfx/Point.h"  // for IntSize
#include "mozilla/ipc/ProtocolUtils.h"
#include "mozilla/ipc/SharedMemory.h"
#include "mozilla/layers/CompositionRecorder.h"
#include "mozilla/layers/CompositorController.h"
#include "mozilla/layers/CompositorOptions.h"
#include "mozilla/layers/CompositorVsyncSchedulerOwner.h"
#include "mozilla/layers/GeckoContentController.h"
#include "mozilla/layers/ISurfaceAllocator.h"  // for ShmemAllocator
#include "mozilla/layers/LayersMessages.h"     // for TargetConfig
#include "mozilla/layers/MetricsSharingController.h"
#include "mozilla/layers/PCompositorBridgeParent.h"
#include "mozilla/layers/APZTestData.h"
#ifdef MOZ_BUILD_WEBRENDER
#  include "mozilla/webrender/WebRenderTypes.h"
#  include "mozilla/webrender/RenderThread.h"
#endif
#include "mozilla/widget/CompositorWidget.h"
#include "nsISupportsImpl.h"
#include "ThreadSafeRefcountingWithMainThreadDestruction.h"
#include "mozilla/layers/UiCompositorControllerParent.h"
#include "mozilla/VsyncDispatcher.h"

class MessageLoop;
class nsIWidget;

namespace mozilla {

class CancelableRunnable;

namespace gfx {
class DrawTarget;
class GPUProcessManager;
class GPUParent;
}  // namespace gfx

namespace ipc {
class Shmem;
#ifdef FUZZING
class ProtocolFuzzerHelper;
#endif
}  // namespace ipc

namespace layers {

class APZCTreeManager;
class APZCTreeManagerParent;
class APZSampler;
class APZUpdater;
class AsyncCompositionManager;
class Compositor;
class CompositorAnimationStorage;
class CompositorBridgeParent;
class CompositorManagerParent;
class CompositorVsyncScheduler;
class HostLayerManager;
class IAPZCTreeManager;
class LayerTransactionParent;
class PAPZParent;
class ContentCompositorBridgeParent;
class CompositorThreadHolder;
class InProcessCompositorSession;
#ifdef MOZ_BUILD_WEBRENDER
class AsyncImagePipelineManager;
class WebRenderBridgeParent;
#endif

struct ScopedLayerTreeRegistration {
  ScopedLayerTreeRegistration(APZCTreeManager* aApzctm, LayersId aLayersId,
                              Layer* aRoot,
                              GeckoContentController* aController);
  ~ScopedLayerTreeRegistration();

 private:
  LayersId mLayersId;
};

class CompositorBridgeParentBase : public PCompositorBridgeParent,
                                   public HostIPCAllocator,
                                   public ShmemAllocator,
                                   public MetricsSharingController {
  friend class PCompositorBridgeParent;

 public:
  explicit CompositorBridgeParentBase(CompositorManagerParent* aManager);

  virtual void ShadowLayersUpdated(LayerTransactionParent* aLayerTree,
                                   const TransactionInfo& aInfo,
                                   bool aHitTestUpdate) = 0;

  virtual AsyncCompositionManager* GetCompositionManager(
      LayerTransactionParent* aLayerTree) {
    return nullptr;
  }

  virtual void NotifyClearCachedResources(LayerTransactionParent* aLayerTree) {}

  virtual void ScheduleComposite(LayerTransactionParent* aLayerTree) {}
  virtual bool SetTestSampleTime(const LayersId& aId, const TimeStamp& aTime) {
    return true;
  }
  virtual void LeaveTestMode(const LayersId& aId) {}
  enum class TransformsToSkip : uint8_t { NoneOfThem = 0, APZ = 1 };
  virtual void ApplyAsyncProperties(LayerTransactionParent* aLayerTree,
                                    TransformsToSkip aSkip) = 0;
  virtual void SetTestAsyncScrollOffset(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId,
#else
      const LayersId& aLayersId,
#endif
      const ScrollableLayerGuid::ViewID& aScrollId, const CSSPoint& aPoint) = 0;
  virtual void SetTestAsyncZoom(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId,
#else
      const LayersId& aLayersId,
#endif
      const ScrollableLayerGuid::ViewID& aScrollId,
      const LayerToParentLayerScale& aZoom) = 0;
  virtual void FlushApzRepaints(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId
#else
      const LayersId& aLayersId
#endif
      ) = 0;
  virtual void GetAPZTestData(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId,
#else
      const LayersId& aLayersId,
#endif
      APZTestData* aOutData) {
  }
  virtual void SetConfirmedTargetAPZC(
      const LayersId& aLayersId, const uint64_t& aInputBlockId,
#ifdef MOZ_BUILD_WEBRENDER
      const nsTArray<SLGuidAndRenderRoot>& aTargets
#else
      const nsTArray<ScrollableLayerGuid>& aTargets
#endif
      ) = 0;
  virtual void UpdatePaintTime(LayerTransactionParent* aLayerTree,
                               const TimeDuration& aPaintTime) {}
  virtual void RegisterPayloads(LayerTransactionParent* aLayerTree,
                                const nsTArray<CompositionPayload>& aPayload) {}

  ShmemAllocator* AsShmemAllocator() override { return this; }

  CompositorBridgeParentBase* AsCompositorBridgeParentBase() override {
    return this;
  }

  mozilla::ipc::IPCResult RecvSyncWithCompositor() { return IPC_OK(); }

  mozilla::ipc::IPCResult Recv__delete__() override { return IPC_OK(); }

  virtual void ObserveLayersUpdate(LayersId aLayersId,
                                   LayersObserverEpoch aEpoch,
                                   bool aActive) = 0;

  // HostIPCAllocator
  base::ProcessId GetChildProcessId() override;
  void NotifyNotUsed(PTextureParent* aTexture,
                     uint64_t aTransactionId) override;
  void SendAsyncMessage(
      const nsTArray<AsyncParentMessageData>& aMessage) override;

  // ShmemAllocator
  bool AllocShmem(size_t aSize,
                  mozilla::ipc::SharedMemory::SharedMemoryType aType,
                  mozilla::ipc::Shmem* aShmem) override;
  bool AllocUnsafeShmem(size_t aSize,
                        mozilla::ipc::SharedMemory::SharedMemoryType aType,
                        mozilla::ipc::Shmem* aShmem) override;
  void DeallocShmem(mozilla::ipc::Shmem& aShmem) override;

  // MetricsSharingController
  NS_IMETHOD_(MozExternalRefCountType) AddRef() override {
    return HostIPCAllocator::AddRef();
  }
  NS_IMETHOD_(MozExternalRefCountType) Release() override {
    return HostIPCAllocator::Release();
  }
  base::ProcessId RemotePid() override;
  bool StartSharingMetrics(mozilla::ipc::SharedMemoryBasic::Handle aHandle,
                           CrossProcessMutexHandle aMutexHandle,
                           LayersId aLayersId, uint32_t aApzcId) override;
  bool StopSharingMetrics(ScrollableLayerGuid::ViewID aScrollId,
                          uint32_t aApzcId) override;

  virtual bool IsRemote() const { return false; }

  virtual void ForceComposeToTarget(gfx::DrawTarget* aTarget,
                                    const gfx::IntRect* aRect = nullptr) {
    MOZ_CRASH();
  }

#ifdef MOZ_BUILD_WEBRENDER
  virtual void NotifyMemoryPressure() {}
  virtual void AccumulateMemoryReport(wr::MemoryReport*) {}
#endif

 protected:
  virtual ~CompositorBridgeParentBase();

  virtual PAPZParent* AllocPAPZParent(const LayersId& layersId) = 0;
  virtual bool DeallocPAPZParent(PAPZParent* aActor) = 0;

  virtual PAPZCTreeManagerParent* AllocPAPZCTreeManagerParent(
      const LayersId& layersId) = 0;
  virtual bool DeallocPAPZCTreeManagerParent(
      PAPZCTreeManagerParent* aActor) = 0;

  virtual PLayerTransactionParent* AllocPLayerTransactionParent(
      const nsTArray<LayersBackend>& layersBackendHints,
      const LayersId& id) = 0;
  virtual bool DeallocPLayerTransactionParent(
      PLayerTransactionParent* aActor) = 0;

  virtual PTextureParent* AllocPTextureParent(
      const SurfaceDescriptor& aSharedData, const ReadLockDescriptor& aReadLock,
      const LayersBackend& aBackend, const TextureFlags& aTextureFlags,
      const LayersId& id, const uint64_t& aSerial
#ifdef MOZ_BUILD_WEBRENDER
      ,
      const MaybeExternalImageId& aExternalImageId
#endif
      ) = 0;
  virtual bool DeallocPTextureParent(PTextureParent* aActor) = 0;

#ifdef MOZ_BUILD_WEBRENDER
  virtual PWebRenderBridgeParent* AllocPWebRenderBridgeParent(
      const PipelineId& pipelineId, const LayoutDeviceIntSize& aSize) = 0;
  virtual bool DeallocPWebRenderBridgeParent(
      PWebRenderBridgeParent* aActor) = 0;
  virtual mozilla::ipc::IPCResult RecvFlushRenderingAsync() = 0;
#endif

  virtual PCompositorWidgetParent* AllocPCompositorWidgetParent(
      const CompositorWidgetInitData& aInitData) = 0;
  virtual bool DeallocPCompositorWidgetParent(
      PCompositorWidgetParent* aActor) = 0;

  virtual mozilla::ipc::IPCResult RecvRemotePluginsReady() = 0;
  virtual mozilla::ipc::IPCResult RecvAdoptChild(const LayersId& id) = 0;
  virtual mozilla::ipc::IPCResult RecvForcePresent() = 0;
  virtual mozilla::ipc::IPCResult RecvNotifyRegionInvalidated(
      const nsIntRegion& region) = 0;
  virtual mozilla::ipc::IPCResult RecvRequestNotifyAfterRemotePaint() = 0;
  virtual mozilla::ipc::IPCResult RecvAllPluginsCaptured() = 0;
  virtual mozilla::ipc::IPCResult RecvBeginRecording(
      const TimeStamp& aRecordingStart, BeginRecordingResolver&& aResolve) = 0;
  virtual mozilla::ipc::IPCResult RecvEndRecording(bool* aOutSuccess) = 0;
  virtual mozilla::ipc::IPCResult RecvInitialize(
      const LayersId& rootLayerTreeId) = 0;
  virtual mozilla::ipc::IPCResult RecvGetFrameUniformity(
      FrameUniformityData* data) = 0;
  virtual mozilla::ipc::IPCResult RecvWillClose() = 0;
  virtual mozilla::ipc::IPCResult RecvPause() = 0;
  virtual mozilla::ipc::IPCResult RecvResume() = 0;
  virtual mozilla::ipc::IPCResult RecvResumeAsync() = 0;
  virtual mozilla::ipc::IPCResult RecvNotifyChildCreated(
      const LayersId& id, CompositorOptions* compositorOptions) = 0;
  virtual mozilla::ipc::IPCResult RecvMapAndNotifyChildCreated(
      const LayersId& id, const ProcessId& owner,
      CompositorOptions* compositorOptions) = 0;
  virtual mozilla::ipc::IPCResult RecvNotifyChildRecreated(
      const LayersId& id, CompositorOptions* compositorOptions) = 0;
  virtual mozilla::ipc::IPCResult RecvMakeSnapshot(
      const SurfaceDescriptor& inSnapshot, const IntRect& dirtyRect) = 0;
  virtual mozilla::ipc::IPCResult RecvFlushRendering() = 0;
  virtual mozilla::ipc::IPCResult RecvWaitOnTransactionProcessed() = 0;
  virtual mozilla::ipc::IPCResult RecvStartFrameTimeRecording(
      const int32_t& bufferSize, uint32_t* startIndex) = 0;
  virtual mozilla::ipc::IPCResult RecvStopFrameTimeRecording(
      const uint32_t& startIndex, nsTArray<float>* intervals) = 0;
  virtual mozilla::ipc::IPCResult RecvCheckContentOnlyTDR(
      const uint32_t& sequenceNum, bool* isContentOnlyTDR) = 0;

  bool mCanSend;

 private:
  RefPtr<CompositorManagerParent> mCompositorManager;
};

MOZ_MAKE_ENUM_CLASS_BITWISE_OPERATORS(
    CompositorBridgeParentBase::TransformsToSkip)

class CompositorBridgeParent final : public CompositorBridgeParentBase,
                                     public CompositorController,
                                     public CompositorVsyncSchedulerOwner {
  friend class CompositorThreadHolder;
  friend class InProcessCompositorSession;
  friend class gfx::GPUProcessManager;
  friend class gfx::GPUParent;
  friend class PCompositorBridgeParent;
#ifdef FUZZING
  friend class mozilla::ipc::ProtocolFuzzerHelper;
#endif

 public:
  NS_IMETHOD_(MozExternalRefCountType) AddRef() override {
    return CompositorBridgeParentBase::AddRef();
  }
  NS_IMETHOD_(MozExternalRefCountType) Release() override {
    return CompositorBridgeParentBase::Release();
  }

  explicit CompositorBridgeParent(CompositorManagerParent* aManager,
                                  CSSToLayoutDeviceScale aScale,
                                  const TimeDuration& aVsyncRate,
                                  const CompositorOptions& aOptions,
                                  bool aUseExternalSurfaceSize,
                                  const gfx::IntSize& aSurfaceSize);

  void InitSameProcess(widget::CompositorWidget* aWidget,
                       const LayersId& aLayerTreeId);

  mozilla::ipc::IPCResult RecvInitialize(
      const LayersId& aRootLayerTreeId) override;
  mozilla::ipc::IPCResult RecvGetFrameUniformity(
      FrameUniformityData* aOutData) override;
  mozilla::ipc::IPCResult RecvWillClose() override;
  mozilla::ipc::IPCResult RecvPause() override;
  mozilla::ipc::IPCResult RecvResume() override;
  mozilla::ipc::IPCResult RecvResumeAsync() override;
  mozilla::ipc::IPCResult RecvNotifyChildCreated(
      const LayersId& child, CompositorOptions* aOptions) override;
  mozilla::ipc::IPCResult RecvMapAndNotifyChildCreated(
      const LayersId& child, const base::ProcessId& pid,
      CompositorOptions* aOptions) override;
  mozilla::ipc::IPCResult RecvNotifyChildRecreated(
      const LayersId& child, CompositorOptions* aOptions) override;
  mozilla::ipc::IPCResult RecvAdoptChild(const LayersId& child) override;
  mozilla::ipc::IPCResult RecvMakeSnapshot(const SurfaceDescriptor& aInSnapshot,
                                           const gfx::IntRect& aRect) override;
  mozilla::ipc::IPCResult RecvFlushRendering() override;
#ifdef MOZ_BUILD_WEBRENDER
  mozilla::ipc::IPCResult RecvFlushRenderingAsync() override;
#endif
  mozilla::ipc::IPCResult RecvWaitOnTransactionProcessed() override;
  mozilla::ipc::IPCResult RecvForcePresent() override;

  mozilla::ipc::IPCResult RecvNotifyRegionInvalidated(
      const nsIntRegion& aRegion) override;
  mozilla::ipc::IPCResult RecvStartFrameTimeRecording(
      const int32_t& aBufferSize, uint32_t* aOutStartIndex) override;
  mozilla::ipc::IPCResult RecvStopFrameTimeRecording(
      const uint32_t& aStartIndex, nsTArray<float>* intervals) override;

  mozilla::ipc::IPCResult RecvCheckContentOnlyTDR(
      const uint32_t& sequenceNum, bool* isContentOnlyTDR) override {
    return IPC_OK();
  }

  // Unused for chrome <-> compositor communication (which this class does).
  // @see ContentCompositorBridgeParent::RecvRequestNotifyAfterRemotePaint
  mozilla::ipc::IPCResult RecvRequestNotifyAfterRemotePaint() override {
    return IPC_OK();
  };

  mozilla::ipc::IPCResult RecvAllPluginsCaptured() override;
  mozilla::ipc::IPCResult RecvBeginRecording(
      const TimeStamp& aRecordingStart,
      BeginRecordingResolver&& aResolve) override;
  mozilla::ipc::IPCResult RecvEndRecording(bool* aOutSuccess) override;

#ifdef MOZ_BUILD_WEBRENDER
  void NotifyMemoryPressure() override;
  void AccumulateMemoryReport(wr::MemoryReport*) override;
#endif

  void ActorDestroy(ActorDestroyReason why) override;

  void ShadowLayersUpdated(LayerTransactionParent* aLayerTree,
                           const TransactionInfo& aInfo,
                           bool aHitTestUpdate) override;
  void ScheduleComposite(LayerTransactionParent* aLayerTree) override;
  bool SetTestSampleTime(const LayersId& aId, const TimeStamp& aTime) override;
  void LeaveTestMode(const LayersId& aId) override;
  void ApplyAsyncProperties(LayerTransactionParent* aLayerTree,
                            TransformsToSkip aSkip) override;
  CompositorAnimationStorage* GetAnimationStorage();
  void SetTestAsyncScrollOffset(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId,
#else
      const LayersId& aLayersId,
#endif
      const ScrollableLayerGuid::ViewID& aScrollId,
      const CSSPoint& aPoint) override;
  void SetTestAsyncZoom(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId,
#else
      const LayersId& aLayersId,
#endif
      const ScrollableLayerGuid::ViewID& aScrollId,
      const LayerToParentLayerScale& aZoom) override;
  void FlushApzRepaints(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId
#else
      const LayersId& aLayersId
#endif
      ) override;
  void GetAPZTestData(
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId,
#else
      const LayersId& aLayersId,
#endif
          APZTestData* aOutData) override;
  void SetConfirmedTargetAPZC(const LayersId& aLayersId,
                              const uint64_t& aInputBlockId,
#ifdef MOZ_BUILD_WEBRENDER
                              const nsTArray<SLGuidAndRenderRoot>& aTargets
#else
                              const nsTArray<ScrollableLayerGuid>& aTargets
#endif
                              ) override;
  AsyncCompositionManager* GetCompositionManager(
      LayerTransactionParent* aLayerTree) override {
    return mCompositionManager;
  }
  void SetFixedLayerMargins(ScreenIntCoord aTop, ScreenIntCoord aBottom);

  PTextureParent* AllocPTextureParent(
      const SurfaceDescriptor& aSharedData, const ReadLockDescriptor& aReadLock,
      const LayersBackend& aLayersBackend, const TextureFlags& aFlags,
      const LayersId& aId, const uint64_t& aSerial
#ifdef MOZ_BUILD_WEBRENDER
      ,
      const wr::MaybeExternalImageId& aExternalImageId
#endif
      ) override;
  bool DeallocPTextureParent(PTextureParent* actor) override;

  bool IsSameProcess() const override;

#ifdef MOZ_BUILD_WEBRENDER
  void NotifyWebRenderContextPurge();
  void NotifyPipelineRendered(const wr::PipelineId& aPipelineId,
                              const wr::Epoch& aEpoch,
                              const VsyncId& aCompositeStartId,
                              TimeStamp& aCompositeStart,
                              TimeStamp& aRenderStart, TimeStamp& aCompositeEnd,
                              wr::RendererStats* aStats = nullptr);
  void NotifyDidSceneBuild(const nsTArray<wr::RenderRoot>& aRenderRoots,
                           RefPtr<wr::WebRenderPipelineInfo> aInfo);
  RefPtr<AsyncImagePipelineManager> GetAsyncImagePipelineManager() const;
#endif

  PCompositorWidgetParent* AllocPCompositorWidgetParent(
      const CompositorWidgetInitData& aInitData) override;
  bool DeallocPCompositorWidgetParent(PCompositorWidgetParent* aActor) override;

  void ObserveLayersUpdate(LayersId aLayersId, LayersObserverEpoch aEpoch,
                           bool aActive) override {}

  /**
   * This forces the is-first-paint flag to true. This is intended to
   * be called by the widget code when it loses its viewport information
   * (or for whatever reason wants to refresh the viewport information).
   * The information refresh happens because the compositor will call
   * SetFirstPaintViewport on the next frame of composition.
   */
  void ForceIsFirstPaint();

  static void SetShadowProperties(Layer* aLayer);

  void NotifyChildCreated(LayersId aChild);

  void AsyncRender();

  // Can be called from any thread
  void ScheduleRenderOnCompositorThread(
#ifdef MOZ_BUILD_WEBRENDER
      const wr::RenderRootSet& aRenderRoots
#endif
      ) override;
  void SchedulePauseOnCompositorThread();
  void InvalidateOnCompositorThread();
  /**
   * Returns true if a surface was obtained and the resume succeeded; false
   * otherwise.
   */
  bool ScheduleResumeOnCompositorThread();
  bool ScheduleResumeOnCompositorThread(int x, int y, int width, int height);

  void ScheduleComposition(
#ifdef MOZ_BUILD_WEBRENDER
      const wr::RenderRootSet& aRenderRoots = wr::RenderRootSet()
#endif
  );

  void NotifyShadowTreeTransaction(LayersId aId, bool aIsFirstPaint,
                                   const FocusTarget& aFocusTarget,
                                   bool aScheduleComposite,
                                   uint32_t aPaintSequenceNumber,
                                   bool aIsRepeatTransaction,
                                   bool aHitTestUpdate);

  void UpdatePaintTime(LayerTransactionParent* aLayerTree,
                       const TimeDuration& aPaintTime) override;
  void RegisterPayloads(LayerTransactionParent* aLayerTree,
                        const nsTArray<CompositionPayload>& aPayload) override;

  /**
   * Check rotation info and schedule a rendering task if needed.
   * Only can be called from compositor thread.
   */
  void ScheduleRotationOnCompositorThread(const TargetConfig& aTargetConfig,
                                          bool aIsFirstPaint);

  /**
   * Returns the unique layer tree identifier that corresponds to the root
   * tree of this compositor.
   */
  LayersId RootLayerTreeId();

  /**
   * Notify local and remote layer trees connected to this compositor that
   * the compositor's local device is being reset. All layers must be
   * invalidated to clear any cached TextureSources.
   *
   * This must be called on the compositor thread.
   */
  void InvalidateRemoteLayers();

  /**
   * Returns a pointer to the CompositorBridgeParent corresponding to the given
   * ID.
   */
  static CompositorBridgeParent* GetCompositorBridgeParent(uint64_t id);

  /**
   * Notify the compositor for the given layer tree that vsync has occurred.
   */
  static void NotifyVsync(const VsyncEvent& aVsync, const LayersId& aLayersId);

  /**
   * Set aController as the pan/zoom callback for the subtree referred
   * to by aLayersId.
   *
   * Must run on content main thread.
   */
  static void SetControllerForLayerTree(LayersId aLayersId,
                                        GeckoContentController* aController);

  struct LayerTreeState {
    LayerTreeState();
    ~LayerTreeState();
    RefPtr<Layer> mRoot;
    RefPtr<GeckoContentController> mController;
    APZCTreeManagerParent* mApzcTreeManagerParent;
    RefPtr<CompositorBridgeParent> mParent;
    HostLayerManager* mLayerManager;
#ifdef MOZ_BUILD_WEBRENDER
    RefPtr<WebRenderBridgeParent> mWrBridge;
#endif
    // Pointer to the ContentCompositorBridgeParent. Used by APZCs to share
    // their FrameMetrics with the corresponding child process that holds
    // the PCompositorBridgeChild
    ContentCompositorBridgeParent* mContentCompositorBridgeParent;
    TargetConfig mTargetConfig;
    LayerTransactionParent* mLayerTree;
    nsTArray<PluginWindowData> mPluginData;
    bool mUpdatedPluginDataAvailable;

    CompositorController* GetCompositorController() const;
    MetricsSharingController* CrossProcessSharingController() const;
    MetricsSharingController* InProcessSharingController() const;
    RefPtr<UiCompositorControllerParent> mUiControllerParent;
  };

  /**
   * Lookup the indirect shadow tree for |aId| and return it if it
   * exists.  Otherwise null is returned.  This must only be called on
   * the compositor thread.
   */
  static LayerTreeState* GetIndirectShadowTree(LayersId aId);

  /**
   * Lookup the indirect shadow tree for |aId|, call the function object and
   * return true if found. If not found, return false.
   */
  static bool CallWithIndirectShadowTree(
      LayersId aId, const std::function<void(LayerTreeState&)>& aFunc);

  /**
   * Given the layers id for a content process, get the APZCTreeManagerParent
   * for the corresponding *root* layers id. That is, the APZCTreeManagerParent,
   * if one is found, will always be connected to the parent process rather
   * than a content process. Note that unless the compositor process is
   * separated this is expected to return null, because if the compositor is
   * living in the gecko parent process then there is no APZCTreeManagerParent
   * for the parent process.
   */
  static APZCTreeManagerParent* GetApzcTreeManagerParentForRoot(
      LayersId aContentLayersId);
  /**
   * Same as the GetApzcTreeManagerParentForRoot function, but returns
   * the GeckoContentController for the parent process.
   */
  static GeckoContentController* GetGeckoContentControllerForRoot(
      LayersId aContentLayersId);

#if defined(XP_WIN) || defined(MOZ_WIDGET_GTK)
  /**
   * Calculates and requests the main thread update plugin positioning, clip,
   * and visibility via ipc.
   */
  bool UpdatePluginWindowState(LayersId aId);

  /**
   * Plugin visibility helpers for the apz (main thread) and compositor
   * thread.
   */
  void ScheduleShowAllPluginWindows() override;
  void ScheduleHideAllPluginWindows() override;
  void ShowAllPluginWindows();
  void HideAllPluginWindows();
#else
  void ScheduleShowAllPluginWindows() override {}
  void ScheduleHideAllPluginWindows() override {}
#endif

  /**
   * Main thread response for a plugin visibility request made by the
   * compositor thread.
   */
  mozilla::ipc::IPCResult RecvRemotePluginsReady() override;

  /**
   * Used by the profiler to denote when a vsync occured
   */
  static void PostInsertVsyncProfilerMarker(mozilla::TimeStamp aVsyncTimestamp);

  widget::CompositorWidget* GetWidget() { return mWidget; }

  virtual void ForceComposeToTarget(
      gfx::DrawTarget* aTarget, const gfx::IntRect* aRect = nullptr) override;

  PAPZCTreeManagerParent* AllocPAPZCTreeManagerParent(
      const LayersId& aLayersId) override;
  bool DeallocPAPZCTreeManagerParent(PAPZCTreeManagerParent* aActor) override;

  // Helper method so that we don't have to expose mApzcTreeManager to
  // ContentCompositorBridgeParent.
  void AllocateAPZCTreeManagerParent(
      const Monitor2AutoLock& aProofOfLayerTreeStateLock,
#ifdef MOZ_BUILD_WEBRENDER
      const WRRootId& aWrRootId,
#else
      const LayersId& aLayersId,
#endif
      LayerTreeState& aLayerTreeStateToUpdate);

  PAPZParent* AllocPAPZParent(const LayersId& aLayersId) override;
  bool DeallocPAPZParent(PAPZParent* aActor) override;

  RefPtr<APZSampler> GetAPZSampler();
  RefPtr<APZUpdater> GetAPZUpdater();

  CompositorOptions GetOptions() const { return mOptions; }

  TimeDuration GetVsyncInterval() const override {
    // the variable is called "rate" but really it's an interval
    return mVsyncRate;
  }

#ifdef MOZ_BUILD_WEBRENDER
  PWebRenderBridgeParent* AllocPWebRenderBridgeParent(
      const wr::PipelineId& aPipelineId,
      const LayoutDeviceIntSize& aSize) override;
  bool DeallocPWebRenderBridgeParent(PWebRenderBridgeParent* aActor) override;
  RefPtr<WebRenderBridgeParent> GetWebRenderBridgeParent() const;
  static RefPtr<CompositorBridgeParent> GetCompositorBridgeParentFromWindowId(
      const wr::WindowId& aWindowId);
#endif
  Maybe<TimeStamp> GetTestingTimeStamp() const;

  static CompositorBridgeParent* GetCompositorBridgeParentFromLayersId(
      const LayersId& aLayersId);

  /**
   * This returns a reference to the IAPZCTreeManager "controller subinterface"
   * to which pan/zoom-related events can be sent. The controller subinterface
   * doesn't expose any sampler-thread APZCTreeManager methods.
   */
  static already_AddRefed<IAPZCTreeManager> GetAPZCTreeManager(
      LayersId aLayersId);

#ifdef MOZ_BUILD_WEBRENDER
  WebRenderBridgeParent* GetWrBridge() { return mWrBridge; }
#endif

 private:
  void Initialize();

  /**
   * Called during destruction in order to release resources as early as
   * possible.
   */
  void StopAndClearResources();

  /**
   * Release compositor-thread resources referred to by |aID|.
   *
   * Must run on the content main thread.
   */
  static void DeallocateLayerTreeId(LayersId aId);

 protected:
  // Protected destructor, to discourage deletion outside of Release():
  virtual ~CompositorBridgeParent();

  void DeferredDestroy();

  PLayerTransactionParent* AllocPLayerTransactionParent(
      const nsTArray<LayersBackend>& aBackendHints,
      const LayersId& aId) override;
  bool DeallocPLayerTransactionParent(
      PLayerTransactionParent* aLayers) override;

  void SetEGLSurfaceRect(int x, int y, int width, int height);

  void InitializeLayerManager(const nsTArray<LayersBackend>& aBackendHints);

 public:
  void PauseComposition();
  void ResumeComposition();
  void ResumeCompositionAndResize(int x, int y, int width, int height);
  void Invalidate();
  bool IsPaused() { return mPaused; }

 protected:
  void ForceComposition();
  void CancelCurrentCompositeTask();

  // CompositorVsyncSchedulerOwner
  bool IsPendingComposite() override;
  void FinishPendingComposite() override;
  void CompositeToTarget(VsyncId aId, gfx::DrawTarget* aTarget,
                         const gfx::IntRect* aRect = nullptr) override;

  bool InitializeAdvancedLayers(const nsTArray<LayersBackend>& aBackendHints,
                                TextureFactoryIdentifier* aOutIdentifier);
  RefPtr<Compositor> NewCompositor(
      const nsTArray<LayersBackend>& aBackendHints);

  /**
   * Add a compositor to the global compositor map.
   */
  static void AddCompositor(CompositorBridgeParent* compositor, uint64_t* id);
  /**
   * Remove a compositor from the global compositor map.
   */
  static CompositorBridgeParent* RemoveCompositor(uint64_t id);

  /**
   * Creates the global compositor map.
   */
  static void Setup();

  /**
   * Remaning cleanups after the compositore thread is gone.
   */
  static void FinishShutdown();

  /**
   * Return true if current state allows compositing, that is
   * finishing a layers transaction.
   */
  bool CanComposite();

  void DidComposite(const VsyncId& aId, TimeStamp& aCompositeStart,
                    TimeStamp& aCompositeEnd);

  void NotifyDidComposite(TransactionId aTransactionId, VsyncId aId,
                          TimeStamp& aCompositeStart, TimeStamp& aCompositeEnd);

  // The indirect layer tree lock must be held before calling this function.
  // Callback should take (LayerTreeState* aState, const LayersId& aLayersId)
  template <typename Lambda>
  inline void ForEachIndirectLayerTree(const Lambda& aCallback);

  RefPtr<HostLayerManager> mLayerManager;
  RefPtr<Compositor> mCompositor;
  RefPtr<AsyncCompositionManager> mCompositionManager;
#ifdef MOZ_BUILD_WEBRENDER
  RefPtr<AsyncImagePipelineManager> mAsyncImageManager;
  RefPtr<WebRenderBridgeParent> mWrBridge;
#endif
  widget::CompositorWidget* mWidget;
  Maybe<TimeStamp> mTestTime;
  CSSToLayoutDeviceScale mScale;
  TimeDuration mVsyncRate;

  TransactionId mPendingTransaction;
  TimeStamp mRefreshStartTime;
  TimeStamp mTxnStartTime;
  TimeStamp mFwdTime;

  bool mPaused;
  bool mHaveCompositionRecorder;

  bool mUseExternalSurfaceSize;
  gfx::IntSize mEGLSurfaceSize;

  CompositorOptions mOptions;

  mozilla::Monitor2 mPauseCompositionMonitor;
  mozilla::Monitor2 mResumeCompositionMonitor;

  uint64_t mCompositorBridgeID;
  LayersId mRootLayerTreeID;

  bool mOverrideComposeReadiness;
  RefPtr<CancelableRunnable> mForceCompositionTask;

  RefPtr<APZCTreeManager> mApzcTreeManager;
  RefPtr<APZSampler> mApzSampler;
  RefPtr<APZUpdater> mApzUpdater;

  RefPtr<CompositorVsyncScheduler> mCompositorScheduler;
  // This makes sure the compositorParent is not destroyed before receiving
  // confirmation that the channel is closed.
  // mSelfRef is cleared in DeferredDestroy which is scheduled by ActorDestroy.
  RefPtr<CompositorBridgeParent> mSelfRef;
  RefPtr<CompositorAnimationStorage> mAnimationStorage;

  TimeDuration mPaintTime;

#if defined(XP_WIN) || defined(MOZ_WIDGET_GTK)
  // cached plugin data used to reduce the number of updates we request.
  LayersId mLastPluginUpdateLayerTreeId;
  nsIntPoint mPluginsLayerOffset;
  nsIntRegion mPluginsLayerVisibleRegion;
  nsTArray<PluginWindowData> mCachedPluginData;
  // Time until which we will block composition to wait for plugin updates.
  TimeStamp mWaitForPluginsUntil;
  // Indicates that we have actually blocked a composition waiting for plugins.
  bool mHaveBlockedForPlugins = false;
  // indicates if plugin window visibility and metric updates are currently
  // being defered due to a scroll operation.
  bool mDeferPluginWindows;
  // indicates if the plugin windows were hidden, and need to be made
  // visible again even if their geometry has not changed.
  bool mPluginWindowsHidden;
#endif

  DISALLOW_EVIL_CONSTRUCTORS(CompositorBridgeParent);
};

int32_t RecordContentFrameTime(const VsyncId& aTxnId,
                               const TimeStamp& aVsyncStart,
                               const TimeStamp& aTxnStart,
                               const VsyncId& aCompositeId,
                               const TimeStamp& aCompositeEnd,
                               const TimeDuration& aFullPaintTime,
                               const TimeDuration& aVsyncRate,
                               bool aContainsSVGGroup, bool aRecordUploadStats
#ifdef MOZ_BUILD_WEBRENDER
                               ,
                               wr::RendererStats* aStats = nullptr
#endif
);

}  // namespace layers
}  // namespace mozilla

#endif  // mozilla_layers_CompositorBridgeParent_h
