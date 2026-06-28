/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/OriginAttributes.h"
#include "mozilla/Preferences.h"
#include "mozilla/dom/BlobURLProtocolHandler.h"
#include "mozilla/dom/quota/QuotaManager.h"
#include "nsIEffectiveTLDService.h"
#include "nsIURI.h"
#include "nsURLHelper.h"

namespace mozilla {

void MakeFirstPartyDomain(const nsACString& aScheme, const nsACString& aHost,
                          int32_t aPort, nsAString& aFirstPartyDomain) {
  if (!OriginAttributes::UseSiteForFirstPartyDomain()) {
    aFirstPartyDomain.Assign(NS_ConvertUTF8toUTF16(aHost));
    return;
  }

  nsAutoCString site;
  site.AssignLiteral("(");
  site.Append(aScheme);
  site.Append(",");
  site.Append(aHost);
  if (aPort != -1) {
    site.Append(",");
    site.AppendInt(aPort);
  }
  site.AppendLiteral(")");

  aFirstPartyDomain.Assign(NS_ConvertUTF8toUTF16(site));
}

void MakeFirstPartyDomain(const nsACString& aScheme, const nsACString& aHost,
                          nsAString& aFirstPartyDomain) {
  MakeFirstPartyDomain(aScheme, aHost, -1, aFirstPartyDomain);
}

void OriginAttributes::SetFirstPartyDomain(const bool aIsTopLevelDocument,
                                           nsIURI* aURI, bool aForced) {
  nsresult rv;

  if (!aURI) {
    return;
  }

  // If the prefs are off or this is not a top level load, bail out.
  if ((!IsFirstPartyEnabled() || !aIsTopLevelDocument) && !aForced) {
    return;
  }

  nsAutoCString scheme;
  rv = aURI->GetScheme(scheme);
  NS_ENSURE_SUCCESS_VOID(rv);

  if (scheme.EqualsLiteral("about")) {
    MakeFirstPartyDomain(scheme,
                         NS_LITERAL_CSTRING(ABOUT_URI_FIRST_PARTY_DOMAIN),
                         mFirstPartyDomain);
    return;
  }

  // Add-on principals should never get any first-party domain
  // attributes in order to guarantee their storage integrity when switching
  // FPI on and off.
  if (scheme.EqualsLiteral("moz-extension")) {
    return;
  }

  nsCOMPtr<nsIPrincipal> blobPrincipal;
  if (dom::BlobURLProtocolHandler::GetBlobURLPrincipal(
          aURI, getter_AddRefs(blobPrincipal))) {
    MOZ_ASSERT(blobPrincipal);
    mFirstPartyDomain = blobPrincipal->OriginAttributesRef().mFirstPartyDomain;
    return;
  }

  nsCOMPtr<nsIEffectiveTLDService> tldService =
      do_GetService(NS_EFFECTIVETLDSERVICE_CONTRACTID);
  MOZ_ASSERT(tldService);
  NS_ENSURE_TRUE_VOID(tldService);

  nsAutoCString baseDomain;
  rv = tldService->GetBaseDomain(aURI, 0, baseDomain);
  if (NS_SUCCEEDED(rv)) {
    MakeFirstPartyDomain(scheme, baseDomain, mFirstPartyDomain);
    return;
  }

  // Saving before rv is overwritten.
  bool isIpAddress = (rv == NS_ERROR_HOST_IS_IP_ADDRESS);
  bool isInsufficientDomainLevels = (rv == NS_ERROR_INSUFFICIENT_DOMAIN_LEVELS);

  int32_t port;
  rv = aURI->GetPort(&port);
  NS_ENSURE_SUCCESS_VOID(rv);

  nsAutoCString host;
  rv = aURI->GetHost(host);
  NS_ENSURE_SUCCESS_VOID(rv);

  if (isIpAddress) {
    // If the host is an IPv4/IPv6 address, we still accept it as a
    // valid firstPartyDomain.
    nsAutoCString ipAddr;

    if (net_IsValidIPv6Addr(host)) {
      // According to RFC2732, the host of an IPv6 address should be an
      // IPv6reference. The GetHost() of nsIURI will only return the IPv6
      // address. So, we need to convert it back to IPv6reference here.
      ipAddr.AssignLiteral("[");
      ipAddr.Append(host);
      ipAddr.AppendLiteral("]");
    } else {
      ipAddr = host;
    }

    MakeFirstPartyDomain(scheme, ipAddr, port, mFirstPartyDomain);
    return;
  }

  if (OriginAttributes::UseSiteForFirstPartyDomain()) {
    MakeFirstPartyDomain(scheme, host, port, mFirstPartyDomain);
    return;
  }

  if (isInsufficientDomainLevels) {
    nsAutoCString publicSuffix;
    rv = tldService->GetPublicSuffix(aURI, publicSuffix);
    if (NS_SUCCEEDED(rv)) {
      MakeFirstPartyDomain(scheme, publicSuffix, port, mFirstPartyDomain);
      return;
    }
  }
}

void OriginAttributes::SetFirstPartyDomain(const bool aIsTopLevelDocument,
                                           const nsACString& aDomain) {
  bool isFirstPartyEnabled = IsFirstPartyEnabled();

  // If the pref is off or this is not a top level load, bail out.
  if (!isFirstPartyEnabled || !aIsTopLevelDocument) {
    return;
  }

  mFirstPartyDomain = NS_ConvertUTF8toUTF16(aDomain);
}

void OriginAttributes::CreateSuffix(nsACString& aStr) const {
  URLParams params;
  nsAutoString value;

  //
  // Important: While serializing any string-valued attributes, perform a
  // release-mode assertion to make sure that they don't contain characters that
  // will break the quota manager when it uses the serialization for file
  // naming.
  //

  if (mInIsolatedMozBrowser) {
    params.Set(NS_LITERAL_STRING("inBrowser"), NS_LITERAL_STRING("1"));
  }

  if (mUserContextId != nsIScriptSecurityManager::DEFAULT_USER_CONTEXT_ID) {
    value.Truncate();
    value.AppendInt(mUserContextId);
    params.Set(NS_LITERAL_STRING("userContextId"), value);
  }

  if (mPrivateBrowsingId) {
    value.Truncate();
    value.AppendInt(mPrivateBrowsingId);
    params.Set(NS_LITERAL_STRING("privateBrowsingId"), value);
  }

  if (!mFirstPartyDomain.IsEmpty()) {
    nsAutoString sanitizedFirstPartyDomain(mFirstPartyDomain);
    sanitizedFirstPartyDomain.ReplaceChar(
        dom::quota::QuotaManager::kReplaceChars, '+');

    params.Set(NS_LITERAL_STRING("firstPartyDomain"),
               sanitizedFirstPartyDomain);
  }

  aStr.Truncate();

  params.Serialize(value);
  if (!value.IsEmpty()) {
    aStr.AppendLiteral("^");
    aStr.Append(NS_ConvertUTF16toUTF8(value));
  }

// In debug builds, check the whole string for illegal characters too (just in
// case).
#ifdef DEBUG
  nsAutoCString str;
  str.Assign(aStr);
  MOZ_ASSERT(str.FindCharInSet(dom::quota::QuotaManager::kReplaceChars) ==
             kNotFound);
#endif
}

already_AddRefed<nsAtom> OriginAttributes::CreateSuffixAtom() const {
  nsAutoCString suffix;
  CreateSuffix(suffix);
  return NS_Atomize(suffix);
}

void OriginAttributes::CreateAnonymizedSuffix(nsACString& aStr) const {
  OriginAttributes attrs = *this;

  if (!attrs.mFirstPartyDomain.IsEmpty()) {
    attrs.mFirstPartyDomain.AssignLiteral("_anonymizedFirstPartyDomain_");
  }

  attrs.CreateSuffix(aStr);
}

bool OriginAttributes::PopulateFromSuffix(const nsACString& aStr) {
  if (aStr.IsEmpty()) {
    return true;
  }

  if (aStr[0] != '^') {
    return false;
  }

  // If mPrivateBrowsingId is passed in as >0 and is not present in the
  // suffix, then it will remain >0 when it should be 0 according to the
  // suffix. Set to 0 before iterating to fix this.
  mPrivateBrowsingId = 0;

  return URLParams::Parse(
      Substring(aStr, 1, aStr.Length() - 1),
      [this](const nsAString& aName, const nsAString& aValue) {
        if (aName.EqualsLiteral("inBrowser")) {
          if (!aValue.EqualsLiteral("1")) {
            return false;
          }

          mInIsolatedMozBrowser = true;
          return true;
        }

        if (aName.EqualsLiteral("addonId") || aName.EqualsLiteral("appId")) {
          // No longer supported. Silently ignore so that legacy origin strings
          // don't cause failures.
          return true;
        }

        if (aName.EqualsLiteral("userContextId")) {
          nsresult rv;
          int64_t val = aValue.ToInteger64(&rv);
          NS_ENSURE_SUCCESS(rv, false);
          NS_ENSURE_TRUE(val <= UINT32_MAX, false);
          mUserContextId = static_cast<uint32_t>(val);

          return true;
        }

        if (aName.EqualsLiteral("privateBrowsingId")) {
          nsresult rv;
          int64_t val = aValue.ToInteger64(&rv);
          NS_ENSURE_SUCCESS(rv, false);
          NS_ENSURE_TRUE(val >= 0 && val <= UINT32_MAX, false);
          mPrivateBrowsingId = static_cast<uint32_t>(val);

          return true;
        }

        if (aName.EqualsLiteral("firstPartyDomain")) {
          MOZ_RELEASE_ASSERT(mFirstPartyDomain.IsEmpty());
          mFirstPartyDomain.Assign(aValue);
          return true;
        }

        // No other attributes are supported.
        return false;
      });
}

bool OriginAttributes::PopulateFromOrigin(const nsACString& aOrigin,
                                          nsACString& aOriginNoSuffix) {
  // RFindChar is only available on nsCString.
  nsCString origin(aOrigin);
  int32_t pos = origin.RFindChar('^');

  if (pos == kNotFound) {
    aOriginNoSuffix = origin;
    return true;
  }

  aOriginNoSuffix = Substring(origin, 0, pos);
  return PopulateFromSuffix(Substring(origin, pos));
}

void OriginAttributes::SyncAttributesWithPrivateBrowsing(
    bool aInPrivateBrowsing) {
  mPrivateBrowsingId = aInPrivateBrowsing ? 1 : 0;
}

/* static */
bool OriginAttributes::IsPrivateBrowsing(const nsACString& aOrigin) {
  nsAutoCString dummy;
  OriginAttributes attrs;
  if (NS_WARN_IF(!attrs.PopulateFromOrigin(aOrigin, dummy))) {
    return false;
  }

  return !!attrs.mPrivateBrowsingId;
}

}  // namespace mozilla
