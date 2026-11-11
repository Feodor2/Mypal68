/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/ArrayUtils.h"

#include "nsSystemInfo.h"
#include "prsystem.h"
#include "prio.h"
#include "mozilla/SSE.h"
#include "mozilla/arm.h"
#include "mozilla/Sprintf.h"

#ifdef XP_WIN
#  include <comutil.h>
#  include <time.h>
#  ifndef __MINGW32__
#    include <iwscapi.h>
#  endif  // __MINGW32__
#  include <windows.h>
#  include <winioctl.h>
#  ifndef __MINGW32__
#    include <wscapi.h>
#  endif  // __MINGW32__
#  include "base/scoped_handle_win.h"
#  include "nsAppDirectoryServiceDefs.h"
#  include "nsDirectoryServiceDefs.h"
#  include "nsDirectoryServiceUtils.h"
#  include "nsIObserverService.h"
#  include "nsWindowsHelpers.h"

#endif

#ifdef XP_MACOSX
#  include "MacHelpers.h"
#endif

#ifdef MOZ_WIDGET_GTK
#  include <gtk/gtk.h>
#  include <dlfcn.h>
#endif

#if defined(XP_LINUX) && !defined(ANDROID)
#  include <unistd.h>
#  include <fstream>
#  include "mozilla/Tokenizer.h"
#  include "nsCharSeparatedTokenizer.h"

#  include <map>
#  include <string>
#endif

#ifdef MOZ_WIDGET_ANDROID
#  include "AndroidBuild.h"
#  include "GeneratedJNIWrappers.h"
#  include "mozilla/jni/Utils.h"
#endif

#ifdef XP_MACOSX
#  include <sys/sysctl.h>
#endif

#if defined(XP_LINUX) && defined(MOZ_SANDBOX)
#  include "mozilla/SandboxInfo.h"
#endif

// Slot for NS_InitXPCOM to pass information to nsSystemInfo::Init.
// Only set to nonzero (potentially) if XP_UNIX.  On such systems, the
// system call to discover the appropriate value is not thread-safe,
// so we must call it before going multithreaded, but nsSystemInfo::Init
// only happens well after that point.
uint32_t nsSystemInfo::gUserUmask = 0;

using namespace mozilla::dom;

#if defined(XP_LINUX) && !defined(ANDROID)
static void SimpleParseKeyValuePairs(
    const std::string& aFilename,
    std::map<nsCString, nsCString>& aKeyValuePairs) {
  std::ifstream input(aFilename.c_str());
  for (std::string line; std::getline(input, line);) {
    nsAutoCString key, value;

    nsCCharSeparatedTokenizer tokens(nsDependentCString(line.c_str()), ':');
    if (tokens.hasMoreTokens()) {
      key = tokens.nextToken();
      if (tokens.hasMoreTokens()) {
        value = tokens.nextToken();
      }
      // We want the value even if there was just one token, to cover the
      // case where we had the key, and the value was blank (seems to be
      // a valid scenario some files.)
      aKeyValuePairs[key] = value;
    }
  }
}
#endif

#if defined(XP_WIN)
namespace {
nsresult GetCountryCode(nsAString& aCountryCode) {
  GEOID geoid = GetUserGeoID(GEOCLASS_NATION);
  if (geoid == GEOID_NOT_AVAILABLE) {
    return NS_ERROR_NOT_AVAILABLE;
  }
  // Get required length
  int numChars = GetGeoInfoW(geoid, GEO_ISO2, nullptr, 0, 0);
  if (!numChars) {
    return NS_ERROR_FAILURE;
  }
  // Now get the string for real
  aCountryCode.SetLength(numChars);
  numChars =
      GetGeoInfoW(geoid, GEO_ISO2, char16ptr_t(aCountryCode.BeginWriting()),
                  aCountryCode.Length(), 0);
  if (!numChars) {
    return NS_ERROR_FAILURE;
  }

  // numChars includes null terminator
  aCountryCode.Truncate(numChars - 1);
  return NS_OK;
}

}  // namespace

#endif  // defined(XP_WIN)

#ifdef XP_MACOSX
static nsresult GetAppleModelId(nsAutoCString& aModelId) {
  size_t numChars = 0;
  size_t result = sysctlbyname("hw.model", nullptr, &numChars, nullptr, 0);
  if (result != 0 || !numChars) {
    return NS_ERROR_FAILURE;
  }
  aModelId.SetLength(numChars);
  result =
      sysctlbyname("hw.model", aModelId.BeginWriting(), &numChars, nullptr, 0);
  if (result != 0) {
    return NS_ERROR_FAILURE;
  }
  // numChars includes null terminator
  aModelId.Truncate(numChars - 1);
  return NS_OK;
}
#endif

