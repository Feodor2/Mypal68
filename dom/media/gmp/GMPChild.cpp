/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "GMPChild.h"

#include "base/command_line.h"
#include "base/task.h"
#ifdef XP_LINUX
#  include "dlfcn.h"
#endif
#include "gmp-video-decode.h"
#include "gmp-video-encode.h"
#include "GMPContentChild.h"
#include "GMPLoader.h"
#include "GMPLog.h"
#include "GMPPlatform.h"
#include "GMPProcessChild.h"
#include "GMPUtils.h"
#include "GMPVideoDecoderChild.h"
#include "GMPVideoEncoderChild.h"
#include "GMPVideoHost.h"
#include "mozilla/Algorithm.h"
#include "mozilla/ipc/CrashReporterClient.h"
#include "mozilla/ipc/ProcessChild.h"
#if defined(MOZ_SANDBOX)
#  if defined(XP_MACOSX)
#    include "mozilla/Sandbox.h"
#  endif
#endif
#include "mozilla/TextUtils.h"
#include "nsDebugImpl.h"
#include "nsExceptionHandler.h"
#include "nsIFile.h"
#include "nsXULAppAPI.h"
#include "prio.h"
#ifdef XP_WIN
#  include <stdlib.h>  // for _exit()
#  include "WinUtils.h"
#else
#  include <unistd.h>  // for _exit()
#endif

using namespace mozilla::ipc;

namespace mozilla {

#define GMP_CHILD_LOG_DEBUG(x, ...)                                   \
  GMP_LOG_DEBUG("GMPChild[pid=%d] " x, (int)base::GetCurrentProcId(), \
                ##__VA_ARGS__)

namespace gmp {

GMPChild::GMPChild()
    : mGMPMessageLoop(MessageLoop::current()), mGMPLoader(nullptr) {
  GMP_CHILD_LOG_DEBUG("GMPChild ctor");
  nsDebugImpl::SetMultiprocessMode("GMP");
}

GMPChild::~GMPChild() {
  GMP_CHILD_LOG_DEBUG("GMPChild dtor");
#ifdef XP_LINUX
  for (auto& libHandle : mLibHandles) {
    dlclose(libHandle);
  }
#endif
}

static bool GetFileBase(const nsAString& aPluginPath,
                        nsCOMPtr<nsIFile>& aLibDirectory,
                        nsCOMPtr<nsIFile>& aFileBase, nsAutoString& aBaseName) {
  nsresult rv = NS_NewLocalFile(aPluginPath, true, getter_AddRefs(aFileBase));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return false;
  }

  if (NS_WARN_IF(NS_FAILED(aFileBase->Clone(getter_AddRefs(aLibDirectory))))) {
    return false;
  }

  nsCOMPtr<nsIFile> parent;
  rv = aFileBase->GetParent(getter_AddRefs(parent));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return false;
  }

  nsAutoString parentLeafName;
  rv = parent->GetLeafName(parentLeafName);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return false;
  }

  aBaseName = Substring(parentLeafName, 4, parentLeafName.Length() - 1);
  return true;
}

static bool GetPluginFile(const nsAString& aPluginPath,
                          nsCOMPtr<nsIFile>& aLibDirectory,
                          nsCOMPtr<nsIFile>& aLibFile) {
  nsAutoString baseName;
  GetFileBase(aPluginPath, aLibDirectory, aLibFile, baseName);

#if defined(XP_MACOSX)
  nsAutoString binaryName =
      NS_LITERAL_STRING("lib") + baseName + NS_LITERAL_STRING(".dylib");
#elif defined(OS_POSIX)
  nsAutoString binaryName =
      NS_LITERAL_STRING("lib") + baseName + NS_LITERAL_STRING(".so");
#elif defined(XP_WIN)
  nsAutoString binaryName = baseName + NS_LITERAL_STRING(".dll");
#else
#  error not defined
#endif
  aLibFile->AppendRelativePath(binaryName);
  return true;
}

