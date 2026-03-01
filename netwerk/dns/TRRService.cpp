/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsICaptivePortalService.h"
#include "nsINetworkLinkService.h"
#include "nsIObserverService.h"
#include "nsNetUtil.h"
#include "nsStandardURL.h"
#include "TRR.h"
#include "TRRService.h"

#include "mozilla/Preferences.h"
#include "mozilla/StaticPrefs_network.h"
#include "mozilla/Tokenizer.h"

static const char kOpenCaptivePortalLoginEvent[] = "captive-portal-login";
static const char kClearPrivateData[] = "clear-private-data";
static const char kPurge[] = "browser:purge-session-history";
static const char kDisableIpv6Pref[] = "network.dns.disableIPv6";
static const char kCaptivedetectCanonicalURL[] = "captivedetect.canonicalURL";

#define TRR_PREF_PREFIX "network.trr."
#define TRR_PREF(x) TRR_PREF_PREFIX x

namespace mozilla {
namespace net {

#undef LOG
extern mozilla::LazyLogModule gHostResolverLog;
#define LOG(args) MOZ_LOG(gHostResolverLog, mozilla::LogLevel::Debug, args)

TRRService* gTRRService = nullptr;
StaticRefPtr<nsIThread> sTRRBackgroundThread;

NS_IMPL_ISUPPORTS(TRRService, nsIObserver, nsISupportsWeakReference)

TRRService::TRRService()
    : mInitialized(false),
      mMode(0),
      mTRRBlocklistExpireTime(72 * 3600),
      mLock("trrservice"),
      mConfirmationNS("example.com"_ns),
      mCaptiveIsPassed(false),
      mVPNDetected(false),
      mClearTRRBLStorage(false),
      mConfirmationState(CONFIRM_INIT),
      mRetryConfirmInterval(1000),
      mTRRFailures(0) {
  MOZ_ASSERT(NS_IsMainThread(), "wrong thread");
}

nsresult TRRService::Init() {
  MOZ_ASSERT(NS_IsMainThread(), "wrong thread");
  if (mInitialized) {
    return NS_OK;
  }
  mInitialized = true;

  nsCOMPtr<nsIObserverService> observerService =
      mozilla::services::GetObserverService();
  if (observerService) {
    observerService->AddObserver(this, NS_CAPTIVE_PORTAL_CONNECTIVITY, true);
    observerService->AddObserver(this, kOpenCaptivePortalLoginEvent, true);
    observerService->AddObserver(this, kClearPrivateData, true);
    observerService->AddObserver(this, kPurge, true);
    observerService->AddObserver(this, "xpcom-shutdown-threads", true);
  }
  nsCOMPtr<nsIPrefBranch> prefBranch;
  GetPrefBranch(getter_AddRefs(prefBranch));
  if (prefBranch) {
    prefBranch->AddObserver(TRR_PREF_PREFIX, this, true);
    prefBranch->AddObserver(kDisableIpv6Pref, this, true);
    prefBranch->AddObserver(kCaptivedetectCanonicalURL, this, true);
  }
  nsCOMPtr<nsICaptivePortalService> captivePortalService =
      do_GetService(NS_CAPTIVEPORTAL_CID);
  if (captivePortalService) {
    int32_t captiveState;
    MOZ_ALWAYS_SUCCEEDS(captivePortalService->GetState(&captiveState));

    if ((captiveState == nsICaptivePortalService::UNLOCKED_PORTAL) ||
        (captiveState == nsICaptivePortalService::NOT_CAPTIVE)) {
      mCaptiveIsPassed = true;
    }
    LOG(("TRRService::Init mCaptiveState=%d mCaptiveIsPassed=%d\n",
         captiveState, (int)mCaptiveIsPassed));
  }

  ReadPrefs(nullptr);

  gTRRService = this;

  nsCOMPtr<nsIThread> thread;
  if (NS_FAILED(NS_NewNamedThread("TRR Background", getter_AddRefs(thread)))) {
    NS_WARNING("NS_NewNamedThread failed!");
    return NS_ERROR_FAILURE;
  }

  sTRRBackgroundThread = thread;

  LOG(("Initialized TRRService\n"));
  return NS_OK;
}

bool TRRService::Enabled() {
  if (mConfirmationState == CONFIRM_INIT &&
      (!StaticPrefs::network_trr_wait_for_portal() || mCaptiveIsPassed ||
       (mMode == MODE_TRRONLY))) {
    LOG(("TRRService::Enabled => CONFIRM_TRYING\n"));
    mConfirmationState = CONFIRM_TRYING;
  }

  if (mConfirmationState == CONFIRM_TRYING) {
    LOG(("TRRService::Enabled MaybeConfirm()\n"));
    MaybeConfirm();
  }

  if (mConfirmationState != CONFIRM_OK) {
    LOG(("TRRService::Enabled mConfirmationState=%d mCaptiveIsPassed=%d\n",
         (int)mConfirmationState, (int)mCaptiveIsPassed));
  }

  return (mConfirmationState == CONFIRM_OK);
}

void TRRService::GetPrefBranch(nsIPrefBranch** result) {
  MOZ_ASSERT(NS_IsMainThread(), "wrong thread");
  *result = nullptr;
  CallGetService(NS_PREFSERVICE_CONTRACTID, result);
}

nsresult TRRService::ReadPrefs(const char* name) {
  MOZ_ASSERT(NS_IsMainThread(), "wrong thread");

  // Whenever a pref change occurs that would cause us to clear the cache
  // we set this to true then do it at the end of the method.
  bool clearEntireCache = false;

  if (!name || !strcmp(name, TRR_PREF("mode"))) {
    // 0 - off, 1 - reserved, 2 - TRR first, 3 - TRR only, 4 - reserved,
    // 5 - explicit off
    uint32_t tmp;
    if (NS_SUCCEEDED(Preferences::GetUint(TRR_PREF("mode"), &tmp))) {
      if (tmp > MODE_TRROFF) {
        tmp = MODE_TRROFF;
      }
      if (tmp == MODE_RESERVED1) {
        tmp = MODE_TRROFF;
      }
      if (tmp == MODE_RESERVED4) {
        tmp = MODE_TRROFF;
      }
      mMode = tmp;
    }
  }
  if (!name || !strcmp(name, TRR_PREF("uri"))) {
    // URI Template, RFC 6570.
    AutoLock lock(mLock);
    nsAutoCString old(mPrivateURI);
    Preferences::GetCString(TRR_PREF("uri"), mPrivateURI);
    nsAutoCString scheme;
    if (!mPrivateURI.IsEmpty()) {
      nsCOMPtr<nsIIOService> ios(do_GetIOService());
      if (ios) {
        ios->ExtractScheme(mPrivateURI, scheme);
      }
    }
    if (!mPrivateURI.IsEmpty() && !scheme.Equals("https")) {
      LOG(("TRRService TRR URI %s is not https. Not used.\n",
           mPrivateURI.get()));
      mPrivateURI.Truncate();
    }
    if (!mPrivateURI.IsEmpty()) {
      // cut off everything from "{" to "}" sequences (potentially multiple),
      // as a crude conversion from template into URI.
      nsAutoCString uri(mPrivateURI);

      do {
        nsCCharSeparatedTokenizer openBrace(uri, '{');
        if (openBrace.hasMoreTokens()) {
          // the 'nextToken' is the left side of the open brace (or full uri)
          nsAutoCString prefix(openBrace.nextToken());

          // if there is an open brace, there's another token
          const nsACString& endBrace = openBrace.nextToken();
          nsCCharSeparatedTokenizer closeBrace(endBrace, '}');
          if (closeBrace.hasMoreTokens()) {
            // there is a close brace as well, make a URI out of the prefix
            // and the suffix
            closeBrace.nextToken();
            nsAutoCString suffix(closeBrace.nextToken());
            uri = prefix + suffix;
          } else {
            // no (more) close brace
            break;
          }
        } else {
          // no (more) open brace
          break;
        }
      } while (true);
      mPrivateURI = uri;
    }
    if (!old.IsEmpty() && !mPrivateURI.Equals(old)) {
      mClearTRRBLStorage = true;
      LOG(("TRRService clearing blacklist because of change is uri service\n"));
      clearEntireCache = true;
    }
  }
  if (!name || !strcmp(name, TRR_PREF("credentials"))) {
    AutoLock lock(mLock);
    Preferences::GetCString(TRR_PREF("credentials"), mPrivateCred);
  }
  if (!name || !strcmp(name, TRR_PREF("confirmationNS"))) {
    AutoLock lock(mLock);
    nsAutoCString old(mConfirmationNS);
    Preferences::GetCString(TRR_PREF("confirmationNS"), mConfirmationNS);
    if (name && !old.IsEmpty() && !mConfirmationNS.Equals(old) &&
        (mConfirmationState > CONFIRM_TRYING)) {
      LOG(("TRR::ReadPrefs: restart confirmationNS state\n"));
      mConfirmationState = CONFIRM_TRYING;
    }
  }
  if (!name || !strcmp(name, TRR_PREF("bootstrapAddress"))) {
    AutoLock lock(mLock);
    Preferences::GetCString(TRR_PREF("bootstrapAddress"), mBootstrapAddr);
    clearEntireCache = true;
  }
  if (!name || !strcmp(name, TRR_PREF("blacklist-duration"))) {
    // prefs is given in number of seconds
    uint32_t secs;
    if (NS_SUCCEEDED(
            Preferences::GetUint(TRR_PREF("blacklist-duration"), &secs))) {
      mTRRBlocklistExpireTime = secs;
    }
  }
  if (!name || !strcmp(name, kDisableIpv6Pref)) {
    bool tmp;
    if (NS_SUCCEEDED(Preferences::GetBool(kDisableIpv6Pref, &tmp))) {
      mDisableIPv6 = tmp;
    }
  }
  if (!name || !strcmp(name, TRR_PREF("excluded-domains")) ||
      !strcmp(name, TRR_PREF("builtin-excluded-domains")) ||
      !strcmp(name, kCaptivedetectCanonicalURL)) {
    AutoLock lock(mLock);
    mExcludedDomains.Clear();

    auto parseExcludedDomains = [this](const char* aPrefName) {
      nsAutoCString excludedDomains;
      Preferences::GetCString(aPrefName, excludedDomains);
      if (excludedDomains.IsEmpty()) {
        return;
      }

      for (const nsACString& tokenSubstring :
           nsCCharSeparatedTokenizerTemplate<
               NS_IsAsciiWhitespace, nsTokenizerFlags::SeparatorOptional>(
               excludedDomains, ',')
               .ToRange()) {
        nsCString token{tokenSubstring};
        LOG(("TRRService::ReadPrefs %s host:[%s]\n", aPrefName, token.get()));
        mExcludedDomains.Insert(token);
      }
    };

    parseExcludedDomains(TRR_PREF("excluded-domains"));
    parseExcludedDomains(TRR_PREF("builtin-excluded-domains"));
    clearEntireCache = true;

    nsAutoCString canonicalSiteURL;
    Preferences::GetCString(kCaptivedetectCanonicalURL, canonicalSiteURL);

    nsCOMPtr<nsIURI> uri;
    nsresult rv = NS_NewURI(getter_AddRefs(uri), canonicalSiteURL,
                            UTF_8_ENCODING, nullptr);
    if (NS_SUCCEEDED(rv)) {
      nsAutoCString host;
      uri->GetHost(host);
      LOG(("TRRService::ReadPrefs captive portal URL:[%s]\n", host.get()));
      mExcludedDomains.Insert(host);
    }
  }

  // if name is null, then we're just now initializing. In that case we don't
  // need to clear the cache.
  if (name && clearEntireCache) {
    if (StaticPrefs::network_trr_clear_cache_on_pref_change()) {
      nsCOMPtr<nsIDNSService> dns = do_GetService(NS_DNSSERVICE_CONTRACTID);
      if (dns) {
        dns->ClearCache(true);
      }
    }
  }

  return NS_OK;
}

nsresult TRRService::GetURI(nsCString& result) {
  AutoLock lock(mLock);
  result = mPrivateURI;
  return NS_OK;
}

nsresult TRRService::GetCredentials(nsCString& result) {
  AutoLock lock(mLock);
  result = mPrivateCred;
  return NS_OK;
}

uint32_t TRRService::GetRequestTimeout() {
  if (mMode == MODE_TRRONLY) {
    return StaticPrefs::network_trr_request_timeout_mode_trronly_ms();
  }

  return StaticPrefs::network_trr_request_timeout_ms();
}

nsresult TRRService::Start() {
  MOZ_ASSERT(NS_IsMainThread(), "wrong thread");
  if (!mInitialized) {
    return NS_ERROR_NOT_INITIALIZED;
  }
  return NS_OK;
}

TRRService::~TRRService() {
  MOZ_ASSERT(NS_IsMainThread(), "wrong thread");
  LOG(("Exiting TRRService\n"));
  gTRRService = nullptr;
}

nsresult TRRService::DispatchTRRRequest(TRR* aTrrRequest) {
  return DispatchTRRRequestInternal(aTrrRequest, true);
}

nsresult TRRService::DispatchTRRRequestInternal(TRR* aTrrRequest,
                                                bool aWithLock) {
  NS_ENSURE_ARG_POINTER(aTrrRequest);
  if (!StaticPrefs::network_trr_fetch_off_main_thread()) {
    return NS_DispatchToMainThread(aTrrRequest);
  }

  RefPtr<TRR> trr = aTrrRequest;
  nsCOMPtr<nsIThread> thread = aWithLock ? TRRThread() : TRRThread_locked();
  if (!thread) {
    return NS_ERROR_FAILURE;
  }

  return thread->Dispatch(trr.forget());
}

already_AddRefed<nsIThread> TRRService::TRRThread() {
  AutoLock lock(mLock);
  return TRRThread_locked();
}

already_AddRefed<nsIThread> TRRService::TRRThread_locked() {
  RefPtr<nsIThread> thread = sTRRBackgroundThread;
  return thread.forget();
}

bool TRRService::IsOnTRRThread() {
  nsCOMPtr<nsIThread> thread;
  {
    AutoLock lock(mLock);
    thread = sTRRBackgroundThread;
  }
  if (!thread) {
    return false;
  }

  return thread->IsOnCurrentThread();
}

NS_IMETHODIMP
TRRService::Observe(nsISupports* aSubject, const char* aTopic,
                    const char16_t* aData) {
  MOZ_ASSERT(NS_IsMainThread(), "wrong thread");
  LOG(("TRR::Observe() topic=%s\n", aTopic));
  if (!strcmp(aTopic, NS_PREFBRANCH_PREFCHANGE_TOPIC_ID)) {
    ReadPrefs(NS_ConvertUTF16toUTF8(aData).get());

    AutoLock lock(mLock);
    if (((mConfirmationState == CONFIRM_INIT) && !mBootstrapAddr.IsEmpty() &&
         (mMode == MODE_TRRONLY)) ||
        (mConfirmationState == CONFIRM_FAILED)) {
      mConfirmationState = CONFIRM_TRYING;
      MaybeConfirm_locked();
    }

  } else if (!strcmp(aTopic, kOpenCaptivePortalLoginEvent)) {
    // We are in a captive portal
    LOG(("TRRservice in captive portal\n"));
    mCaptiveIsPassed = false;
  } else if (!strcmp(aTopic, NS_CAPTIVE_PORTAL_CONNECTIVITY)) {
    nsAutoCString data = NS_ConvertUTF16toUTF8(aData);
    LOG(("TRRservice captive portal was %s\n", data.get()));
    if (!mTRRBLStorage) {
      // We need a lock if we modify mTRRBLStorage variable because it is
      // access off the main thread as well.
      AutoLock lock(mLock);
      mTRRBLStorage = DataStorage::Get(DataStorageClass::TRRBlacklist);
      if (mTRRBLStorage) {
        if (NS_FAILED(mTRRBLStorage->Init(nullptr))) {
          mTRRBLStorage = nullptr;
        }
        if (mClearTRRBLStorage) {
          if (mTRRBLStorage) {
            mTRRBLStorage->Clear();
          }
          mClearTRRBLStorage = false;
        }
      }
    }

    if (!mCaptiveIsPassed) {
      if (mConfirmationState != CONFIRM_OK) {
        mConfirmationState = CONFIRM_TRYING;
        MaybeConfirm();
      }
    } else {
      LOG(("TRRservice CP clear when already up!\n"));
    }

    mCaptiveIsPassed = true;

  } else if (!strcmp(aTopic, kClearPrivateData) || !strcmp(aTopic, kPurge)) {
    // flush the TRR blocklist, both in-memory and on-disk
    if (mTRRBLStorage) {
      mTRRBLStorage->Clear();
    }
  } else if (!strcmp(aTopic, "xpcom-shutdown-threads")) {
    if (sTRRBackgroundThread) {
      nsCOMPtr<nsIThread> thread;
      {
        AutoLock lock(mLock);
        thread = sTRRBackgroundThread.get();
        sTRRBackgroundThread = nullptr;
      }
      MOZ_ALWAYS_SUCCEEDS(thread->Shutdown());
    }
  }
  return NS_OK;
}

void TRRService::CheckVPNStatus(nsINetworkLinkService* aLinkService) {
  if (!aLinkService) {
    return;
  }

  bool vpnDetected = false;
  aLinkService->GetVpnDetected(&vpnDetected);
  mVPNDetected = vpnDetected;
  LOG(("TRRService vpnDetected=%d", vpnDetected));
}

void TRRService::MaybeConfirm() {
  AutoLock lock(mLock);
  MaybeConfirm_locked();
}

void TRRService::MaybeConfirm_locked() {
  mLock.AssertCurrentThreadOwns();
  if (TRR_DISABLED(mMode) || mConfirmer ||
      mConfirmationState != CONFIRM_TRYING) {
    LOG(
        ("TRRService:MaybeConfirm mode=%d, mConfirmer=%p "
         "mConfirmationState=%d\n",
         (int)mMode, (void*)mConfirmer, (int)mConfirmationState));
    return;
  }

  if (mConfirmationNS.Equals("skip") || mMode == MODE_TRRONLY) {
    LOG(("TRRService starting confirmation test %s SKIPPED\n",
         mPrivateURI.get()));
    mConfirmationState = CONFIRM_OK;
  } else {
    LOG(("TRRService starting confirmation test %s %s\n", mPrivateURI.get(),
         mConfirmationNS.get()));
    mConfirmer = new TRR(this, mConfirmationNS, TRRTYPE_NS, ""_ns, false);
    DispatchTRRRequestInternal(mConfirmer, false);
  }
}

bool TRRService::MaybeBootstrap(const nsACString& aPossible,
                                nsACString& aResult) {
  AutoLock lock(mLock);
  if (TRR_DISABLED(mMode) || mBootstrapAddr.IsEmpty()) {
    return false;
  }

  nsCOMPtr<nsIURI> url;
  nsresult rv =
      NS_MutateURI(NS_STANDARDURLMUTATOR_CONTRACTID)
          .Apply(&nsIStandardURLMutator::Init, nsIStandardURL::URLTYPE_STANDARD,
                 443, mPrivateURI, nullptr, nullptr, nullptr)
          .Finalize(url);
  if (NS_FAILED(rv)) {
    LOG(("TRRService::MaybeBootstrap failed to create URI!\n"));
    return false;
  }

  nsAutoCString host;
  url->GetHost(host);
  if (!aPossible.Equals(host)) {
    return false;
  }
  LOG(("TRRService::MaybeBootstrap: use %s instead of %s\n",
       mBootstrapAddr.get(), host.get()));
  aResult = mBootstrapAddr;
  return true;
}

bool TRRService::IsDomainBlocked(const nsACString& aHost,
                                 const nsACString& aOriginSuffix,
                                 bool aPrivateBrowsing) {
  if (!Enabled()) {
    return true;
  }

  // It's OK to call this method here because it only happens on the main
  // thread, and we only change the excluded domains/dns suffix list
  // on the main thread in response to observer notifications.
  // Calling the locking version of this method would cause us to grab
  // the mutex for every label of the hostname, which would be very
  // inefficient.
  if (NS_IsMainThread()) {
    if (IsExcludedFromTRR_unlocked(aHost)) {
      return true;
    }
  } else {
    MOZ_ASSERT(IsOnTRRThread());
    if (IsExcludedFromTRR(aHost)) {
      return true;
    }
  }

  if (!mTRRBLStorage) {
    return false;
  }

  if (mClearTRRBLStorage) {
    mTRRBLStorage->Clear();
    mClearTRRBLStorage = false;
    return false;  // just cleared!
  }

  // use a unified casing for the hashkey
  nsAutoCString hashkey(aHost + aOriginSuffix);
  nsCString val(mTRRBLStorage->Get(hashkey, aPrivateBrowsing
                                                ? DataStorage_Private
                                                : DataStorage_Persistent));

  if (!val.IsEmpty()) {
    nsresult code;
    int32_t until = val.ToInteger(&code) + mTRRBlocklistExpireTime;
    int32_t expire = NowInSeconds();
    if (NS_SUCCEEDED(code) && (until > expire)) {
      LOG(("Host [%s] is TRR blocklisted\n", nsCString(aHost).get()));
      return true;
    }

    // the blocklisted entry has expired
    mTRRBLStorage->Remove(hashkey, aPrivateBrowsing ? DataStorage_Private
                                                    : DataStorage_Persistent);
  }
  return false;
}

// When running in TRR-only mode, the blocklist is not used and it will also
// try resolving the localhost / .local names.
bool TRRService::IsTemporarilyBlocked(const nsACString& aHost,
                                      const nsACString& aOriginSuffix,
                                      bool aPrivateBrowsing,
                                      bool aParentsToo)  // false if domain
{
  if (mMode == MODE_TRRONLY) {
    return false;  // might as well try
  }

  LOG(("Checking if host [%s] is blocklisted", aHost.BeginReading()));

  int32_t dot = aHost.FindChar('.');
  if ((dot == kNotFound) && aParentsToo) {
    // Only if a full host name. Domains can be dotless to be able to
    // blocklist entire TLDs
    return true;
  }

  if (IsDomainBlocked(aHost, aOriginSuffix, aPrivateBrowsing)) {
    return true;
  }

  nsDependentCSubstring domain = Substring(aHost, 0);
  while (dot != kNotFound) {
    dot++;
    domain.Rebind(domain, dot, domain.Length() - dot);

    if (IsDomainBlocked(domain, aOriginSuffix, aPrivateBrowsing)) {
      return true;
    }

    dot = domain.FindChar('.');
  }

  return false;
}

bool TRRService::IsExcludedFromTRR(const nsACString& aHost) {
  // This method may be called off the main thread. We need to lock so
  // mExcludedDomains and mDNSSuffixDomains don't change while this code
  // is running.
  AutoLock lock(mLock);

  return IsExcludedFromTRR_unlocked(aHost);
}

bool TRRService::IsExcludedFromTRR_unlocked(const nsACString& aHost) {
  if (mVPNDetected && !StaticPrefs::network_trr_enable_when_vpn_detected()) {
    LOG(("%s is excluded from TRR because of VPN", aHost.BeginReading()));
    return true;
  }
  if (!NS_IsMainThread()) {
    mLock.AssertCurrentThreadOwns();
  }

  int32_t dot = 0;
  // iteratively check the sub-domain of |aHost|
  while (dot < static_cast<int32_t>(aHost.Length())) {
    nsDependentCSubstring subdomain =
        Substring(aHost, dot, aHost.Length() - dot);

    if (mExcludedDomains.Contains(subdomain)) {
      LOG(("Subdomain [%s] of host [%s] Is Excluded From TRR via pref\n",
           subdomain.BeginReading(), aHost.BeginReading()));
      return true;
    }

    dot = aHost.FindChar('.', dot + 1);
    if (dot == kNotFound) {
      break;
    }
    dot++;
  }

  return false;
}

class ProxyBlockList : public Runnable {
 public:
  ProxyBlockList(TRRService* service, const nsACString& aHost,
                 const nsACString& aOriginSuffix, bool pb, bool aParentsToo)
      : mozilla::Runnable("ProxyBlockList"),
        mService(service),
        mHost(aHost),
        mOriginSuffix(aOriginSuffix),
        mPB(pb),
        mParentsToo(aParentsToo) {}