using namespace mozilla;

nsSystemInfo::nsSystemInfo() {}

nsSystemInfo::~nsSystemInfo() {}

// CPU-specific information.
static const struct PropItems {
  const char* name;
  bool (*propfun)(void);
} cpuPropItems[] = {
    // x86-specific bits.
    {"hasMMX", mozilla::supports_mmx},
    {"hasSSE", mozilla::supports_sse},
    {"hasSSE2", mozilla::supports_sse2},
    {"hasSSE3", mozilla::supports_sse3},
    {"hasSSSE3", mozilla::supports_ssse3},
    {"hasSSE4A", mozilla::supports_sse4a},
    {"hasSSE4_1", mozilla::supports_sse4_1},
    {"hasSSE4_2", mozilla::supports_sse4_2},
    {"hasAVX", mozilla::supports_avx},
    {"hasAVX2", mozilla::supports_avx2},
    {"hasAES", mozilla::supports_aes},
    // ARM-specific bits.
    {"hasEDSP", mozilla::supports_edsp},
    {"hasARMv6", mozilla::supports_armv6},
    {"hasARMv7", mozilla::supports_armv7},
    {"hasNEON", mozilla::supports_neon}};

nsresult nsSystemInfo::Init() {
  // This uses the observer service on Windows, so for simplicity
  // check that it is called from the main thread on all platforms.
  MOZ_ASSERT(NS_IsMainThread());

  nsresult rv;

  static const struct {
    PRSysInfo cmd;
    const char* name;
  } items[] = {{PR_SI_SYSNAME, "name"},
               {PR_SI_ARCHITECTURE, "arch"},
               {PR_SI_RELEASE, "version"}};

  for (uint32_t i = 0; i < (sizeof(items) / sizeof(items[0])); i++) {
    char buf[SYS_INFO_BUFFER_LENGTH];
    if (PR_GetSystemInfo(items[i].cmd, buf, sizeof(buf)) == PR_SUCCESS) {
      rv = SetPropertyAsACString(NS_ConvertASCIItoUTF16(items[i].name),
                                 nsDependentCString(buf));
      if (NS_WARN_IF(NS_FAILED(rv))) {
        return rv;
      }
    } else {
      NS_WARNING("PR_GetSystemInfo failed");
    }
  }

  rv = SetPropertyAsBool(NS_ConvertASCIItoUTF16("hasWindowsTouchInterface"),
                         false);
  NS_ENSURE_SUCCESS(rv, rv);

  SetUint64Property(u"memsize"_ns, PR_GetPhysicalMemorySize());

#ifdef XP_WIN
  bool isMinGW =
#  ifdef __MINGW32__
      true;
#  else
      false;
#  endif
  rv = SetPropertyAsBool(u"isMinGW"_ns, !!isMinGW);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return rv;
  }

  BOOL isWow64 = false;
  USHORT processMachine = IMAGE_FILE_MACHINE_UNKNOWN;
  USHORT nativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
  BOOL gotWow64Value = IsWow64Process(GetCurrentProcess(), &isWow64);
  // The function only indicates a WOW64 environment if it's 32-bit x86
  // running on x86-64, so emulate what IsWow64Process2 would have given.
  if (gotWow64Value && isWow64) {
    processMachine = IMAGE_FILE_MACHINE_I386;
    nativeMachine = IMAGE_FILE_MACHINE_AMD64;
  }
  NS_WARNING_ASSERTION(gotWow64Value, "IsWow64Process failed");
  if (gotWow64Value) {
    // Set this always, even for the x86-on-arm64 case.
    rv = SetPropertyAsBool(NS_LITERAL_STRING("isWow64"), !!isWow64);
    if (NS_WARN_IF(NS_FAILED(rv))) {
      return rv;
    }
    // Additional information if we're running x86-on-arm64
    bool isWowARM64 = (processMachine == IMAGE_FILE_MACHINE_I386 &&
                       nativeMachine == 0xAA64);
    rv = SetPropertyAsBool(NS_LITERAL_STRING("isWowARM64"), !!isWowARM64);
    if (NS_WARN_IF(NS_FAILED(rv))) {
      return rv;
    }
  }
  nsAutoString countryCode;
  if (NS_SUCCEEDED(GetCountryCode(countryCode))) {
    rv = SetPropertyAsAString(NS_LITERAL_STRING("countryCode"), countryCode);
    NS_ENSURE_SUCCESS(rv, rv);
  }

