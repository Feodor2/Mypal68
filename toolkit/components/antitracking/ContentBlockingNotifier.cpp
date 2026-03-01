/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AntiTrackingLog.h"
#include "ContentBlockingNotifier.h"
#include "AntiTrackingUtils.h"

#include "mozilla/AbstractEventQueue.h"
#include "mozilla/StaticPrefs_privacy.h"
#include "mozilla/dom/BrowserChild.h"
#include "nsGlobalWindowInner.h"
#include "nsIClassifiedChannel.h"
#include "nsIOService.h"
#include "nsIRunnable.h"
#include "nsIParentChannel.h"
#include "nsIScriptError.h"
#include "nsIURI.h"
#include "nsJSUtils.h"
#include "mozIThirdPartyUtil.h"

using namespace mozilla;
using mozilla::dom::BrowsingContext;
using mozilla::dom::ContentChild;
using mozilla::dom::Document;

static const uint32_t kMaxConsoleOutputDelayMs = 100;

namespace {

void RunConsoleReportingRunnable(already_AddRefed<nsIRunnable>&& aRunnable) {
  if (StaticPrefs::privacy_restrict3rdpartystorage_console_lazy()) {
    nsresult rv = NS_DispatchToCurrentThreadQueue(std::move(aRunnable),
                                                  kMaxConsoleOutputDelayMs,
                                                  EventQueuePriority::Idle);
    if (NS_WARN_IF(NS_FAILED(rv))) {
      return;
    }
  } else {
    nsCOMPtr<nsIRunnable> runnable(std::move(aRunnable));
    nsresult rv = runnable->Run();
    if (NS_WARN_IF(NS_FAILED(rv))) {
      return;
    }
  }
}

void ReportBlockingToConsole(nsPIDOMWindowOuter* aWindow, nsIURI* aURI,
                             uint32_t aRejectedReason) {
  MOZ_ASSERT(aWindow && aURI);
  MOZ_ASSERT(
      aRejectedReason == 0 ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_BLOCKED_BY_PERMISSION ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_BLOCKED_TRACKER ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_PARTITIONED_FOREIGN ||
      aRejectedReason == nsIWebProgressListener::STATE_COOKIES_BLOCKED_ALL ||
      aRejectedReason == nsIWebProgressListener::STATE_COOKIES_BLOCKED_FOREIGN);

  RefPtr<Document> doc = aWindow->GetExtantDoc();
  if (NS_WARN_IF(!doc)) {
    return;
  }

  nsAutoString sourceLine;
  uint32_t lineNumber = 0, columnNumber = 0;
  JSContext* cx = nsContentUtils::GetCurrentJSContext();
  if (cx) {
    nsJSUtils::GetCallingLocation(cx, sourceLine, &lineNumber, &columnNumber);
  }

  nsCOMPtr<nsIURI> uri(aURI);

  RefPtr<Runnable> runnable = NS_NewRunnableFunction(
      "ReportBlockingToConsoleDelayed",
      [doc, sourceLine, lineNumber, columnNumber, uri, aRejectedReason]() {
        const char* message = nullptr;
        nsAutoCString category;
        // When changing this list, please make sure to update the corresponding
        // code in antitracking_head.js (inside _createTask).
        switch (aRejectedReason) {
          case nsIWebProgressListener::STATE_COOKIES_BLOCKED_BY_PERMISSION:
            message = "CookieBlockedByPermission";
            category = NS_LITERAL_CSTRING("cookieBlockedPermission");
            break;

          case nsIWebProgressListener::STATE_COOKIES_BLOCKED_TRACKER:
            message = "CookieBlockedTracker";
            category = NS_LITERAL_CSTRING("cookieBlockedTracker");
            break;

          case nsIWebProgressListener::STATE_COOKIES_BLOCKED_ALL:
            message = "CookieBlockedAll";
            category = NS_LITERAL_CSTRING("cookieBlockedAll");
            break;

          case nsIWebProgressListener::STATE_COOKIES_BLOCKED_FOREIGN:
            message = "CookieBlockedForeign";
            category = NS_LITERAL_CSTRING("cookieBlockedForeign");
            break;

          default:
            return;
        }

        MOZ_ASSERT(message);

        // Strip the URL of any possible username/password and make it ready
        // to be presented in the UI.
        nsCOMPtr<nsIURI> exposableURI =
            net::nsIOService::CreateExposableURI(uri);

        AutoTArray<nsString, 1> params;
        CopyUTF8toUTF16(exposableURI->GetSpecOrDefault(),
                        *params.AppendElement());

        nsContentUtils::ReportToConsole(nsIScriptError::warningFlag, category,
                                        doc, nsContentUtils::eNECKO_PROPERTIES,
                                        message, params, nullptr, sourceLine,
                                        lineNumber, columnNumber);
      });

  RunConsoleReportingRunnable(runnable.forget());
}

void NotifyBlockingDecision(nsIChannel* aReportingChannel,
                            nsIChannel* aTrackingChannel,
                            ContentBlockingNotifier::BlockingDecision aDecision,
                            uint32_t aRejectedReason, nsIURI* aURI,
                            nsPIDOMWindowOuter* aWindow) {
  MOZ_ASSERT(aWindow);

  // When this is called with system priviledged, the decision should always be
  // ALLOW, and we can also stop processing this event.
  if (nsGlobalWindowOuter::Cast(aWindow)->GetPrincipal() ==
      nsContentUtils::GetSystemPrincipal()) {
    MOZ_DIAGNOSTIC_ASSERT(aDecision ==
                          ContentBlockingNotifier::BlockingDecision::eAllow);
    return;
  }

  nsAutoCString trackingOrigin;
  if (aURI) {
    Unused << nsContentUtils::GetASCIIOrigin(aURI, trackingOrigin);
  }

  if (aDecision == ContentBlockingNotifier::BlockingDecision::eBlock) {
    aWindow->NotifyContentBlockingEvent(aRejectedReason, aReportingChannel,
                                        true, trackingOrigin, aTrackingChannel);

    ReportBlockingToConsole(aWindow, aURI, aRejectedReason);
  }

  aWindow->NotifyContentBlockingEvent(
      nsIWebProgressListener::STATE_COOKIES_LOADED, aReportingChannel, false,
      trackingOrigin, aTrackingChannel);
}

}  // namespace

