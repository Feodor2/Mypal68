/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AntiTrackingLog.h"
#include "ContentBlockingLog.h"

namespace mozilla {

Maybe<uint32_t> ContentBlockingLog::RecordLogParent(
    const nsACString& aOrigin, uint32_t aType, bool aBlocked,
    const Maybe<ContentBlockingNotifier::StorageAccessPermissionGrantedReason>&
        aReason,
    const nsTArray<nsCString>& aTrackingFullHashes) {
  MOZ_ASSERT(XRE_IsParentProcess());

  uint32_t events = GetContentBlockingEventsInLog();

  bool blockedValue = aBlocked;
  bool unblocked = false;

  switch (aType) {
    case nsIWebProgressListener::STATE_COOKIES_LOADED:
      MOZ_ASSERT(!aBlocked,
                 "We don't expected to see blocked STATE_COOKIES_LOADED");
      [[fallthrough]];

    case nsIWebProgressListener::STATE_BLOCKED_TRACKING_CONTENT:
    case nsIWebProgressListener::STATE_COOKIES_BLOCKED_BY_PERMISSION:
    case nsIWebProgressListener::STATE_COOKIES_BLOCKED_ALL:
    case nsIWebProgressListener::STATE_COOKIES_BLOCKED_FOREIGN:
      RecordLogInternal(aOrigin, aType, blockedValue);
      break;

    default:
      // Ignore nsIWebProgressListener::STATE_BLOCKED_UNSAFE_CONTENT;
      break;
  }

  if (!aBlocked) {
    unblocked = (events & aType) != 0;
  }

  const uint32_t oldEvents = events;
  if (blockedValue) {
    events |= aType;
  } else if (unblocked) {
    events &= ~aType;
  }

  if (events == oldEvents
#ifdef ANDROID
      // GeckoView always needs to notify about blocked trackers,
      // since the GeckoView API always needs to report the URI and
      // type of any blocked tracker. We use a platform-dependent code
      // path here because reporting this notification on desktop
      // platforms isn't necessary and doing so can have a big
      // performance cost.
      && aType != nsIWebProgressListener::STATE_BLOCKED_TRACKING_CONTENT
#endif
  ) {
    // Avoid dispatching repeated notifications when nothing has
    // changed
    return Nothing();
  }

  return Some(events);
}

}  // namespace mozilla