#endif

#if defined(XP_MACOSX)
  nsAutoString countryCode;
  if (NS_SUCCEEDED(GetSelectedCityInfo(countryCode))) {
    rv = SetPropertyAsAString(NS_LITERAL_STRING("countryCode"), countryCode);
    NS_ENSURE_SUCCESS(rv, rv);
  }

  nsAutoCString modelId;
  if (NS_SUCCEEDED(GetAppleModelId(modelId))) {
    rv = SetPropertyAsACString(NS_LITERAL_STRING("appleModelId"), modelId);
    NS_ENSURE_SUCCESS(rv, rv);
  }
#endif

#if defined(MOZ_WIDGET_GTK)
  // This must be done here because NSPR can only separate OS's when compiled,
  // not libraries. 64 bytes is going to be well enough for "GTK " followed by 3
  // integers separated with dots.
  char gtkver[64];
  ssize_t gtkver_len = 0;

  if (gtkver_len <= 0) {
    gtkver_len = SprintfLiteral(gtkver, "GTK %u.%u.%u", gtk_major_version,
                                gtk_minor_version, gtk_micro_version);
  }

  nsAutoCString secondaryLibrary;
  if (gtkver_len > 0 && gtkver_len < int(sizeof(gtkver))) {
    secondaryLibrary.Append(nsDependentCSubstring(gtkver, gtkver_len));
  }

  void* libpulse = dlopen("libpulse.so.0", RTLD_LAZY);
  const char* libpulseVersion = "not-available";
  if (libpulse) {
    auto pa_get_library_version = reinterpret_cast<const char* (*)()>(
        dlsym(libpulse, "pa_get_library_version"));

    if (pa_get_library_version) {
      libpulseVersion = pa_get_library_version();
    }
  }

  secondaryLibrary.AppendPrintf(",libpulse %s", libpulseVersion);

  if (libpulse) {
    dlclose(libpulse);
  }

  rv = SetPropertyAsACString(u"secondaryLibrary"_ns, secondaryLibrary);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return rv;
  }
#endif

#ifdef MOZ_WIDGET_ANDROID
  AndroidSystemInfo info;
  GetAndroidSystemInfo(&info);
  SetupAndroidInfo(info);
#endif

#if defined(XP_LINUX) && defined(MOZ_SANDBOX)
  SandboxInfo sandInfo = SandboxInfo::Get();

  SetPropertyAsBool(u"hasSeccompBPF"_ns,
                    sandInfo.Test(SandboxInfo::kHasSeccompBPF));
  SetPropertyAsBool(u"hasSeccompTSync"_ns,
                    sandInfo.Test(SandboxInfo::kHasSeccompTSync));
  SetPropertyAsBool(u"hasUserNamespaces"_ns,
                    sandInfo.Test(SandboxInfo::kHasUserNamespaces));
  SetPropertyAsBool(u"hasPrivilegedUserNamespaces"_ns,
                    sandInfo.Test(SandboxInfo::kHasPrivilegedUserNamespaces));

  if (sandInfo.Test(SandboxInfo::kEnabledForContent)) {
    SetPropertyAsBool(u"canSandboxContent"_ns, sandInfo.CanSandboxContent());
  }

  if (sandInfo.Test(SandboxInfo::kEnabledForMedia)) {
    SetPropertyAsBool(u"canSandboxMedia"_ns, sandInfo.CanSandboxMedia());
  }
#endif  // XP_LINUX && MOZ_SANDBOX

  return NS_OK;
}

#ifdef MOZ_WIDGET_ANDROID
// Prerelease versions of Android use a letter instead of version numbers.
// Unfortunately this breaks websites due to the user agent.
// Chrome works around this by hardcoding an Android version when a
// numeric version can't be obtained. We're doing the same.
// This version will need to be updated whenever there is a new official
// Android release.
// See: https://cs.chromium.org/chromium/src/base/sys_info_android.cc?l=61
#  define DEFAULT_ANDROID_VERSION "6.0.99"

