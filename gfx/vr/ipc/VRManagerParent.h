/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_GFX_VR_VRMANAGERPARENT_H
#define MOZILLA_GFX_VR_VRMANAGERPARENT_H

#include "mozilla/layers/CompositorThread.h"  // for CompositorThreadHolder
#include "mozilla/layers/CompositableTransactionParent.h"  // need?
#include "mozilla/gfx/PVRManagerParent.h"  // for PVRManagerParent
#include "mozilla/gfx/PVRLayerParent.h"    // for PVRLayerParent
#include "mozilla/ipc/ProtocolUtils.h"     // for IToplevelProtocol
#include "mozilla/TimeStamp.h"             // for TimeStamp
#include "gfxVR.h"                         // for VRFieldOfView

namespace mozilla {
using namespace layers;
namespace gfx {

class VRManager;

namespace impl {
class VRDisplayPuppet;
class VRControllerPuppet;
}  // namespace impl

class VRManagerParent final : public PVRManagerParent {
  NS_INLINE_DECL_THREADSAFE_REFCOUNTING(VRManagerParent);

  friend class PVRManagerParent;

 public:
  explicit VRManagerParent(ProcessId aChildProcessId, bool aIsContentChild);

  static VRManagerParent* CreateSameProcess();
  static bool CreateForGPUProcess(Endpoint<PVRManagerParent>&& aEndpoint);
  static bool CreateForContent(Endpoint<PVRManagerParent>&& aEndpoint);

  bool IsSameProcess() const;
  bool HaveEventListener();
  bool HaveControllerListener();
  bool GetVRActiveStatus();
  bool SendGamepadUpdate(const GamepadChangeEvent& aGamepadEvent);
  bool SendReplyGamepadVibrateHaptic(const uint32_t& aPromiseID);

 protected:
  ~VRManagerParent();

  PVRLayerParent* AllocPVRLayerParent(const uint32_t& aDisplayID,
                                      const uint32_t& aGroup);
  bool DeallocPVRLayerParent(PVRLayerParent* actor);

  virtual void ActorDestroy(ActorDestroyReason why) override;
  void OnChannelConnected(int32_t pid) override;

  mozilla::ipc::IPCResult RecvRefreshDisplays();
  mozilla::ipc::IPCResult RecvResetSensor(const uint32_t& aDisplayID);
  mozilla::ipc::IPCResult RecvSetGroupMask(const uint32_t& aDisplayID,
                                           const uint32_t& aGroupMask);
  mozilla::ipc::IPCResult RecvSetHaveEventListener(
      const bool& aHaveEventListener);
  mozilla::ipc::IPCResult RecvControllerListenerAdded();
  mozilla::ipc::IPCResult RecvControllerListenerRemoved();
  mozilla::ipc::IPCResult RecvVibrateHaptic(const uint32_t& aControllerIdx,
                                            const uint32_t& aHapticIndex,
                                            const double& aIntensity,
                                            const double& aDuration,
                                            const uint32_t& aPromiseID);
  mozilla::ipc::IPCResult RecvStopVibrateHaptic(const uint32_t& aControllerIdx);
  mozilla::ipc::IPCResult RecvCreateVRTestSystem();
  mozilla::ipc::IPCResult RecvCreateVRServiceTestDisplay(
      const nsCString& aID, const uint32_t& aPromiseID);
  mozilla::ipc::IPCResult RecvCreateVRServiceTestController(
      const nsCString& aID, const uint32_t& aPromiseID);
  mozilla::ipc::IPCResult RecvSetDisplayInfoToMockDisplay(
      const uint32_t& aDeviceID, const VRDisplayInfo& aDisplayInfo);
  mozilla::ipc::IPCResult RecvSetSensorStateToMockDisplay(
      const uint32_t& aDeviceID, const VRHMDSensorState& aSensorState);
  mozilla::ipc::IPCResult RecvNewButtonEventToMockController(
      const uint32_t& aDeviceID, const long& aButton, const bool& aPressed);
  mozilla::ipc::IPCResult RecvNewAxisMoveEventToMockController(
      const uint32_t& aDeviceID, const long& aAxis, const double& aValue);
  mozilla::ipc::IPCResult RecvNewPoseMoveToMockController(
      const uint32_t& aDeviceID, const GamepadPoseState& pose);
  mozilla::ipc::IPCResult RecvStartVRNavigation(const uint32_t& aDeviceID);
  mozilla::ipc::IPCResult RecvStopVRNavigation(const uint32_t& aDeviceID,
                                               const TimeDuration& aTimeout);
  mozilla::ipc::IPCResult RecvStartActivity();
  mozilla::ipc::IPCResult RecvStopActivity();

 private:
  void RegisterWithManager();
  void UnregisterFromManager();

  void Bind(Endpoint<PVRManagerParent>&& aEndpoint);

  static void RegisterVRManagerInCompositorThread(VRManagerParent* aVRManager);

  void DeferredDestroy();
  already_AddRefed<impl::VRControllerPuppet> GetControllerPuppet(
      uint32_t aDeviceID);

  // This keeps us alive until ActorDestroy(), at which point we do a
  // deferred destruction of ourselves.
  RefPtr<VRManagerParent> mSelfRef;
  // Keep the compositor thread alive, until we have destroyed ourselves.
  RefPtr<CompositorThreadHolder> mCompositorThreadHolder;

  // Keep the VRManager alive, until we have destroyed ourselves.
  RefPtr<VRManager> mVRManagerHolder;
  nsRefPtrHashtable<nsUint32HashKey, impl::VRControllerPuppet>
      mVRControllerTests;
  uint32_t mControllerTestID;
  bool mHaveEventListener;
  bool mHaveControllerListener;
  bool mIsContentChild;

  // When VR tabs are switched the background, we won't need to
  // initialize its session in VRService thread.
  bool mVRActiveStatus;
};

class VRManagerPromise final {
  friend class VRManager;

 public:
  explicit VRManagerPromise(RefPtr<VRManagerParent> aParent,
                            uint32_t aPromiseID)
      : mParent(aParent), mPromiseID(aPromiseID) {}
  ~VRManagerPromise() { mParent = nullptr; }
  bool operator==(const VRManagerPromise& aOther) const {
    return mParent == aOther.mParent && mPromiseID == aOther.mPromiseID;
  }

 private:
  RefPtr<VRManagerParent> mParent;
  uint32_t mPromiseID;
};

}  // namespace gfx
}  // namespace mozilla

#endif  // MOZILLA_GFX_VR_VRMANAGERPARENT_H
