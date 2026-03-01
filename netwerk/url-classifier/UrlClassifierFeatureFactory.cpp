/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/net/UrlClassifierFeatureFactory.h"

// List of Features
#include "UrlClassifierFeatureTrackingProtection.h"
#include "UrlClassifierFeatureTrackingAnnotation.h"
#include "UrlClassifierFeatureCustomTables.h"

#include "nsAppRunner.h"

namespace mozilla {
namespace net {

/* static */
void UrlClassifierFeatureFactory::Shutdown() {
  // We want to expose Features only in the parent process.
  if (!XRE_IsParentProcess()) {
    return;
  }

  UrlClassifierFeatureTrackingAnnotation::MaybeShutdown();
  UrlClassifierFeatureTrackingProtection::MaybeShutdown();
}

/* static */
void UrlClassifierFeatureFactory::GetFeaturesFromChannel(
    nsIChannel* aChannel,
    nsTArray<nsCOMPtr<nsIUrlClassifierFeature>>& aFeatures) {
  MOZ_ASSERT(XRE_IsParentProcess());
  MOZ_ASSERT(aChannel);

  nsCOMPtr<nsIUrlClassifierFeature> feature;

  // Note that the order of the features is extremely important! When more than
  // 1 feature classifies the channel, we call ::ProcessChannel() following this
  // feature order, and this could produce different results with a different
  // feature ordering.

  // Tracking Protection
  feature = UrlClassifierFeatureTrackingProtection::MaybeCreate(aChannel);
  if (feature) {
    aFeatures.AppendElement(feature);
  }

  // Tracking Annotation
  feature = UrlClassifierFeatureTrackingAnnotation::MaybeCreate(aChannel);
  if (feature) {
    aFeatures.AppendElement(feature);
  }
}

/* static */
already_AddRefed<nsIUrlClassifierFeature>
UrlClassifierFeatureFactory::GetFeatureByName(const nsACString& aName) {
  if (!XRE_IsParentProcess()) {
    return nullptr;
  }

  nsCOMPtr<nsIUrlClassifierFeature> feature;

  // Tracking Protection
  feature = UrlClassifierFeatureTrackingProtection::GetIfNameMatches(aName);
  if (feature) {
    return feature.forget();
  }

  // Tracking Annotation
  feature = UrlClassifierFeatureTrackingAnnotation::GetIfNameMatches(aName);
  if (feature) {
    return feature.forget();
  }

  return nullptr;
}

/* static */
void UrlClassifierFeatureFactory::GetFeatureNames(nsTArray<nsCString>& aArray) {
  if (!XRE_IsParentProcess()) {
    return;
  }

  nsAutoCString name;

  // Tracking Protection
  name.Assign(UrlClassifierFeatureTrackingProtection::Name());
  if (!name.IsEmpty()) {
    aArray.AppendElement(name);
  }

  // Tracking Annotation
  name.Assign(UrlClassifierFeatureTrackingAnnotation::Name());
  if (!name.IsEmpty()) {
    aArray.AppendElement(name);
  }
}

/* static */
already_AddRefed<nsIUrlClassifierFeature>
UrlClassifierFeatureFactory::CreateFeatureWithTables(
    const nsACString& aName, const nsTArray<nsCString>& aBlacklistTables,
    const nsTArray<nsCString>& aWhitelistTables) {
  nsCOMPtr<nsIUrlClassifierFeature> feature =
      new UrlClassifierFeatureCustomTables(aName, aBlacklistTables,
                                           aWhitelistTables);
  return feature.forget();
}

namespace {

struct BlockingErrorCode {
  nsresult mErrorCode;
  uint32_t mBlockingEventCode;
  const char* mConsoleMessage;
  nsCString mConsoleCategory;
};

static const BlockingErrorCode sBlockingErrorCodes[] = {
    {NS_ERROR_TRACKING_URI,
     nsIWebProgressListener::STATE_BLOCKED_TRACKING_CONTENT,
     "TrackerUriBlocked", "Tracking Protection"_ns},
};

}  // namespace

/* static */
bool UrlClassifierFeatureFactory::IsClassifierBlockingErrorCode(
    nsresult aError) {
  // In theory we can iterate through the features, but at the moment, we can
  // just have a simple check here.
  for (const auto& blockingErrorCode : sBlockingErrorCodes) {
    if (aError == blockingErrorCode.mErrorCode) {
      return true;
    }
  }

  return false;
}

/* static */
bool UrlClassifierFeatureFactory::IsClassifierBlockingEventCode(
    uint32_t aEventCode) {
  for (const auto& blockingErrorCode : sBlockingErrorCodes) {
    if (aEventCode == blockingErrorCode.mBlockingEventCode) {
      return true;
    }
  }
  return false;
}

/* static */
uint32_t UrlClassifierFeatureFactory::GetClassifierBlockingEventCode(
    nsresult aErrorCode) {
  for (const auto& blockingErrorCode : sBlockingErrorCodes) {
    if (aErrorCode == blockingErrorCode.mErrorCode) {
      return blockingErrorCode.mBlockingEventCode;
    }
  }
  return 0;
}

/* static */ const char*
UrlClassifierFeatureFactory::ClassifierBlockingErrorCodeToConsoleMessage(
    nsresult aError, nsACString& aCategory) {
  for (const auto& blockingErrorCode : sBlockingErrorCodes) {
    if (aError == blockingErrorCode.mErrorCode) {
      aCategory = blockingErrorCode.mConsoleCategory;
      return blockingErrorCode.mConsoleMessage;
    }
  }

  return nullptr;
}

}  // namespace net
}  // namespace mozilla