static bool GetPluginFile(const nsAString& aPluginPath,
                          nsCOMPtr<nsIFile>& aLibFile) {
  nsCOMPtr<nsIFile> unusedlibDir;
  return GetPluginFile(aPluginPath, unusedlibDir, aLibFile);
}

#if defined(XP_MACOSX)
static nsCString GetNativeTarget(nsIFile* aFile) {
  bool isLink;
  nsCString path;
  aFile->IsSymlink(&isLink);
  if (isLink) {
    aFile->GetNativeTarget(path);
  } else {
    aFile->GetNativePath(path);
  }
  return path;
}

#  if defined(MOZ_SANDBOX)
static bool GetPluginPaths(const nsAString& aPluginPath,
                           nsCString& aPluginDirectoryPath,
                           nsCString& aPluginFilePath) {
  nsCOMPtr<nsIFile> libDirectory, libFile;
  if (!GetPluginFile(aPluginPath, libDirectory, libFile)) {
    return false;
  }

  // Mac sandbox rules expect paths to actual files and directories -- not
  // soft links.
  libDirectory->Normalize();
  aPluginDirectoryPath = GetNativeTarget(libDirectory);

  libFile->Normalize();
  aPluginFilePath = GetNativeTarget(libFile);

  return true;
}

static bool GetAppPaths(nsCString& aAppPath, nsCString& aAppBinaryPath) {
  nsAutoCString appPath;
  nsAutoCString appBinaryPath(
      (CommandLine::ForCurrentProcess()->argv()[0]).c_str());

  nsAutoCString::const_iterator start, end;
  appBinaryPath.BeginReading(start);
  appBinaryPath.EndReading(end);
  if (RFindInReadable(NS_LITERAL_CSTRING(".app/Contents/MacOS/"), start, end)) {
    end = start;
    ++end;
    ++end;
    ++end;
    ++end;
    appBinaryPath.BeginReading(start);
    appPath.Assign(Substring(start, end));
  } else {
    return false;
  }

  nsCOMPtr<nsIFile> app, appBinary;
  nsresult rv = NS_NewLocalFile(NS_ConvertUTF8toUTF16(appPath), true,
                                getter_AddRefs(app));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return false;
  }
  rv = NS_NewLocalFile(NS_ConvertUTF8toUTF16(appBinaryPath), true,
                       getter_AddRefs(appBinary));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return false;
  }

  // Mac sandbox rules expect paths to actual files and directories -- not
  // soft links.
  aAppPath = GetNativeTarget(app);
  appBinaryPath = GetNativeTarget(appBinary);

  return true;
}

bool GMPChild::SetMacSandboxInfo(bool aAllowWindowServer) {
  if (!mGMPLoader) {
    return false;
  }
  nsAutoCString pluginDirectoryPath, pluginFilePath;
  if (!GetPluginPaths(mPluginPath, pluginDirectoryPath, pluginFilePath)) {
    return false;
  }
  nsAutoCString appPath, appBinaryPath;
  if (!GetAppPaths(appPath, appBinaryPath)) {
    return false;
  }

  MacSandboxInfo info;
  info.type = MacSandboxType_GMP;
  info.shouldLog = Preferences::GetBool("security.sandbox.logging.enabled") ||
                   PR_GetEnv("MOZ_SANDBOX_LOGGING");
  info.hasWindowServer = aAllowWindowServer;
  info.pluginPath.assign(pluginDirectoryPath.get());
  info.pluginBinaryPath.assign(pluginFilePath.get());
  info.appPath.assign(appPath.get());
  info.appBinaryPath.assign(appBinaryPath.get());

  mGMPLoader->SetSandboxInfo(&info);
  return true;
}
#  endif  // MOZ_SANDBOX
#endif    // XP_MACOSX

bool GMPChild::Init(const nsAString& aPluginPath, base::ProcessId aParentPid,
                    MessageLoop* aIOLoop, UniquePtr<IPC::Channel> aChannel) {
  GMP_CHILD_LOG_DEBUG("%s pluginPath=%s", __FUNCTION__,
                      NS_ConvertUTF16toUTF8(aPluginPath).get());

  if (NS_WARN_IF(!Open(std::move(aChannel), aParentPid, aIOLoop))) {
    return false;
  }

  CrashReporterClient::InitSingleton(this);

  mPluginPath = aPluginPath;

  return true;
}

