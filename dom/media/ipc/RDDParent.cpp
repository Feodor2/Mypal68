/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "RDDParent.h"

#if defined(XP_WIN)
#  include <process.h>
#  include <dwrite.h>
#endif

#include "chrome/common/ipc_channel.h"
#include "mozilla/Assertions.h"
#include "mozilla/HangDetails.h"
#include "mozilla/Preferences.h"
#include "mozilla/RemoteDecoderManagerChild.h"
#include "mozilla/RemoteDecoderManagerParent.h"
#include "mozilla/TimeStamp.h"
#include "mozilla/dom/MemoryReportRequest.h"
#include "mozilla/ipc/CrashReporterClient.h"
#include "mozilla/ipc/ProcessChild.h"
#include "mozilla/gfx/gfxVars.h"

#if defined(XP_LINUX) && defined(MOZ_SANDBOX)
#  include "mozilla/Sandbox.h"
#endif

#ifdef MOZ_GECKO_PROFILER
#  include "ChildProfilerController.h"
#endif

#if defined(XP_MACOSX) && defined(MOZ_SANDBOX)
#  include "mozilla/Sandbox.h"
#  include "nsMacUtilsImpl.h"
#  include <Carbon/Carbon.h>  // for CGSSetDenyWindowServerConnections
#  include "RDDProcessHost.h"
#endif

#include "nsDebugImpl.h"
#include "nsThreadManager.h"
#include "ProcessUtils.h"

