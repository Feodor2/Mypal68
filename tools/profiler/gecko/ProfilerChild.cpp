/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ProfilerChild.h"

#include "GeckoProfiler.h"
#include "platform.h"

#include "nsThreadUtils.h"

namespace mozilla {

ProfilerChild::ProfilerChild()
    : mThread(NS_GetCurrentThread()), mDestroyed(false) {
  MOZ_COUNT_CTOR(ProfilerChild);
}

ProfilerChild::~ProfilerChild() { MOZ_COUNT_DTOR(ProfilerChild); }

mozilla::ipc::IPCResult ProfilerChild::RecvStart(
    const ProfilerInitParams& params) {
  nsTArray<const char*> filterArray;
  for (size_t i = 0; i < params.filters().Length(); ++i) {
    filterArray.AppendElement(params.filters()[i].get());
  }

  profiler_start(PowerOfTwo32(params.entries()), params.interval(),
                 params.features(), filterArray.Elements(),
                 filterArray.Length(), params.duration());

  return IPC_OK();
}

mozilla::ipc::IPCResult ProfilerChild::RecvEnsureStarted(
    const ProfilerInitParams& params) {
  nsTArray<const char*> filterArray;
  for (size_t i = 0; i < params.filters().Length(); ++i) {
    filterArray.AppendElement(params.filters()[i].get());
  }

  profiler_ensure_started(PowerOfTwo32(params.entries()), params.interval(),
                          params.features(), filterArray.Elements(),
                          filterArray.Length(), params.duration());

  return IPC_OK();
}

mozilla::ipc::IPCResult ProfilerChild::RecvStop() {
  profiler_stop();
  return IPC_OK();
}

mozilla::ipc::IPCResult ProfilerChild::RecvPause() {
  profiler_pause();
  return IPC_OK();
}

mozilla::ipc::IPCResult ProfilerChild::RecvResume() {
  profiler_resume();
  return IPC_OK();
}

mozilla::ipc::IPCResult ProfilerChild::RecvClearAllPages() {
  profiler_clear_all_pages();
  return IPC_OK();
}

static nsCString CollectProfileOrEmptyString(bool aIsShuttingDown) {
  nsCString profileCString;
  UniquePtr<char[]> profile =
      profiler_get_profile(/* aSinceTime */ 0, aIsShuttingDown);
  if (profile) {
    size_t len = strlen(profile.get());
    profileCString.Adopt(profile.release(), len);
  } else {
    profileCString = EmptyCString();
  }
  return profileCString;
}

mozilla::ipc::IPCResult ProfilerChild::RecvGatherProfile(
    GatherProfileResolver&& aResolve) {
  mozilla::ipc::Shmem shmem;
  profiler_get_profile_json_into_lazily_allocated_buffer(
      [&](size_t allocationSize) -> char* {
        if (AllocShmem(allocationSize,
                       mozilla::ipc::Shmem::SharedMemory::TYPE_BASIC, &shmem)) {
          return shmem.get<char>();
        }
        return nullptr;
      },
      /* aSinceTime */ 0,
      /* aIsShuttingDown */ false);
  aResolve(std::move(shmem));
  return IPC_OK();
}

void ProfilerChild::ActorDestroy(ActorDestroyReason aActorDestroyReason) {
  mDestroyed = true;
}

void ProfilerChild::Destroy() {
  if (!mDestroyed) {
    Close();
  }
}

nsCString ProfilerChild::GrabShutdownProfile() {
  return CollectProfileOrEmptyString(/* aIsShuttingDown */ true);
}

}  // namespace mozilla
