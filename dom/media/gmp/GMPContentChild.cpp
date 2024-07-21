/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "GMPContentChild.h"
#include "GMPChild.h"
#include "GMPVideoDecoderChild.h"
#include "GMPVideoEncoderChild.h"
#include "base/task.h"
#include "GMPUtils.h"

namespace mozilla {
namespace gmp {

GMPContentChild::GMPContentChild(GMPChild* aChild) : mGMPChild(aChild) {
  MOZ_COUNT_CTOR(GMPContentChild);
}

GMPContentChild::~GMPContentChild() { MOZ_COUNT_DTOR(GMPContentChild); }

MessageLoop* GMPContentChild::GMPMessageLoop() {
  return mGMPChild->GMPMessageLoop();
}

void GMPContentChild::CheckThread() {
  MOZ_ASSERT(mGMPChild->mGMPMessageLoop == MessageLoop::current());
}

void GMPContentChild::ActorDestroy(ActorDestroyReason aWhy) {
  mGMPChild->GMPContentChildActorDestroy(this);
}

void GMPContentChild::ProcessingError(Result aCode, const char* aReason) {
  mGMPChild->ProcessingError(aCode, aReason);
}

already_AddRefed<PGMPVideoDecoderChild>
GMPContentChild::AllocPGMPVideoDecoderChild(const uint32_t& aDecryptorId) {
  return MakeAndAddRef<GMPVideoDecoderChild>(this);
}

already_AddRefed<PGMPVideoEncoderChild>
GMPContentChild::AllocPGMPVideoEncoderChild() {
  return MakeAndAddRef<GMPVideoEncoderChild>(this);
}

mozilla::ipc::IPCResult GMPContentChild::RecvPGMPVideoDecoderConstructor(
    PGMPVideoDecoderChild* aActor, const uint32_t& aDecryptorId) {
  auto vdc = static_cast<GMPVideoDecoderChild*>(aActor);

  void* vd = nullptr;
  GMPErr err =
      mGMPChild->GetAPI(GMP_API_VIDEO_DECODER, &vdc->Host(), &vd, aDecryptorId);
  if (err != GMPNoErr || !vd) {
    NS_WARNING("GMPGetAPI call failed trying to construct decoder.");
    return IPC_FAIL_NO_REASON(this);
  }

  vdc->Init(static_cast<GMPVideoDecoder*>(vd));

  return IPC_OK();
}

mozilla::ipc::IPCResult GMPContentChild::RecvPGMPVideoEncoderConstructor(
    PGMPVideoEncoderChild* aActor) {
  auto vec = static_cast<GMPVideoEncoderChild*>(aActor);

  void* ve = nullptr;
  GMPErr err = mGMPChild->GetAPI(GMP_API_VIDEO_ENCODER, &vec->Host(), &ve);
  if (err != GMPNoErr || !ve) {
    NS_WARNING("GMPGetAPI call failed trying to construct encoder.");
    return IPC_FAIL_NO_REASON(this);
  }

  vec->Init(static_cast<GMPVideoEncoder*>(ve));

  return IPC_OK();
}

void GMPContentChild::CloseActive() {
  // Invalidate and remove any remaining API objects.
  const ManagedContainer<PGMPVideoDecoderChild>& videoDecoders =
      ManagedPGMPVideoDecoderChild();
  for (auto iter = videoDecoders.ConstIter(); !iter.Done(); iter.Next()) {
    iter.Get()->GetKey()->SendShutdown();
  }

  const ManagedContainer<PGMPVideoEncoderChild>& videoEncoders =
      ManagedPGMPVideoEncoderChild();
  for (auto iter = videoEncoders.ConstIter(); !iter.Done(); iter.Next()) {
    iter.Get()->GetKey()->SendShutdown();
  }
}

bool GMPContentChild::IsUsed() {
  return !ManagedPGMPVideoDecoderChild().IsEmpty() ||
         !ManagedPGMPVideoEncoderChild().IsEmpty();
}

}  // namespace gmp
}  // namespace mozilla