namespace mozilla {

using namespace ipc;
using namespace gfx;

static RDDParent* sRDDParent;

RDDParent::RDDParent() : mLaunchTime(TimeStamp::Now()) { sRDDParent = this; }

RDDParent::~RDDParent() { sRDDParent = nullptr; }

/* static */
RDDParent* RDDParent::GetSingleton() { return sRDDParent; }

bool RDDParent::Init(base::ProcessId aParentPid, const char* aParentBuildID,
                     MessageLoop* aIOLoop, UniquePtr<IPC::Channel> aChannel) {
  // Initialize the thread manager before starting IPC. Otherwise, messages
  // may be posted to the main thread and we won't be able to process them.
  if (NS_WARN_IF(NS_FAILED(nsThreadManager::get().Init()))) {
    return false;
  }

  // Now it's safe to start IPC.
  if (NS_WARN_IF(!Open(std::move(aChannel), aParentPid, aIOLoop))) {
    return false;
  }

  nsDebugImpl::SetMultiprocessMode("RDD");

  // This must be checked before any IPDL message, which may hit sentinel
  // errors due to parent and content processes having different
  // versions.
  MessageChannel* channel = GetIPCChannel();
  if (channel && !channel->SendBuildIDsMatchMessage(aParentBuildID)) {
    // We need to quit this process if the buildID doesn't match the parent's.
    // This can occur when an update occurred in the background.
    ProcessChild::QuickExit();
  }

  // Init crash reporter support.
  CrashReporterClient::InitSingleton(this);

  if (NS_FAILED(NS_InitMinimalXPCOM())) {
    return false;
  }

  gfxVars::Initialize();

  mozilla::ipc::SetThisProcessName("RDD Process");
  return true;
}

#if defined(XP_MACOSX) && defined(MOZ_SANDBOX)
extern "C" {
CGError CGSSetDenyWindowServerConnections(bool);
void CGSShutdownServerConnections();
};

static void StartRDDMacSandbox() {
  // Actual security benefits are only acheived when we additionally deny
  // future connections.
  CGError result = CGSSetDenyWindowServerConnections(true);
  MOZ_DIAGNOSTIC_ASSERT(result == kCGErrorSuccess);
#  if !MOZ_DIAGNOSTIC_ASSERT_ENABLED
  Unused << result;
#  endif

  MacSandboxInfo info;
  RDDProcessHost::StaticFillMacSandboxInfo(info);

  std::string err;
  bool rv = mozilla::StartMacSandbox(info, err);
  if (!rv) {
    NS_WARNING(err.c_str());
    MOZ_CRASH("mozilla::StartMacSandbox failed");
  }
}
#endif

mozilla::ipc::IPCResult RDDParent::RecvInit(
    nsTArray<GfxVarUpdate>&& vars, const Maybe<FileDescriptor>& aBrokerFd,
    bool aStartMacSandbox) {
  for (const auto& var : vars) {
    gfxVars::ApplyUpdate(var);
  }

#if defined(MOZ_SANDBOX)
#  if defined(XP_MACOSX)
  // Close all current connections to the WindowServer. This ensures that the
  // Activity Monitor will not label the content process as "Not responding"
  // because it's not running a native event loop. See bug 1384336.
  CGSShutdownServerConnections();

  if (aStartMacSandbox) {
    StartRDDMacSandbox();
  } else {
#    ifdef DEBUG
    AssertMacSandboxEnabled();
#    endif
  }
#  elif defined(XP_LINUX)
  int fd = -1;
  if (aBrokerFd.isSome()) {
    fd = aBrokerFd.value().ClonePlatformHandle().release();
  }
  SetRemoteDataDecoderSandbox(fd);
#  endif  // XP_MACOSX/XP_LINUX
#endif    // MOZ_SANDBOX

  return IPC_OK();
}

IPCResult RDDParent::RecvUpdateVar(const GfxVarUpdate& aUpdate) {
  gfxVars::ApplyUpdate(aUpdate);
  return IPC_OK();
}

mozilla::ipc::IPCResult RDDParent::RecvInitProfiler(
    Endpoint<PProfilerChild>&& aEndpoint) {
#ifdef MOZ_GECKO_PROFILER
  mProfilerController = ChildProfilerController::Create(std::move(aEndpoint));
#endif
  return IPC_OK();
}

mozilla::ipc::IPCResult RDDParent::RecvNewContentRemoteDecoderManager(
    Endpoint<PRemoteDecoderManagerParent>&& aEndpoint) {
  if (!RemoteDecoderManagerParent::CreateForContent(std::move(aEndpoint))) {
    return IPC_FAIL_NO_REASON(this);
  }
  return IPC_OK();
}

mozilla::ipc::IPCResult RDDParent::RecvInitVideoBridge(
    Endpoint<PVideoBridgeChild>&& aEndpoint) {
  if (!RemoteDecoderManagerParent::CreateVideoBridgeToOtherProcess(
          std::move(aEndpoint))) {
    return IPC_FAIL_NO_REASON(this);
  }
  return IPC_OK();
}

mozilla::ipc::IPCResult RDDParent::RecvRequestMemoryReport(
    const uint32_t& aGeneration, const bool& aAnonymize,
    const bool& aMinimizeMemoryUsage, const Maybe<FileDescriptor>& aDMDFile) {
  nsPrintfCString processName("RDD (pid %u)", (unsigned)getpid());

  mozilla::dom::MemoryReportRequestClient::Start(
      aGeneration, aAnonymize, aMinimizeMemoryUsage, aDMDFile, processName,
      [&](const MemoryReport& aReport) {
        Unused << GetSingleton()->SendAddMemoryReport(aReport);
      },
      [&](const uint32_t& aGeneration) {
        return GetSingleton()->SendFinishMemoryReport(aGeneration);
      });
  return IPC_OK();
}

mozilla::ipc::IPCResult RDDParent::RecvPreferenceUpdate(const Pref& aPref) {
  Preferences::SetPreference(aPref);
  return IPC_OK();
}

void RDDParent::ActorDestroy(ActorDestroyReason aWhy) {
  if (AbnormalShutdown == aWhy) {
    NS_WARNING("Shutting down RDD process early due to a crash!");
    ProcessChild::QuickExit();
  }

#ifndef NS_FREE_PERMANENT_DATA
  // No point in going through XPCOM shutdown because we don't keep persistent
  // state.
  ProcessChild::QuickExit();
#endif

#ifdef MOZ_GECKO_PROFILER
  if (mProfilerController) {
    mProfilerController->Shutdown();
    mProfilerController = nullptr;
  }
#endif

  RemoteDecoderManagerParent::ShutdownVideoBridge();

  gfxVars::Shutdown();
  CrashReporterClient::DestroySingleton();
  XRE_ShutdownChildProcess();
}

}  // namespace mozilla