mozilla::ipc::IPCResult GMPChild::RecvProvideStorageId(
    const nsCString& aStorageId) {
  GMP_CHILD_LOG_DEBUG("%s", __FUNCTION__);
  mStorageId = aStorageId;
  return IPC_OK();
}

GMPErr GMPChild::GetAPI(const char* aAPIName, void* aHostAPI, void** aPluginAPI,
                        uint32_t aDecryptorId) {
  if (!mGMPLoader) {
    return GMPGenericErr;
  }
  return mGMPLoader->GetAPI(aAPIName, aHostAPI, aPluginAPI, aDecryptorId);
}

bool GMPChild::GetUTF8LibPath(nsACString& aOutLibPath) {
#if defined(XP_MACOSX) && defined(MOZ_SANDBOX)
  nsAutoCString pluginDirectoryPath, pluginFilePath;
  if (!GetPluginPaths(mPluginPath, pluginDirectoryPath, pluginFilePath)) {
    MOZ_CRASH("Error scanning plugin path");
  }
  aOutLibPath.Assign(pluginFilePath);
  return true;
#else
  nsCOMPtr<nsIFile> libFile;
  if (!GetPluginFile(mPluginPath, libFile)) {
    return false;
  }

  if (!FileExists(libFile)) {
    NS_WARNING("Can't find GMP library file!");
    return false;
  }

  nsAutoString path;
  libFile->GetPath(path);
  aOutLibPath = NS_ConvertUTF16toUTF8(path);

  return true;
#endif
}

mozilla::ipc::IPCResult GMPChild::AnswerStartPlugin(const nsString& aAdapter) {
  GMP_CHILD_LOG_DEBUG("%s", __FUNCTION__);

  nsCString libPath;
  if (!GetUTF8LibPath(libPath)) {
    CrashReporter::AnnotateCrashReport(
        CrashReporter::Annotation::GMPLibraryPath,
        NS_ConvertUTF16toUTF8(mPluginPath));

#ifdef XP_WIN
    return IPC_FAIL(this,
                    nsPrintfCString("Failed to get lib path with error(%d).",
                                    GetLastError())
                        .get());
#else
    return IPC_FAIL(this, "Failed to get lib path.");
#endif
  }

  auto platformAPI = new GMPPlatformAPI();
  InitPlatformAPI(*platformAPI, this);

  mGMPLoader = MakeUnique<GMPLoader>();
#if defined(MOZ_SANDBOX)
  if (!mGMPLoader->CanSandbox()) {
    GMP_CHILD_LOG_DEBUG("%s Can't sandbox GMP, failing", __FUNCTION__);
    delete platformAPI;
    return IPC_FAIL(this, "Can't sandbox GMP.");
  }
#endif

  GMPAdapter* adapter = nullptr;

  if (!mGMPLoader->Load(libPath.get(), libPath.Length(), platformAPI,
                        adapter)) {
    NS_WARNING("Failed to load GMP");
    delete platformAPI;
    CrashReporter::AnnotateCrashReport(
        CrashReporter::Annotation::GMPLibraryPath,
        NS_ConvertUTF16toUTF8(mPluginPath));

#ifdef XP_WIN
    return IPC_FAIL(this, nsPrintfCString("Failed to load GMP with error(%d).",
                                          GetLastError())
                              .get());
#else
    return IPC_FAIL(this, "Failed to load GMP.");
#endif
  }

  return IPC_OK();
}

MessageLoop* GMPChild::GMPMessageLoop() { return mGMPMessageLoop; }

