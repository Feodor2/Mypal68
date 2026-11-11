/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AutoplayPolicy.h"

#include "mozilla/dom/AudioContext.h"
#include "mozilla/dom/Document.h"
#include "mozilla/dom/FeaturePolicyUtils.h"
#include "mozilla/dom/HTMLMediaElement.h"
#include "mozilla/dom/HTMLMediaElementBinding.h"
#include "mozilla/dom/UserActivation.h"
#include "mozilla/dom/WindowContext.h"
#include "mozilla/Logging.h"
#include "mozilla/MediaManager.h"
#include "mozilla/Preferences.h"
#include "mozilla/Components.h"
#include "mozilla/StaticPrefs_media.h"
#include "nsContentUtils.h"
#include "nsGlobalWindowInner.h"
#include "nsIAutoplay.h"
#include "nsIDocShell.h"
#include "nsIDocShellTreeItem.h"
#include "nsIPermissionManager.h"
#include "nsIPrincipal.h"
#include "nsPIDOMWindow.h"

mozilla::LazyLogModule gAutoplayPermissionLog("Autoplay");

#define AUTOPLAY_LOG(msg, ...) \
  MOZ_LOG(gAutoplayPermissionLog, LogLevel::Debug, (msg, ##__VA_ARGS__))

namespace mozilla::dom {

static const uint32_t sPOLICY_STICKY_ACTIVATION = 0;
static const uint32_t sPOLICY_TRANSIENT_ACTIVATION = 1;
static const uint32_t sPOLICY_USER_INPUT_DEPTH = 2;

static bool IsActivelyCapturingOrHasAPermission(nsPIDOMWindowInner* aWindow) {
  // Pages which have been granted permission to capture WebRTC camera or
  // microphone or screen are assumed to be trusted, and are allowed to
  // autoplay.
  if (MediaManager::GetIfExists()) {
    return MediaManager::GetIfExists()->IsActivelyCapturingOrHasAPermission(
        aWindow->WindowID());
  }

  auto principal = nsGlobalWindowInner::Cast(aWindow)->GetPrincipal();
  return (nsContentUtils::IsExactSitePermAllow(principal, "camera"_ns) ||
          nsContentUtils::IsExactSitePermAllow(principal, "microphone"_ns) ||
          nsContentUtils::IsExactSitePermAllow(principal, "screen"_ns));
}

static uint32_t SiteAutoplayPerm(nsPIDOMWindowInner* aWindow) {
  if (!aWindow || !aWindow->GetBrowsingContext()) {
    return nsIPermissionManager::DENY_ACTION;
  }
  return aWindow->GetBrowsingContext()
      ->GetTopWindowContext()
      ->GetAutoplayPermission();
}

static bool IsWindowAllowedToPlay(nsPIDOMWindowInner* aWindow) {
  if (!aWindow) {
    return false;
  }

  if (IsActivelyCapturingOrHasAPermission(aWindow)) {
    AUTOPLAY_LOG(
        "Allow autoplay as document has camera or microphone or screen"
        " permission.");
    return true;
  }

  RefPtr<BrowsingContext> topLevelBC = aWindow->GetBrowsingContext()->Top();
  if (topLevelBC->HasBeenUserGestureActivated()) {
    AUTOPLAY_LOG(
        "Allow autoplay as top-level context has been activated by user "
        "gesture.");
    return true;
  }

  Document* currentDoc = aWindow->GetExtantDoc();
  if (!currentDoc) {
    return false;
  }

  bool isTopLevelContent = !aWindow->GetBrowsingContext()->GetParent();
  if (currentDoc->MediaDocumentKind() == Document::MediaDocumentKind::Video &&
      isTopLevelContent) {
    AUTOPLAY_LOG("Allow top-level video document to autoplay.");
    return true;
  }

  if (currentDoc->IsExtensionPage()) {
    AUTOPLAY_LOG("Allow autoplay as in extension document.");
    return true;
  }

  return false;
}

static uint32_t DefaultAutoplayBehaviour() {
  int prefValue =
      Preferences::GetInt("media.autoplay.default", nsIAutoplay::ALLOWED);
  if (prefValue == nsIAutoplay::ALLOWED) {
    return nsIAutoplay::ALLOWED;
  }
  if (prefValue == nsIAutoplay::BLOCKED_ALL) {
    return nsIAutoplay::BLOCKED_ALL;
  }
  return nsIAutoplay::BLOCKED;
}

static bool IsMediaElementInaudible(const HTMLMediaElement& aElement) {
  if (aElement.Volume() == 0.0 || aElement.Muted()) {
    AUTOPLAY_LOG("Media %p is muted.", &aElement);
    return true;
  }

  if (!aElement.HasAudio() &&
      aElement.ReadyState() >= HTMLMediaElement_Binding::HAVE_METADATA) {
    AUTOPLAY_LOG("Media %p has no audio track", &aElement);
    return true;
  }

  return false;
}

static bool IsAudioContextAllowedToPlay(const AudioContext& aContext) {
  // Offline context won't directly output sound to audio devices.
  return aContext.IsOffline() ||
         IsWindowAllowedToPlay(aContext.GetParentObject());
}

static bool IsEnableBlockingWebAudioByUserGesturePolicy() {
  return Preferences::GetBool("media.autoplay.block-webaudio", false) &&
         StaticPrefs::media_autoplay_blocking_policy() ==
             sPOLICY_STICKY_ACTIVATION;
}

static bool IsAllowedToPlayByBlockingModel(const HTMLMediaElement& aElement) {
  const uint32_t policy = StaticPrefs::media_autoplay_blocking_policy();
  if (policy == sPOLICY_STICKY_ACTIVATION) {
    const bool isAllowed =
        IsWindowAllowedToPlay(aElement.OwnerDoc()->GetInnerWindow());
    AUTOPLAY_LOG("Use 'sticky-activation', isAllowed=%d", isAllowed);
    return isAllowed;
  }
  // If element is blessed, it would always be allowed to play().
  const bool isElementBlessed = aElement.IsBlessed();
  if (policy == sPOLICY_USER_INPUT_DEPTH) {
    const bool isUserInput = UserActivation::IsHandlingUserInput();
    AUTOPLAY_LOG("Use 'User-Input-Depth', isBlessed=%d, isUserInput=%d",
                 isElementBlessed, isUserInput);
    return isElementBlessed || isUserInput;
  }
  const bool hasTransientActivation =
      aElement.OwnerDoc()->HasValidTransientUserGestureActivation();
  AUTOPLAY_LOG(
      "Use 'transient-activation', isBlessed=%d, "
      "hasValidTransientActivation=%d",
      isElementBlessed, hasTransientActivation);
  return isElementBlessed || hasTransientActivation;
}

static bool IsAllowedToPlayInternal(const HTMLMediaElement& aElement) {
  bool isInaudible = IsMediaElementInaudible(aElement);
  bool isUsingAutoplayModel = IsAllowedToPlayByBlockingModel(aElement);

  uint32_t defaultBehaviour = DefaultAutoplayBehaviour();
  uint32_t sitePermission =
      SiteAutoplayPerm(aElement.OwnerDoc()->GetInnerWindow());

  AUTOPLAY_LOG(
      "IsAllowedToPlayInternal, isInaudible=%d,"
      "isUsingAutoplayModel=%d, sitePermission=%d, defaultBehaviour=%d",
      isInaudible, isUsingAutoplayModel, sitePermission, defaultBehaviour);

  // For site permissions we store permissionManager values except
  // for BLOCKED_ALL, for the default pref values we store
  // nsIAutoplay values.
  if (sitePermission == nsIPermissionManager::ALLOW_ACTION) {
    return true;
  }

  if (sitePermission == nsIPermissionManager::DENY_ACTION) {
    return isInaudible || isUsingAutoplayModel;
  }

  if (sitePermission == nsIAutoplay::BLOCKED_ALL) {
    return isUsingAutoplayModel;
  }

  if (defaultBehaviour == nsIAutoplay::ALLOWED) {
    return true;
  }

  if (defaultBehaviour == nsIAutoplay::BLOCKED) {
    return isInaudible || isUsingAutoplayModel;
  }

  MOZ_ASSERT(defaultBehaviour == nsIAutoplay::BLOCKED_ALL);
  return isUsingAutoplayModel;
}

/* static */
uint32_t AutoplayPolicy::GetSiteAutoplayPermission(nsIPrincipal* aPrincipal) {
  if (!aPrincipal) {
    return nsIPermissionManager::DENY_ACTION;
  }

  nsCOMPtr<nsIPermissionManager> permMgr = services::GetPermissionManager();
  if (!permMgr) {
    return nsIPermissionManager::DENY_ACTION;
  }

  uint32_t perm = nsIPermissionManager::DENY_ACTION;
  permMgr->TestExactPermissionFromPrincipal(
      aPrincipal, NS_LITERAL_CSTRING("autoplay-media"), &perm);
  return perm;
}

/* static */
bool AutoplayPolicy::IsAllowedToPlay(const HTMLMediaElement& aElement) {
  const bool result = IsAllowedToPlayInternal(aElement);
  AUTOPLAY_LOG("IsAllowedToPlay, mediaElement=%p, isAllowToPlay=%s", &aElement,
               result ? "allowed" : "blocked");
  return result;
}

/* static */
bool AutoplayPolicy::IsAllowedToPlay(const AudioContext& aContext) {
  /**
   * The autoplay checking has 5 different phases,
   * 1. check whether audio context itself meets the autoplay condition
   * 2. check if we enable blocking web audio or not
   *    (only support blocking when using user-gesture-activation model)
   * 3. check whether the site is in the autoplay whitelist
   * 4. check global autoplay setting and check wether the site is in the
   *    autoplay blacklist.
   * 5. check whether media is allowed under current blocking model
   *    (only support user-gesture-activation model)
   */
  if (aContext.IsOffline()) {
    return true;
  }

  if (!IsEnableBlockingWebAudioByUserGesturePolicy()) {
    return true;
  }

  nsPIDOMWindowInner* window = aContext.GetParentObject();
  uint32_t sitePermission = SiteAutoplayPerm(window);

  if (sitePermission == nsIPermissionManager::ALLOW_ACTION) {
    AUTOPLAY_LOG(
        "Allow autoplay as document has permanent autoplay permission.");
    return true;
  }

  if (DefaultAutoplayBehaviour() == nsIAutoplay::ALLOWED &&
      sitePermission != nsIPermissionManager::DENY_ACTION &&
      sitePermission != nsIAutoplay::BLOCKED_ALL) {
    AUTOPLAY_LOG(
        "Allow autoplay as global autoplay setting is allowing autoplay by "
        "default.");
    return true;
  }

  return IsWindowAllowedToPlay(window);
}

/* static */
DocumentAutoplayPolicy AutoplayPolicy::IsAllowedToPlay(
    const Document& aDocument) {
  if (DefaultAutoplayBehaviour() == nsIAutoplay::ALLOWED ||
      IsWindowAllowedToPlay(aDocument.GetInnerWindow())) {
    return DocumentAutoplayPolicy::Allowed;
  }

  if (DefaultAutoplayBehaviour() == nsIAutoplay::BLOCKED) {
    return DocumentAutoplayPolicy::Allowed_muted;
  }

  return DocumentAutoplayPolicy::Disallowed;
}

}  // namespace mozilla::dom
