/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_GFX_VR_VRMANAGERCHILD_H
#define MOZILLA_GFX_VR_VRMANAGERCHILD_H

#include "mozilla/Attributes.h"
#include "mozilla/dom/WindowBinding.h"  // For FrameRequestCallback
#include "mozilla/gfx/PVRManagerChild.h"
#include "mozilla/ipc/SharedMemory.h"  // for SharedMemory, etc
#include "ThreadSafeRefcountingWithMainThreadDestruction.h"
#include "mozilla/layers/ISurfaceAllocator.h"  // for ISurfaceAllocator
#include "mozilla/layers/LayersTypes.h"        // for LayersBackend
#include "mozilla/layers/TextureForwarder.h"

namespace mozilla {
namespace dom {
class Promise;
class GamepadManager;
class Navigator;
class VRDisplay;
class VREventObserver;
class VRMockDisplay;
}  // namespace dom
namespace layers {
class SyncObjectClient;
}
namespace gfx {
class VRLayerChild;
class VRDisplayClient;

class VRManagerChild : public PVRManagerChild {
  friend class PVRManagerChild;

 public:
  NS_INLINE_DECL_THREADSAFE_REFCOUNTING(VRManagerChild);

  static VRManagerChild* Get();

  // Indicate that an observer wants to receive VR events.
  void AddListener(dom::VREventObserver* aObserver);
  // Indicate that an observer should no longer receive VR events.
  void RemoveListener(dom::VREventObserver* aObserver);
  void StartActivity();
  void StopActivity();

  bool GetVRDisplays(nsTArray<RefPtr<VRDisplayClient>>& aDisplays);
  bool RefreshVRDisplaysWithCallback(uint64_t aWindowId);
  void AddPromise(const uint32_t& aID, dom::Promise* aPromise);

  void CreateVRServiceTestDisplay(const nsCString& aID, dom::Promise* aPromise);
  void CreateVRServiceTestController(const nsCString& aID,
                                     dom::Promise* aPromise);

  static void InitSameProcess();
  static void InitWithGPUProcess(Endpoint<PVRManagerChild>&& aEndpoint);
  static bool InitForContent(Endpoint<PVRManagerChild>&& aEndpoint);
  static bool ReinitForContent(Endpoint<PVRManagerChild>&& aEndpoint);
  static void ShutDown();

  static bool IsCreated();

  PVRLayerChild* CreateVRLayer(uint32_t aDisplayID,
                               nsISerialEventTarget* aTarget, uint32_t aGroup);

  static void IdentifyTextureHost(
      const layers::TextureFactoryIdentifier& aIdentifier);
  layers::LayersBackend GetBackendType() const;
  layers::SyncObjectClient* GetSyncObject() { return mSyncObject; }

  nsresult ScheduleFrameRequestCallback(
      mozilla::dom::FrameRequestCallback& aCallback, int32_t* aHandle);
  void CancelFrameRequestCallback(int32_t aHandle);
  MOZ_CAN_RUN_SCRIPT
  void RunFrameRequestCallbacks();
  void NotifyPresentationGenerationChanged(uint32_t aDisplayID);

  MOZ_CAN_RUN_SCRIPT
  void UpdateDisplayInfo(nsTArray<VRDisplayInfo>& aDisplayUpdates);
  void FireDOMVRDisplayMountedEvent(uint32_t aDisplayID);
  void FireDOMVRDisplayUnmountedEvent(uint32_t aDisplayID);
  void FireDOMVRDisplayConnectEvent(uint32_t aDisplayID);
  void FireDOMVRDisplayDisconnectEvent(uint32_t aDisplayID);
  void FireDOMVRDisplayPresentChangeEvent(uint32_t aDisplayID);
  void FireDOMVRDisplayConnectEventsForLoad(dom::VREventObserver* aObserver);

  virtual void HandleFatalError(const char* aMsg) const override;

 protected:
  explicit VRManagerChild();
  ~VRManagerChild();
  void Destroy();
  static void DeferredDestroy(RefPtr<VRManagerChild> aVRManagerChild);

  PVRLayerChild* AllocPVRLayerChild(const uint32_t& aDisplayID,
                                    const uint32_t& aGroup);
  bool DeallocPVRLayerChild(PVRLayerChild* actor);

  // MOZ_CAN_RUN_SCRIPT_BOUNDARY until we can mark ipdl-generated things as
  // MOZ_CAN_RUN_SCRIPT.
  MOZ_CAN_RUN_SCRIPT_BOUNDARY
  mozilla::ipc::IPCResult RecvUpdateDisplayInfo(
      nsTArray<VRDisplayInfo>&& aDisplayUpdates);

  mozilla::ipc::IPCResult RecvDispatchSubmitFrameResult(
      const uint32_t& aDisplayID, const VRSubmitFrameResultInfo& aResult);
  mozilla::ipc::IPCResult RecvGamepadUpdate(
      const GamepadChangeEvent& aGamepadEvent);
  mozilla::ipc::IPCResult RecvReplyGamepadVibrateHaptic(
      const uint32_t& aPromiseID);

  mozilla::ipc::IPCResult RecvReplyCreateVRServiceTestDisplay(
      const nsCString& aID, const uint32_t& aPromiseID,
      const uint32_t& aDeviceID);
  mozilla::ipc::IPCResult RecvReplyCreateVRServiceTestController(
      const nsCString& aID, const uint32_t& aPromiseID,
      const uint32_t& aDeviceID);
  bool IsSameProcess() const { return OtherPid() == base::GetCurrentProcId(); }

 private:
  void FireDOMVRDisplayMountedEventInternal(uint32_t aDisplayID);
  void FireDOMVRDisplayUnmountedEventInternal(uint32_t aDisplayID);
  void FireDOMVRDisplayConnectEventInternal(uint32_t aDisplayID);
  void FireDOMVRDisplayDisconnectEventInternal(uint32_t aDisplayID);
  void FireDOMVRDisplayPresentChangeEventInternal(uint32_t aDisplayID);
  void FireDOMVRDisplayConnectEventsForLoadInternal(
      uint32_t aDisplayID, dom::VREventObserver* aObserver);
  void NotifyPresentationGenerationChangedInternal(uint32_t aDisplayID);

  nsTArray<RefPtr<VRDisplayClient>> mDisplays;
  bool mDisplaysInitialized;
  nsTArray<uint64_t> mNavigatorCallbacks;

  MessageLoop* mMessageLoop;

  struct FrameRequest;

  nsTArray<FrameRequest> mFrameRequestCallbacks;
  /**
   * The current frame request callback handle
   */
  int32_t mFrameRequestCallbackCounter;
  mozilla::TimeStamp mStartTimeStamp;

  nsTArray<RefPtr<dom::VREventObserver>> mListeners;

  layers::LayersBackend mBackend;
  RefPtr<layers::SyncObjectClient> mSyncObject;
  nsRefPtrHashtable<nsUint32HashKey, dom::Promise> mGamepadPromiseList;
  uint32_t mPromiseID;
  nsRefPtrHashtable<nsUint32HashKey, dom::Promise> mPromiseList;
  RefPtr<dom::VRMockDisplay> mVRMockDisplay;
  VRControllerState mLastControllerState[kVRControllerMaxCount];

  DISALLOW_COPY_AND_ASSIGN(VRManagerChild);
};

}  // namespace gfx
}  // namespace mozilla

#endif  // MOZILLA_GFX_VR_VRMANAGERCHILD_H