void GMPChild::ActorDestroy(ActorDestroyReason aWhy) {
  GMP_CHILD_LOG_DEBUG("%s reason=%d", __FUNCTION__, aWhy);

  for (uint32_t i = mGMPContentChildren.Length(); i > 0; i--) {
    MOZ_ASSERT_IF(aWhy == NormalShutdown,
                  !mGMPContentChildren[i - 1]->IsUsed());
    mGMPContentChildren[i - 1]->Close();
  }

  if (mGMPLoader) {
    mGMPLoader->Shutdown();
  }
  if (AbnormalShutdown == aWhy) {
    NS_WARNING("Abnormal shutdown of GMP process!");
    ProcessChild::QuickExit();
  }

  CrashReporterClient::DestroySingleton();

  XRE_ShutdownChildProcess();
}

void GMPChild::ProcessingError(Result aCode, const char* aReason) {
  switch (aCode) {
    case MsgDropped:
      _exit(0);  // Don't trigger a crash report.
    case MsgNotKnown:
      MOZ_CRASH("aborting because of MsgNotKnown");
    case MsgNotAllowed:
      MOZ_CRASH("aborting because of MsgNotAllowed");
    case MsgPayloadError:
      MOZ_CRASH("aborting because of MsgPayloadError");
    case MsgProcessingError:
      MOZ_CRASH("aborting because of MsgProcessingError");
    case MsgRouteError:
      MOZ_CRASH("aborting because of MsgRouteError");
    case MsgValueError:
      MOZ_CRASH("aborting because of MsgValueError");
    default:
      MOZ_CRASH("not reached");
  }
}

PGMPTimerChild* GMPChild::AllocPGMPTimerChild() {
  return new GMPTimerChild(this);
}

bool GMPChild::DeallocPGMPTimerChild(PGMPTimerChild* aActor) {
  MOZ_ASSERT(mTimerChild == static_cast<GMPTimerChild*>(aActor));
  mTimerChild = nullptr;
  return true;
}

GMPTimerChild* GMPChild::GetGMPTimers() {
  if (!mTimerChild) {
    PGMPTimerChild* sc = SendPGMPTimerConstructor();
    if (!sc) {
      return nullptr;
    }
    mTimerChild = static_cast<GMPTimerChild*>(sc);
  }
  return mTimerChild;
}

PGMPStorageChild* GMPChild::AllocPGMPStorageChild() {
  return new GMPStorageChild(this);
}

bool GMPChild::DeallocPGMPStorageChild(PGMPStorageChild* aActor) {
  mStorage = nullptr;
  return true;
}

GMPStorageChild* GMPChild::GetGMPStorage() {
  if (!mStorage) {
    PGMPStorageChild* sc = SendPGMPStorageConstructor();
    if (!sc) {
      return nullptr;
    }
    mStorage = static_cast<GMPStorageChild*>(sc);
  }
  return mStorage;
}

mozilla::ipc::IPCResult GMPChild::RecvCrashPluginNow() {
  MOZ_CRASH();
  return IPC_OK();
}

mozilla::ipc::IPCResult GMPChild::RecvCloseActive() {
  for (uint32_t i = mGMPContentChildren.Length(); i > 0; i--) {
    mGMPContentChildren[i - 1]->CloseActive();
  }
  return IPC_OK();
}

mozilla::ipc::IPCResult GMPChild::RecvInitGMPContentChild(
    Endpoint<PGMPContentChild>&& aEndpoint) {
  GMPContentChild* child =
      mGMPContentChildren.AppendElement(new GMPContentChild(this))->get();
  aEndpoint.Bind(child);
  return IPC_OK();
}

void GMPChild::GMPContentChildActorDestroy(GMPContentChild* aGMPContentChild) {
  for (uint32_t i = mGMPContentChildren.Length(); i > 0; i--) {
    UniquePtr<GMPContentChild>& toDestroy = mGMPContentChildren[i - 1];
    if (toDestroy.get() == aGMPContentChild) {
      SendPGMPContentChildDestroyed();
      RefPtr<DeleteTask<GMPContentChild>> task =
          new DeleteTask<GMPContentChild>(toDestroy.release());
      MessageLoop::current()->PostTask(task.forget());
      mGMPContentChildren.RemoveElementAt(i - 1);
      break;
    }
  }
}

}  // namespace gmp
}  // namespace mozilla

#undef GMP_CHILD_LOG_DEBUG
#undef __CLASS__