/* static */
void ContentBlockingNotifier::ReportUnblockingToConsole(
    nsPIDOMWindowInner* aWindow, const nsAString& aTrackingOrigin,
    ContentBlockingNotifier::StorageAccessGrantedReason aReason) {
  nsCOMPtr<nsIPrincipal> principal =
      nsGlobalWindowInner::Cast(aWindow)->GetPrincipal();
  if (NS_WARN_IF(!principal)) {
    return;
  }

  RefPtr<Document> doc = aWindow->GetExtantDoc();
  if (NS_WARN_IF(!doc)) {
    return;
  }

  nsAutoString trackingOrigin(aTrackingOrigin);

  nsAutoString sourceLine;
  uint32_t lineNumber = 0, columnNumber = 0;
  JSContext* cx = nsContentUtils::GetCurrentJSContext();
  if (cx) {
    nsJSUtils::GetCallingLocation(cx, sourceLine, &lineNumber, &columnNumber);
  }

  RefPtr<Runnable> runnable = NS_NewRunnableFunction(
      "ReportUnblockingToConsoleDelayed",
      [doc, principal, trackingOrigin, sourceLine, lineNumber, columnNumber,
       aReason]() {
        nsAutoString origin;
        nsresult rv = nsContentUtils::GetUTFOrigin(principal, origin);
        if (NS_WARN_IF(NS_FAILED(rv))) {
          return;
        }

        // Not adding grantedOrigin yet because we may not want it later.
        AutoTArray<nsString, 3> params = {origin, trackingOrigin};
        const char* messageWithSameOrigin = nullptr;

        switch (aReason) {
          case ContentBlockingNotifier::eStorageAccessAPI:
            messageWithSameOrigin = "CookieAllowedForTrackerByStorageAccessAPI";
            break;

          case ContentBlockingNotifier::eOpenerAfterUserInteraction:
            [[fallthrough]];
          case ContentBlockingNotifier::eOpener:
            messageWithSameOrigin = "CookieAllowedForTrackerByHeuristic";
            break;
        }

        nsContentUtils::ReportToConsole(
            nsIScriptError::warningFlag, ANTITRACKING_CONSOLE_CATEGORY, doc,
            nsContentUtils::eNECKO_PROPERTIES, messageWithSameOrigin, params,
            nullptr, sourceLine, lineNumber, columnNumber);
      });

  RunConsoleReportingRunnable(runnable.forget());
}

