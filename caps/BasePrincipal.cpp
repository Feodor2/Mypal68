/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/BasePrincipal.h"

#include "nsDocShell.h"

#include "ExpandedPrincipal.h"
#include "nsNetUtil.h"
#include "nsIURIWithSpecialOrigin.h"
#include "nsScriptSecurityManager.h"
#include "nsServiceManagerUtils.h"
#include "nsAboutProtocolUtils.h"
#include "ThirdPartyUtil.h"
#include "mozilla/ContentPrincipal.h"
#include "mozilla/NullPrincipal.h"
#include "mozilla/dom/BlobURLProtocolHandler.h"
#include "mozilla/dom/ChromeUtils.h"
#include "mozilla/dom/ToJSValue.h"

namespace mozilla {

BasePrincipal::BasePrincipal(PrincipalKind aKind)
    : mKind(aKind), mHasExplicitDomain(false), mInitialized(false) {}

BasePrincipal::~BasePrincipal() {}

NS_IMETHODIMP
BasePrincipal::GetOrigin(nsACString& aOrigin) {
  MOZ_ASSERT(mInitialized);

  nsresult rv = GetOriginNoSuffix(aOrigin);
  NS_ENSURE_SUCCESS(rv, rv);

  nsAutoCString suffix;
  rv = GetOriginSuffix(suffix);
  NS_ENSURE_SUCCESS(rv, rv);
  aOrigin.Append(suffix);
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetOriginNoSuffix(nsACString& aOrigin) {
  MOZ_ASSERT(mInitialized);
  mOriginNoSuffix->ToUTF8String(aOrigin);
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetSiteOrigin(nsACString& aSiteOrigin) {
  MOZ_ASSERT(mInitialized);
  return GetOrigin(aSiteOrigin);
}

bool BasePrincipal::Subsumes(nsIPrincipal* aOther,
                             DocumentDomainConsideration aConsideration) {
  MOZ_ASSERT(aOther);
  MOZ_ASSERT_IF(Kind() == eCodebasePrincipal, mOriginSuffix);

  // Expanded principals handle origin attributes for each of their
  // sub-principals individually, null principals do only simple checks for
  // pointer equality, and system principals are immune to origin attributes
  // checks, so only do this check for codebase principals.
  if (Kind() == eCodebasePrincipal &&
      mOriginSuffix != Cast(aOther)->mOriginSuffix) {
    return false;
  }

  return SubsumesInternal(aOther, aConsideration);
}

NS_IMETHODIMP
BasePrincipal::Equals(nsIPrincipal* aOther, bool* aResult) {
  NS_ENSURE_ARG_POINTER(aOther);

  *aResult = FastEquals(aOther);

  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::EqualsConsideringDomain(nsIPrincipal* aOther, bool* aResult) {
  NS_ENSURE_ARG_POINTER(aOther);

  *aResult = FastEqualsConsideringDomain(aOther);

  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::Subsumes(nsIPrincipal* aOther, bool* aResult) {
  NS_ENSURE_ARG_POINTER(aOther);

  *aResult = FastSubsumes(aOther);

  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::SubsumesConsideringDomain(nsIPrincipal* aOther, bool* aResult) {
  NS_ENSURE_ARG_POINTER(aOther);

  *aResult = FastSubsumesConsideringDomain(aOther);

  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::SubsumesConsideringDomainIgnoringFPD(nsIPrincipal* aOther,
                                                    bool* aResult) {
  NS_ENSURE_ARG_POINTER(aOther);

  *aResult = FastSubsumesConsideringDomainIgnoringFPD(aOther);

  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::CheckMayLoad(nsIURI* aURI, bool aReport,
                            bool aAllowIfInheritsPrincipal) {
  NS_ENSURE_ARG_POINTER(aURI);

  // Check the internal method first, which allows us to quickly approve loads
  // for the System Principal.
  if (MayLoadInternal(aURI)) {
    return NS_OK;
  }

  nsresult rv;
  if (aAllowIfInheritsPrincipal) {
    // If the caller specified to allow loads of URIs that inherit
    // our principal, allow the load if this URI inherits its principal.
    bool doesInheritSecurityContext;
    rv = NS_URIChainHasFlags(aURI,
                             nsIProtocolHandler::URI_INHERITS_SECURITY_CONTEXT,
                             &doesInheritSecurityContext);
    if (NS_SUCCEEDED(rv) && doesInheritSecurityContext) {
      return NS_OK;
    }
  }

  bool fetchableByAnyone;
  rv = NS_URIChainHasFlags(aURI, nsIProtocolHandler::URI_FETCHABLE_BY_ANYONE,
                           &fetchableByAnyone);
  if (NS_SUCCEEDED(rv) && fetchableByAnyone) {
    return NS_OK;
  }

  if (aReport) {
    nsCOMPtr<nsIURI> prinURI;
    rv = GetURI(getter_AddRefs(prinURI));
    if (NS_SUCCEEDED(rv) && prinURI) {
      nsScriptSecurityManager::ReportError(
          "CheckSameOriginError", prinURI, aURI,
          mOriginAttributes.mPrivateBrowsingId > 0);
    }
  }

  return NS_ERROR_DOM_BAD_URI;
}

NS_IMETHODIMP
BasePrincipal::IsThirdPartyURI(nsIURI* aURI, bool* aRes) {
  *aRes = true;
  // If we do not have a URI its always 3rd party.
  nsCOMPtr<nsIURI> prinURI;
  nsresult rv = GetURI(getter_AddRefs(prinURI));
  if (NS_FAILED(rv) || !prinURI) {
    return NS_OK;
  }
  ThirdPartyUtil* thirdPartyUtil = ThirdPartyUtil::GetInstance();
  return thirdPartyUtil->IsThirdPartyURI(prinURI, aURI, aRes);
}

NS_IMETHODIMP
BasePrincipal::IsThirdPartyPrincipal(nsIPrincipal* aPrin, bool* aRes) {
  *aRes = true;
  nsCOMPtr<nsIURI> prinURI;
  nsresult rv = GetURI(getter_AddRefs(prinURI));
  if (NS_FAILED(rv) || !prinURI) {
    return NS_OK;
  }
  return aPrin->IsThirdPartyURI(prinURI, aRes);
}

NS_IMETHODIMP
BasePrincipal::GetIsNullPrincipal(bool* aResult) {
  *aResult = Kind() == eNullPrincipal;
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetIsCodebasePrincipal(bool* aResult) {
  *aResult = Kind() == eCodebasePrincipal;
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetIsExpandedPrincipal(bool* aResult) {
  *aResult = Kind() == eExpandedPrincipal;
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetIsSystemPrincipal(bool* aResult) {
  *aResult = IsSystemPrincipal();
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetIsAddonOrExpandedAddonPrincipal(bool* aResult) {
  *aResult = AddonPolicy() || ContentScriptAddonPolicy();
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::SchemeIs(const char* aScheme, bool* aResult) {
  *aResult = false;
  nsCOMPtr<nsIURI> prinURI;
  nsresult rv = GetURI(getter_AddRefs(prinURI));
  if (NS_FAILED(rv) || !prinURI) {
    return NS_OK;
  }
  *aResult = prinURI->SchemeIs(aScheme);
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetAboutModuleFlags(uint32_t* flags) {
  *flags = 0;
  nsCOMPtr<nsIURI> prinURI;
  nsresult rv = GetURI(getter_AddRefs(prinURI));
  if (NS_FAILED(rv) || !prinURI) {
    return NS_ERROR_NOT_AVAILABLE;
  }
  if (!prinURI->SchemeIs("about")) {
    return NS_OK;
  }

  nsCOMPtr<nsIAboutModule> aboutModule;
  rv = NS_GetAboutModule(prinURI, getter_AddRefs(aboutModule));
  if (NS_FAILED(rv) || !aboutModule) {
    return rv;
  }
  return aboutModule->GetURIFlags(prinURI, flags);
}

NS_IMETHODIMP
BasePrincipal::GetOriginAttributes(JSContext* aCx,
                                   JS::MutableHandle<JS::Value> aVal) {
  if (NS_WARN_IF(!ToJSValue(aCx, mOriginAttributes, aVal))) {
    return NS_ERROR_FAILURE;
  }
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetOriginSuffix(nsACString& aOriginAttributes) {
  MOZ_ASSERT(mOriginSuffix);
  mOriginSuffix->ToUTF8String(aOriginAttributes);
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetUserContextId(uint32_t* aUserContextId) {
  *aUserContextId = UserContextId();
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetPrivateBrowsingId(uint32_t* aPrivateBrowsingId) {
  *aPrivateBrowsingId = PrivateBrowsingId();
  return NS_OK;
}

NS_IMETHODIMP
BasePrincipal::GetIsInIsolatedMozBrowserElement(
    bool* aIsInIsolatedMozBrowserElement) {
  *aIsInIsolatedMozBrowserElement = IsInIsolatedMozBrowserElement();
  return NS_OK;
}

nsresult BasePrincipal::GetAddonPolicy(nsISupports** aResult) {
  RefPtr<extensions::WebExtensionPolicy> policy(AddonPolicy());
  policy.forget(aResult);
  return NS_OK;
}

extensions::WebExtensionPolicy* BasePrincipal::AddonPolicy() {
  if (Is<ContentPrincipal>()) {
    return As<ContentPrincipal>()->AddonPolicy();
  }
  return nullptr;
}

bool BasePrincipal::AddonHasPermission(const nsAtom* aPerm) {
  if (auto policy = AddonPolicy()) {
    return policy->HasPermission(aPerm);
  }
  return false;
}

nsIPrincipal* BasePrincipal::PrincipalToInherit(nsIURI* aRequestedURI) {
  if (Is<ExpandedPrincipal>()) {
    return As<ExpandedPrincipal>()->PrincipalToInherit(aRequestedURI);
  }
  return this;
}

already_AddRefed<BasePrincipal> BasePrincipal::CreateCodebasePrincipal(
    nsIURI* aURI, const OriginAttributes& aAttrs) {
  MOZ_ASSERT(aURI);

  nsAutoCString originNoSuffix;
  nsresult rv =
      ContentPrincipal::GenerateOriginNoSuffixFromURI(aURI, originNoSuffix);
  if (NS_FAILED(rv)) {
    // If the generation of the origin fails, we still want to have a valid
    // principal. Better to return a null principal here.
    return NullPrincipal::Create(aAttrs);
  }

  return CreateCodebasePrincipal(aURI, aAttrs, originNoSuffix);
}

already_AddRefed<BasePrincipal> BasePrincipal::CreateCodebasePrincipal(
    nsIURI* aURI, const OriginAttributes& aAttrs,
    const nsACString& aOriginNoSuffix) {
  MOZ_ASSERT(aURI);
  MOZ_ASSERT(!aOriginNoSuffix.IsEmpty());

  // If the URI is supposed to inherit the security context of whoever loads it,
  // we shouldn't make a codebase principal for it.
  bool inheritsPrincipal;
  nsresult rv = NS_URIChainHasFlags(
      aURI, nsIProtocolHandler::URI_INHERITS_SECURITY_CONTEXT,
      &inheritsPrincipal);
  if (NS_FAILED(rv) || inheritsPrincipal) {
    return NullPrincipal::Create(aAttrs);
  }

  // Check whether the URI knows what its principal is supposed to be.
#if defined(MOZ_THUNDERBIRD) || defined(MOZ_SUITE)
  nsCOMPtr<nsIURIWithSpecialOrigin> uriWithSpecialOrigin =
      do_QueryInterface(aURI);
  if (uriWithSpecialOrigin) {
    nsCOMPtr<nsIURI> origin;
    rv = uriWithSpecialOrigin->GetOrigin(getter_AddRefs(origin));
    if (NS_WARN_IF(NS_FAILED(rv))) {
      return nullptr;
    }
    MOZ_ASSERT(origin);
    OriginAttributes attrs;
    RefPtr<BasePrincipal> principal = CreateCodebasePrincipal(origin, attrs);
    return principal.forget();
  }
#endif

  nsCOMPtr<nsIPrincipal> blobPrincipal;
  if (dom::BlobURLProtocolHandler::GetBlobURLPrincipal(
          aURI, getter_AddRefs(blobPrincipal))) {
    MOZ_ASSERT(blobPrincipal);
    RefPtr<BasePrincipal> principal = Cast(blobPrincipal);
    return principal.forget();
  }

  // Mint a codebase principal.
  RefPtr<ContentPrincipal> codebase = new ContentPrincipal();
  rv = codebase->Init(aURI, aAttrs, aOriginNoSuffix);
  NS_ENSURE_SUCCESS(rv, nullptr);
  return codebase.forget();
}

already_AddRefed<BasePrincipal> BasePrincipal::CreateCodebasePrincipal(
    const nsACString& aOrigin) {
  MOZ_ASSERT(!StringBeginsWith(aOrigin, NS_LITERAL_CSTRING("[")),
             "CreateCodebasePrincipal does not support System and Expanded "
             "principals");

  MOZ_ASSERT(!StringBeginsWith(aOrigin,
                               NS_LITERAL_CSTRING(NS_NULLPRINCIPAL_SCHEME ":")),
             "CreateCodebasePrincipal does not support NullPrincipal");

  nsAutoCString originNoSuffix;
  OriginAttributes attrs;
  if (!attrs.PopulateFromOrigin(aOrigin, originNoSuffix)) {
    return nullptr;
  }

  nsCOMPtr<nsIURI> uri;
  nsresult rv = NS_NewURI(getter_AddRefs(uri), originNoSuffix);
  NS_ENSURE_SUCCESS(rv, nullptr);

  return BasePrincipal::CreateCodebasePrincipal(uri, attrs);
}

already_AddRefed<BasePrincipal> BasePrincipal::CloneForcingOriginAttributes(
    const OriginAttributes& aOriginAttributes) {
  if (NS_WARN_IF(!IsCodebasePrincipal())) {
    return nullptr;
  }

  nsAutoCString originNoSuffix;
  nsresult rv = GetOriginNoSuffix(originNoSuffix);
  NS_ENSURE_SUCCESS(rv, nullptr);

  nsIURI* uri = static_cast<ContentPrincipal*>(this)->mCodebase;
  RefPtr<ContentPrincipal> copy = new ContentPrincipal();
  rv = copy->Init(uri, aOriginAttributes, originNoSuffix);
  NS_ENSURE_SUCCESS(rv, nullptr);

  return copy.forget();
}

extensions::WebExtensionPolicy* BasePrincipal::ContentScriptAddonPolicy() {
  if (!Is<ExpandedPrincipal>()) {
    return nullptr;
  }

  auto expanded = As<ExpandedPrincipal>();
  for (auto& prin : expanded->AllowList()) {
    if (auto policy = BasePrincipal::Cast(prin)->AddonPolicy()) {
      return policy;
    }
  }

  return nullptr;
}

bool BasePrincipal::AddonAllowsLoad(nsIURI* aURI,
                                    bool aExplicit /* = false */) {
  if (Is<ExpandedPrincipal>()) {
    return As<ExpandedPrincipal>()->AddonAllowsLoad(aURI, aExplicit);
  }
  if (auto policy = AddonPolicy()) {
    return policy->CanAccessURI(aURI, aExplicit);
  }
  return false;
}

void BasePrincipal::FinishInit(const nsACString& aOriginNoSuffix,
                               const OriginAttributes& aOriginAttributes) {
  mInitialized = true;
  mOriginAttributes = aOriginAttributes;

  // First compute the origin suffix since it's infallible.
  nsAutoCString originSuffix;
  mOriginAttributes.CreateSuffix(originSuffix);
  mOriginSuffix = NS_Atomize(originSuffix);

  MOZ_ASSERT(!aOriginNoSuffix.IsEmpty());
  mOriginNoSuffix = NS_Atomize(aOriginNoSuffix);
}

void BasePrincipal::FinishInit(BasePrincipal* aOther,
                               const OriginAttributes& aOriginAttributes) {
  mInitialized = true;
  mOriginAttributes = aOriginAttributes;

  // First compute the origin suffix since it's infallible.
  nsAutoCString originSuffix;
  mOriginAttributes.CreateSuffix(originSuffix);
  mOriginSuffix = NS_Atomize(originSuffix);

  mOriginNoSuffix = aOther->mOriginNoSuffix;
  mHasExplicitDomain = aOther->mHasExplicitDomain;
}

bool SiteIdentifier::Equals(const SiteIdentifier& aOther) const {
  MOZ_ASSERT(IsInitialized());
  MOZ_ASSERT(aOther.IsInitialized());
  return mPrincipal->FastEquals(aOther.mPrincipal);
}

}  // namespace mozilla