  NS_IMETHOD Run() override {
    mService->AddToBlocklist(mHost, mOriginSuffix, mPB, mParentsToo);
    mService = nullptr;
    return NS_OK;
  }

 private:
  RefPtr<TRRService> mService;
  nsCString mHost;
  nsCString mOriginSuffix;
  bool mPB;
  bool mParentsToo;
};

void TRRService::AddToBlocklist(const nsACString& aHost,
                                const nsACString& aOriginSuffix,
                                bool privateBrowsing, bool aParentsToo) {
  {
    AutoLock lock(mLock);
    if (!mTRRBLStorage) {
      return;
    }
  }

  if (!NS_IsMainThread()) {
    NS_DispatchToMainThread(new ProxyBlockList(this, aHost, aOriginSuffix,
                                               privateBrowsing, aParentsToo));
    return;
  }

  MOZ_ASSERT(NS_IsMainThread());

  LOG(("TRR blocklist %s\n", nsCString(aHost).get()));
  nsAutoCString hashkey(aHost + aOriginSuffix);
  nsAutoCString val;
  val.AppendInt(NowInSeconds());  // creation time

  // this overwrites any existing entry
  mTRRBLStorage->Put(
      hashkey, val,
      privateBrowsing ? DataStorage_Private : DataStorage_Persistent);

  if (aParentsToo) {
    // when given a full host name, verify its domain as well
    int32_t dot = aHost.FindChar('.');
    if (dot != kNotFound) {
      // this has a domain to be checked
      dot++;
      nsDependentCSubstring domain =
          Substring(aHost, dot, aHost.Length() - dot);
      nsAutoCString check(domain);
      if (IsTemporarilyBlocked(check, aOriginSuffix, privateBrowsing, false)) {
        // the domain part is already blocklisted, no need to add this entry
        return;
      }
      // verify 'check' over TRR
      LOG(("TRR: verify if '%s' resolves as NS\n", check.get()));

      // check if there's an NS entry for this name
      RefPtr<TRR> trr =
          new TRR(this, check, TRRTYPE_NS, aOriginSuffix, privateBrowsing);
      DispatchTRRRequest(trr);
    }
  }
}

NS_IMETHODIMP
TRRService::Notify(nsITimer* aTimer) {
  if (aTimer == mRetryConfirmTimer) {
    mRetryConfirmTimer = nullptr;
    if (mConfirmationState == CONFIRM_FAILED) {
      LOG(("TRRService retry NS of %s\n", mConfirmationNS.get()));
      mConfirmationState = CONFIRM_TRYING;
      MaybeConfirm();
    }
  } else {
    MOZ_CRASH("Unknown timer");
  }

  return NS_OK;
}

void TRRService::TRRIsOkay(enum TrrOkay aReason) {
  MOZ_ASSERT_IF(XRE_IsParentProcess(), NS_IsMainThread() || IsOnTRRThread());

  if (aReason == OKAY_NORMAL) {
    mTRRFailures = 0;
  } else if ((mMode == MODE_TRRFIRST) && (mConfirmationState == CONFIRM_OK)) {
    // only count failures while in OK state
    uint32_t fails = ++mTRRFailures;
    if (fails >= StaticPrefs::network_trr_max_fails()) {
      LOG(("TRRService goes FAILED after %u failures in a row\n", fails));
      mConfirmationState = CONFIRM_FAILED;
      // Fire off a timer and start re-trying the NS domain again
      NS_NewTimerWithCallback(getter_AddRefs(mRetryConfirmTimer), this,
                              mRetryConfirmInterval, nsITimer::TYPE_ONE_SHOT);
      mTRRFailures = 0;  // clear it again
    }
  }
}

AHostResolver::LookupStatus TRRService::CompleteLookup(
    nsHostRecord* rec, nsresult status, AddrInfo* aNewRRSet, bool pb,
    const nsACString& aOriginSuffix) {
  // this is an NS check for the TRR blocklist or confirmationNS check

  MOZ_ASSERT_IF(XRE_IsParentProcess(), NS_IsMainThread() || IsOnTRRThread());
  MOZ_ASSERT(!rec);

  RefPtr<AddrInfo> newRRSet(aNewRRSet);
  MOZ_ASSERT(newRRSet && newRRSet->IsTRR() == TRRTYPE_NS);

#ifdef DEBUG
  {
    AutoLock lock(mLock);
    MOZ_ASSERT(!mConfirmer || (mConfirmationState == CONFIRM_TRYING));
  }
#endif
  if (mConfirmationState == CONFIRM_TRYING) {
    {
      AutoLock lock(mLock);
      MOZ_ASSERT(mConfirmer);
      mConfirmationState = NS_SUCCEEDED(status) ? CONFIRM_OK : CONFIRM_FAILED;
      LOG(("TRRService finishing confirmation test %s %d %X\n",
           mPrivateURI.get(), (int)mConfirmationState, (unsigned int)status));
      mConfirmer = nullptr;
    }
    if (mConfirmationState == CONFIRM_FAILED) {
      // retry failed NS confirmation
      NS_NewTimerWithCallback(getter_AddRefs(mRetryConfirmTimer), this,
                              mRetryConfirmInterval, nsITimer::TYPE_ONE_SHOT);
      if (mRetryConfirmInterval < 64000) {
        // double the interval up to this point
        mRetryConfirmInterval *= 2;
      }
    } else {
      // A fresh confirmation means previous blocked entries might not
      // be valid anymore.
      mClearTRRBLStorage = true;
      mRetryConfirmInterval = 1000;
    }
    return LOOKUP_OK;
  }

  // when called without a host record, this is a domain name check response.
  if (NS_SUCCEEDED(status)) {
    LOG(("TRR verified %s to be fine!\n", newRRSet->mHostName.get()));
  } else {
    LOG(("TRR says %s doesn't resolve as NS!\n", newRRSet->mHostName.get()));
    AddToBlocklist(newRRSet->mHostName, aOriginSuffix, pb, false);
  }
  return LOOKUP_OK;
}

AHostResolver::LookupStatus TRRService::CompleteLookupByType(
    nsHostRecord*, nsresult, mozilla::net::TypeRecordResultType& aResult,
    uint32_t aTtl, bool aPb) {
  return LOOKUP_OK;
}

#undef LOG

}  // namespace net
}  // namespace mozilla