/* static */
void ContentBlockingNotifier::OnDecision(nsIChannel* aChannel,
                                         BlockingDecision aDecision,
                                         uint32_t aRejectedReason) {
  MOZ_ASSERT(
      aRejectedReason == 0 ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_BLOCKED_BY_PERMISSION ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_BLOCKED_TRACKER ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_PARTITIONED_FOREIGN ||
      aRejectedReason == nsIWebProgressListener::STATE_COOKIES_BLOCKED_ALL ||
      aRejectedReason == nsIWebProgressListener::STATE_COOKIES_BLOCKED_FOREIGN);
  MOZ_ASSERT(aDecision == BlockingDecision::eBlock ||
             aDecision == BlockingDecision::eAllow);

  if (!aChannel) {
    return;
  }

  // Can be called in EITHER the parent or child process.
  if (XRE_IsParentProcess()) {
    nsCOMPtr<nsIParentChannel> parentChannel;
    NS_QueryNotificationCallbacks(aChannel, parentChannel);
    if (parentChannel) {
      // This channel is a parent-process proxy for a child process request.
      // Tell the child process channel to do this instead.
      if (aDecision == BlockingDecision::eBlock) {
        parentChannel->NotifyCookieBlocked(aRejectedReason);
      } else {
        parentChannel->NotifyCookieAllowed();
      }
    }
    return;
  }

  MOZ_ASSERT(XRE_IsContentProcess());

  nsCOMPtr<mozIThirdPartyUtil> thirdPartyUtil = services::GetThirdPartyUtil();
  if (!thirdPartyUtil) {
    return;
  }

  nsCOMPtr<nsIURI> uriBeingLoaded =
      AntiTrackingUtils::MaybeGetDocumentURIBeingLoaded(aChannel);
  nsCOMPtr<mozIDOMWindowProxy> win;
  nsresult rv = thirdPartyUtil->GetTopWindowForChannel(aChannel, uriBeingLoaded,
                                                       getter_AddRefs(win));
  NS_ENSURE_SUCCESS_VOID(rv);

  nsCOMPtr<nsPIDOMWindowOuter> pwin = nsPIDOMWindowOuter::From(win);
  if (!pwin) {
    return;
  }

  nsCOMPtr<nsIURI> uri;
  aChannel->GetURI(getter_AddRefs(uri));

  NotifyBlockingDecision(aChannel, aChannel, aDecision, aRejectedReason,
                         uri, pwin);
}

/* static */
void ContentBlockingNotifier::OnDecision(nsPIDOMWindowInner* aWindow,
                                         BlockingDecision aDecision,
                                         uint32_t aRejectedReason) {
  MOZ_ASSERT(aWindow);
  MOZ_ASSERT(
      aRejectedReason == 0 ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_BLOCKED_BY_PERMISSION ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_BLOCKED_TRACKER ||
      aRejectedReason ==
          nsIWebProgressListener::STATE_COOKIES_PARTITIONED_FOREIGN ||
      aRejectedReason == nsIWebProgressListener::STATE_COOKIES_BLOCKED_ALL ||
      aRejectedReason == nsIWebProgressListener::STATE_COOKIES_BLOCKED_FOREIGN);
  MOZ_ASSERT(aDecision == BlockingDecision::eBlock ||
             aDecision == BlockingDecision::eAllow);

  nsCOMPtr<nsPIDOMWindowOuter> pwin = AntiTrackingUtils::GetTopWindow(aWindow);
  if (!pwin) {
    return;
  }

  nsPIDOMWindowInner* inner = pwin->GetCurrentInnerWindow();
  if (!inner) {
    return;
  }
  Document* pwinDoc = inner->GetExtantDoc();
  if (!pwinDoc) {
    return;
  }
  nsIChannel* channel = pwinDoc->GetChannel();
  if (!channel) {
    return;
  }

  Document* document = aWindow->GetExtantDoc();
  if (!document) {
    return;
  }
  nsIURI* uri = document->GetDocumentURI();
  nsIChannel* trackingChannel = document->GetChannel();

  NotifyBlockingDecision(channel, trackingChannel, aDecision, aRejectedReason,
                         uri, pwin);
}