/* static */
void nsSystemInfo::GetAndroidSystemInfo(AndroidSystemInfo* aInfo) {
  if (!jni::IsAvailable()) {
    // called from xpcshell etc.
    aInfo->sdk_version() = 0;
    return;
  }

  jni::String::LocalRef model = java::sdk::Build::MODEL();
  aInfo->device() = model->ToString();

  jni::String::LocalRef manufacturer =
      mozilla::java::sdk::Build::MANUFACTURER();
  aInfo->manufacturer() = manufacturer->ToString();

  jni::String::LocalRef hardware = java::sdk::Build::HARDWARE();
  aInfo->hardware() = hardware->ToString();

  jni::String::LocalRef release = java::sdk::VERSION::RELEASE();
  nsString str(release->ToString());
  int major_version;
  int minor_version;
  int bugfix_version;
  int num_read = sscanf(NS_ConvertUTF16toUTF8(str).get(), "%d.%d.%d",
                        &major_version, &minor_version, &bugfix_version);
  if (num_read == 0) {
    aInfo->release_version() = NS_LITERAL_STRING(DEFAULT_ANDROID_VERSION);
  } else {
    aInfo->release_version() = str;
  }

  aInfo->sdk_version() = jni::GetAPIVersion();
  aInfo->isTablet() = java::GeckoAppShell::IsTablet();
}

void nsSystemInfo::SetupAndroidInfo(const AndroidSystemInfo& aInfo) {
  if (!aInfo.device().IsEmpty()) {
    SetPropertyAsAString(u"device"_ns, aInfo.device());
  }
  if (!aInfo.manufacturer().IsEmpty()) {
    SetPropertyAsAString(u"manufacturer"_ns, aInfo.manufacturer());
  }
  if (!aInfo.release_version().IsEmpty()) {
    SetPropertyAsAString(u"release_version"_ns, aInfo.release_version());
  }
  SetPropertyAsBool(u"tablet"_ns, aInfo.isTablet());
  // NSPR "version" is the kernel version. For Android we want the Android
  // version. Rename SDK version to version and put the kernel version into
  // kernel_version.
  nsAutoString str;
  nsresult rv = GetPropertyAsAString(u"version"_ns, str);
  if (NS_SUCCEEDED(rv)) {
    SetPropertyAsAString(u"kernel_version"_ns, str);
  }
  // When JNI is not available (eg. in xpcshell tests), sdk_version is 0.
  if (aInfo.sdk_version() != 0) {
    if (!aInfo.hardware().IsEmpty()) {
      SetPropertyAsAString(u"hardware"_ns, aInfo.hardware());
    }
    SetPropertyAsInt32(u"version"_ns, aInfo.sdk_version());
  }
}
#endif  // MOZ_WIDGET_ANDROID

void nsSystemInfo::SetInt32Property(const nsAString& aPropertyName,
                                    const int32_t aValue) {
  NS_WARNING_ASSERTION(aValue > 0, "Unable to read system value");
  if (aValue > 0) {
#ifdef DEBUG
    nsresult rv =
#endif
        SetPropertyAsInt32(aPropertyName, aValue);
    NS_WARNING_ASSERTION(NS_SUCCEEDED(rv), "Unable to set property");
  }
}

void nsSystemInfo::SetUint32Property(const nsAString& aPropertyName,
                                     const uint32_t aValue) {
  // Only one property is currently set via this function.
  // It may legitimately be zero.
#ifdef DEBUG
  nsresult rv =
#endif
      SetPropertyAsUint32(aPropertyName, aValue);
  NS_WARNING_ASSERTION(NS_SUCCEEDED(rv), "Unable to set property");
}

void nsSystemInfo::SetUint64Property(const nsAString& aPropertyName,
                                     const uint64_t aValue) {
  NS_WARNING_ASSERTION(aValue > 0, "Unable to read system value");
  if (aValue > 0) {
#ifdef DEBUG
    nsresult rv =
#endif
        SetPropertyAsUint64(aPropertyName, aValue);
    NS_WARNING_ASSERTION(NS_SUCCEEDED(rv), "Unable to set property");
  }
}

#if defined(XP_WIN)
NS_IMETHODIMP
nsSystemInfo::Observe(nsISupports* aSubject, const char* aTopic,
                      const char16_t* aData) {
  if (!strcmp(aTopic, "profile-do-change")) {
    nsresult rv;
    nsCOMPtr<nsIObserverService> obsService =
        do_GetService(NS_OBSERVERSERVICE_CONTRACTID, &rv);
    if (NS_FAILED(rv)) {
      return rv;
    }
    rv = obsService->RemoveObserver(this, "profile-do-change");
    if (NS_FAILED(rv)) {
      return rv;
    }
  }
  return NS_OK;
}

NS_IMPL_ISUPPORTS_INHERITED(nsSystemInfo, nsHashPropertyBag, nsIObserver)
#endif  // defined(XP_WIN)
