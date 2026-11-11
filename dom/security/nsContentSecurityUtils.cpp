/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A namespace class for static content security utilities. */

#include "nsContentSecurityUtils.h"

#include "mozilla/Components.h"
#include "nsIContentSecurityPolicy.h"
#include "nsIChannel.h"
#include "nsIHttpChannel.h"
#include "nsIMultiPartChannel.h"
#include "nsIURI.h"

#include "js/Array.h"  // JS::GetArrayLength
#include "js/friend/ErrorMessages.h"  // JSMSG_UNSAFE_FILENAME
#include "mozilla/Logging.h"
#include "mozilla/dom/Document.h"
#include "mozilla/StaticPrefs_security.h"
#include "mozilla/StaticPrefs_extensions.h"
#include "mozilla/StaticPrefs_dom.h"
#include "nsIConsoleService.h"
#include "nsIStringBundle.h"

using namespace mozilla;
using namespace mozilla::dom;

extern mozilla::LazyLogModule sCSMLog;

// Helper function for IsConsideredSameOriginForUIR which makes
// Principals of scheme 'http' return Principals of scheme 'https'.
static already_AddRefed<nsIPrincipal> MakeHTTPPrincipalHTTPS(
    nsIPrincipal* aPrincipal) {
  nsCOMPtr<nsIPrincipal> principal = aPrincipal;
  // if the principal is not http, then it can also not be upgraded
  // to https.
  if (!principal->SchemeIs("http")) {
    return principal.forget();
  }

  nsAutoCString spec;
  aPrincipal->GetAsciiSpec(spec);
  // replace http with https
  spec.ReplaceLiteral(0, 4, "https");

  nsCOMPtr<nsIURI> newURI;
  nsresult rv = NS_NewURI(getter_AddRefs(newURI), spec);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return nullptr;
  }

  mozilla::OriginAttributes OA =
      BasePrincipal::Cast(aPrincipal)->OriginAttributesRef();

  principal = BasePrincipal::CreateContentPrincipal(newURI, OA);
  return principal.forget();
}

/* static */
bool nsContentSecurityUtils::IsConsideredSameOriginForUIR(
    nsIPrincipal* aTriggeringPrincipal, nsIPrincipal* aResultPrincipal) {
  MOZ_ASSERT(aTriggeringPrincipal);
  MOZ_ASSERT(aResultPrincipal);
  // we only have to make sure that the following truth table holds:
  // aTriggeringPrincipal         | aResultPrincipal             | Result
  // ----------------------------------------------------------------
  // http://example.com/foo.html  | http://example.com/bar.html  | true
  // http://example.com/foo.html  | https://example.com/bar.html | true
  // https://example.com/foo.html | https://example.com/bar.html | true
  // https://example.com/foo.html | http://example.com/bar.html  | true

  // fast path if both principals are same-origin
  if (aTriggeringPrincipal->Equals(aResultPrincipal)) {
    return true;
  }

  // in case a principal uses a scheme of 'http' then we just upgrade to
  // 'https' and use the principal equals comparison operator to check
  // for same-origin.
  nsCOMPtr<nsIPrincipal> compareTriggeringPrincipal =
      MakeHTTPPrincipalHTTPS(aTriggeringPrincipal);

  nsCOMPtr<nsIPrincipal> compareResultPrincipal =
      MakeHTTPPrincipalHTTPS(aResultPrincipal);

  return compareTriggeringPrincipal->Equals(compareResultPrincipal);
}

class EvalUsageNotificationRunnable final : public Runnable {
 public:
  EvalUsageNotificationRunnable(bool aIsSystemPrincipal,
                                NS_ConvertUTF8toUTF16& aFileNameA,
                                uint64_t aWindowID, uint32_t aLineNumber,
                                uint32_t aColumnNumber)
      : mozilla::Runnable("EvalUsageNotificationRunnable"),
        mIsSystemPrincipal(aIsSystemPrincipal),
        mFileNameA(aFileNameA),
        mWindowID(aWindowID),
        mLineNumber(aLineNumber),
        mColumnNumber(aColumnNumber) {}

  NS_IMETHOD Run() override {
    nsContentSecurityUtils::NotifyEvalUsage(
        mIsSystemPrincipal, mFileNameA, mWindowID, mLineNumber, mColumnNumber);
    return NS_OK;
  }

  void Revoke() {}

 private:
  bool mIsSystemPrincipal;
  NS_ConvertUTF8toUTF16 mFileNameA;
  uint64_t mWindowID;
  uint32_t mLineNumber;
  uint32_t mColumnNumber;
};

/* static */
bool nsContentSecurityUtils::IsEvalAllowed(JSContext* cx,
                                           bool aIsSystemPrincipal,
                                           const nsAString& aScript) {
  // This allowlist contains files that are permanently allowed to use
  // eval()-like functions. It will ideally be restricted to files that are
  // exclusively used in testing contexts.
  static nsLiteralCString evalAllowlist[] = {
      // Test-only third-party library
      "resource://testing-common/sinon-7.2.7.js"_ns,
      // Test-only third-party library
      "resource://testing-common/ajv-4.1.1.js"_ns,
      // Test-only utility
      "resource://testing-common/content-task.js"_ns,

      // Tracked by Bug 1584605
      "resource:///modules/translation/cld-worker.js"_ns,

      // require.js implements a script loader for workers. It uses eval
      // to load the script; but injection is only possible in situations
      // that you could otherwise control script that gets executed, so
      // it is okay to allow eval() as it adds no additional attack surface.
      // Bug 1584564 tracks requiring safe usage of require.js
      "resource://gre/modules/workers/require.js"_ns,

      // The Browser Toolbox/Console
      "debugger"_ns,
  };

  // We also permit two specific idioms in eval()-like contexts. We'd like to
  // elminate these too; but there are in-the-wild Mozilla privileged extensions
  // that use them.
  static constexpr auto sAllowedEval1 = u"this"_ns;
  static constexpr auto sAllowedEval2 =
      u"function anonymous(\n) {\nreturn this\n}"_ns;

  if (MOZ_LIKELY(!aIsSystemPrincipal && !XRE_IsE10sParentProcess())) {
    // We restrict eval in the system principal and parent process.
    // Other uses (like web content and null principal) are allowed.
    return true;
  }

  if (JS::ContextOptionsRef(cx).disableEvalSecurityChecks()) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() because this JSContext was set to allow it"));
    return true;
  }

  if (aIsSystemPrincipal &&
      StaticPrefs::security_allow_eval_with_system_principal()) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() with System Principal because allowing pref is "
             "enabled"));
    return true;
  }

  if (XRE_IsE10sParentProcess() &&
      StaticPrefs::security_allow_eval_in_parent_process()) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() in parent process because allowing pref is "
             "enabled"));
    return true;
  }

  // We only perform a check of this preference on the Main Thread
  // (because a String-based preference check is only safe on Main Thread.)
  // The consequence of this is that if a user is using userChromeJS _and_
  // the scripts they use start a worker and that worker uses eval - we will
  // enter this function, skip over this pref check that would normally cause
  // us to allow the eval usage - and we will block it.
  // While not ideal, we do not officially support userChromeJS, and hopefully
  // the usage of workers and eval in workers is even lower that userChromeJS
  // usage.
  if (NS_IsMainThread()) {
    // This preference is a file used for autoconfiguration of Firefox
    // by administrators. It has also been (ab)used by the userChromeJS
    // project to run legacy-style 'extensions', some of which use eval,
    // all of which run in the System Principal context.
    nsAutoString jsConfigPref;
    Preferences::GetString("general.config.filename", jsConfigPref);
    if (!jsConfigPref.IsEmpty()) {
      MOZ_LOG(sCSMLog, LogLevel::Debug,
              ("Allowing eval() %s because of "
               "general.config.filename",
               (aIsSystemPrincipal ? "with System Principal"
                                   : "in parent process")));
      return true;
    }
  }

  if (XRE_IsE10sParentProcess() &&
      !StaticPrefs::extensions_webextensions_remote()) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() in parent process because the web extension "
             "process is disabled"));
    return true;
  }

  // We permit these two common idioms to get access to the global JS object
  if (!aScript.IsEmpty() &&
      (aScript == sAllowedEval1 || aScript == sAllowedEval2)) {
    MOZ_LOG(
        sCSMLog, LogLevel::Debug,
        ("Allowing eval() %s because a key string is "
         "provided",
         (aIsSystemPrincipal ? "with System Principal" : "in parent process")));
    return true;
  }

  // Check the allowlist for the provided filename. getFilename is a helper
  // function
  nsAutoCString fileName;
  uint32_t lineNumber = 0, columnNumber = 0;
  nsJSUtils::GetCallingLocation(cx, fileName, &lineNumber, &columnNumber);
  if (fileName.IsEmpty()) {
    fileName = "unknown-file"_ns;
  }

  NS_ConvertUTF8toUTF16 fileNameA(fileName);
  for (const nsLiteralCString& allowlistEntry : evalAllowlist) {
    // checking if current filename begins with entry, because JS Engine
    // gives us additional stuff for code inside eval or Function ctor
    // e.g., "require.js > Function"
    if (StringBeginsWith(fileName, allowlistEntry)) {
      MOZ_LOG(sCSMLog, LogLevel::Debug,
              ("Allowing eval() %s because the containing "
               "file is in the allowlist",
               (aIsSystemPrincipal ? "with System Principal"
                                   : "in parent process")));
      return true;
    }
  }

  // Send Log to the Console
  uint64_t windowID = nsJSUtils::GetCurrentlyRunningCodeInnerWindowID(cx);
  if (NS_IsMainThread()) {
    nsContentSecurityUtils::NotifyEvalUsage(aIsSystemPrincipal, fileNameA,
                                            windowID, lineNumber, columnNumber);
  } else {
    auto runnable = new EvalUsageNotificationRunnable(
        aIsSystemPrincipal, fileNameA, windowID, lineNumber, columnNumber);
    NS_DispatchToMainThread(runnable);
  }

  // Log to MOZ_LOG
  MOZ_LOG(sCSMLog, LogLevel::Warning,
          ("Blocking eval() %s from file %s and script "
           "provided %s",
           (aIsSystemPrincipal ? "with System Principal" : "in parent process"),
           fileName.get(), NS_ConvertUTF16toUTF8(aScript).get()));

  // Maybe Crash
#ifdef DEBUG
  // MOZ_CRASH_UNSAFE_PRINTF gives us at most 1024 characters to print.
  // The given string literal leaves us with ~950, so I'm leaving
  // each 475 for fileName and aScript each.
  if (fileName.Length() > 475) {
    fileName.SetLength(475);
  }
  nsAutoCString trimmedScript = NS_ConvertUTF16toUTF8(aScript);
  if (trimmedScript.Length() > 475) {
    trimmedScript.SetLength(475);
  }
  MOZ_CRASH_UNSAFE_PRINTF(
      "Blocking eval() %s from file %s and script provided "
      "%s",
      (aIsSystemPrincipal ? "with System Principal" : "in parent process"),
      fileName.get(), trimmedScript.get());
#endif

  return false;
}

/* static */
void nsContentSecurityUtils::NotifyEvalUsage(bool aIsSystemPrincipal,
                                             NS_ConvertUTF8toUTF16& aFileNameA,
                                             uint64_t aWindowID,
                                             uint32_t aLineNumber,
                                             uint32_t aColumnNumber) {

  // Report an error to console
  nsCOMPtr<nsIConsoleService> console(
      do_GetService(NS_CONSOLESERVICE_CONTRACTID));
  if (!console) {
    return;
  }
  nsCOMPtr<nsIScriptError> error(do_CreateInstance(NS_SCRIPTERROR_CONTRACTID));
  if (!error) {
    return;
  }
  nsCOMPtr<nsIStringBundle> bundle;
  nsCOMPtr<nsIStringBundleService> stringService =
      mozilla::components::StringBundle::Service();
  if (!stringService) {
    return;
  }
  stringService->CreateBundle(
      "chrome://global/locale/security/security.properties",
      getter_AddRefs(bundle));
  if (!bundle) {
    return;
  }
  nsAutoString message;
  AutoTArray<nsString, 1> formatStrings = {aFileNameA};
  nsresult rv = bundle->FormatStringFromName("RestrictBrowserEvalUsage",
                                             formatStrings, message);
  if (NS_FAILED(rv)) {
    return;
  }

  rv = error->InitWithWindowID(message, aFileNameA, u""_ns, aLineNumber,
                               aColumnNumber, nsIScriptError::errorFlag,
                               "BrowserEvalUsage", aWindowID,
                               true /* From chrome context */);
  if (NS_FAILED(rv)) {
    return;
  }
  console->LogMessage(error);
}

/* static */
nsresult nsContentSecurityUtils::GetHttpChannelFromPotentialMultiPart(
    nsIChannel* aChannel, nsIHttpChannel** aHttpChannel) {
  nsCOMPtr<nsIHttpChannel> httpChannel = do_QueryInterface(aChannel);
  if (httpChannel) {
    httpChannel.forget(aHttpChannel);
    return NS_OK;
  }

  nsCOMPtr<nsIMultiPartChannel> multipart = do_QueryInterface(aChannel);
  if (!multipart) {
    *aHttpChannel = nullptr;
    return NS_OK;
  }

  nsCOMPtr<nsIChannel> baseChannel;
  nsresult rv = multipart->GetBaseChannel(getter_AddRefs(baseChannel));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return rv;
  }

  httpChannel = do_QueryInterface(baseChannel);
  httpChannel.forget(aHttpChannel);

  return NS_OK;
}

nsresult ParseCSPAndEnforceFrameAncestorCheck(
    nsIChannel* aChannel, nsIContentSecurityPolicy** aOutCSP) {
  MOZ_ASSERT(aChannel);

  // CSP can only hang off an http channel, if this channel is not
  // an http channel then there is nothing to do here.
  nsCOMPtr<nsIHttpChannel> httpChannel;
  nsresult rv = nsContentSecurityUtils::GetHttpChannelFromPotentialMultiPart(
      aChannel, getter_AddRefs(httpChannel));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return rv;
  }

  if (!httpChannel) {
    return NS_OK;
  }

  nsCOMPtr<nsILoadInfo> loadInfo = aChannel->LoadInfo();
  ExtContentPolicyType contentType = loadInfo->GetExternalContentPolicyType();
  // frame-ancestor check only makes sense for subdocument and object loads,
  // if this is not a load of such type, there is nothing to do here.
  if (contentType != ExtContentPolicy::TYPE_SUBDOCUMENT &&
      contentType != ExtContentPolicy::TYPE_OBJECT) {
    return NS_OK;
  }

  nsAutoCString tCspHeaderValue, tCspROHeaderValue;

  Unused << httpChannel->GetResponseHeader(
      NS_LITERAL_CSTRING("content-security-policy"), tCspHeaderValue);

  Unused << httpChannel->GetResponseHeader(
      NS_LITERAL_CSTRING("content-security-policy-report-only"),
      tCspROHeaderValue);

  // if there are no CSP values, then there is nothing to do here.
  if (tCspHeaderValue.IsEmpty() && tCspROHeaderValue.IsEmpty()) {
    return NS_OK;
  }

  NS_ConvertASCIItoUTF16 cspHeaderValue(tCspHeaderValue);
  NS_ConvertASCIItoUTF16 cspROHeaderValue(tCspROHeaderValue);

  RefPtr<nsCSPContext> csp = new nsCSPContext();
  nsCOMPtr<nsIPrincipal> resultPrincipal;
  rv = nsContentUtils::GetSecurityManager()->GetChannelResultPrincipal(
      aChannel, getter_AddRefs(resultPrincipal));
  NS_ENSURE_SUCCESS(rv, rv);
  nsCOMPtr<nsIURI> selfURI;
  aChannel->GetURI(getter_AddRefs(selfURI));

  nsCOMPtr<nsIReferrerInfo> referrerInfo = httpChannel->GetReferrerInfo();
  nsAutoString referrerSpec;
  if (referrerInfo) {
    referrerInfo->GetComputedReferrerSpec(referrerSpec);
  }
  uint64_t innerWindowID = loadInfo->GetInnerWindowID();

  rv = csp->SetRequestContextWithPrincipal(resultPrincipal, selfURI,
                                           referrerSpec, innerWindowID);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return rv;
  }

  // ----- if there's a full-strength CSP header, apply it.
  if (!cspHeaderValue.IsEmpty()) {
    rv = CSP_AppendCSPFromHeader(csp, cspHeaderValue, false);
    NS_ENSURE_SUCCESS(rv, rv);
  }

  // ----- if there's a report-only CSP header, apply it.
  if (!cspROHeaderValue.IsEmpty()) {
    rv = CSP_AppendCSPFromHeader(csp, cspROHeaderValue, true);
    NS_ENSURE_SUCCESS(rv, rv);
  }

  // ----- Enforce frame-ancestor policy on any applied policies
  bool safeAncestry = false;
  // PermitsAncestry sends violation reports when necessary
  rv = csp->PermitsAncestry(loadInfo, &safeAncestry);

  if (NS_FAILED(rv) || !safeAncestry) {
    // stop!  ERROR page!
    aChannel->Cancel(NS_ERROR_CSP_FRAME_ANCESTOR_VIOLATION);
    return NS_ERROR_CSP_FRAME_ANCESTOR_VIOLATION;
  }

  // return the CSP for x-frame-options check
  csp.forget(aOutCSP);

  return NS_OK;
}

void EnforceXFrameOptionsCheck(nsIChannel* aChannel,
                               nsIContentSecurityPolicy* aCsp) {
  MOZ_ASSERT(aChannel);
  if (!FramingChecker::CheckFrameOptions(aChannel, aCsp)) {
    // stop!  ERROR page!
    aChannel->Cancel(NS_ERROR_XFO_VIOLATION);
  }
}

/* static */
void nsContentSecurityUtils::PerformCSPFrameAncestorAndXFOCheck(
    nsIChannel* aChannel) {
  nsCOMPtr<nsIContentSecurityPolicy> csp;
  nsresult rv =
      ParseCSPAndEnforceFrameAncestorCheck(aChannel, getter_AddRefs(csp));
  if (NS_FAILED(rv)) {
    return;
  }

  // X-Frame-Options needs to be enforced after CSP frame-ancestors
  // checks because if frame-ancestors is present, then x-frame-options
  // will be discarded
  EnforceXFrameOptionsCheck(aChannel, csp);
}

#if defined(DEBUG)
/* static */
void nsContentSecurityUtils::AssertAboutPageHasCSP(Document* aDocument) {
  // We want to get to a point where all about: pages ship with a CSP. This
  // assertion ensures that we can not deploy new about: pages without a CSP.
  // Please note that any about: page should not use inline JS or inline CSS,
  // and instead should load JS and CSS from an external file (*.js, *.css)
  // which allows us to apply a strong CSP omitting 'unsafe-inline'. Ideally,
  // the CSP allows precisely the resources that need to be loaded; but it
  // should at least be as strong as:
  // <meta http-equiv="Content-Security-Policy" content="default-src chrome:;
  // object-src 'none'"/>

  // Check if we should skip the assertion
#ifdef DEBUG
  if (StaticPrefs::dom_security_skip_about_page_has_csp_assert()) {
    return;
  }
#endif

  // Check if we are loading an about: URI at all
  nsCOMPtr<nsIURI> documentURI = aDocument->GetDocumentURI();
  if (!documentURI->SchemeIs("about")) {
    return;
  }

  nsCOMPtr<nsIContentSecurityPolicy> csp = aDocument->GetCsp();
  bool foundDefaultSrc = false;
  bool foundObjectSrc = false;
  bool foundUnsafeEval = false;
  bool foundUnsafeInline = false;
  if (csp) {
    uint32_t policyCount = 0;
    csp->GetPolicyCount(&policyCount);
    nsAutoString parsedPolicyStr;
    for (uint32_t i = 0; i < policyCount; ++i) {
      csp->GetPolicyString(i, parsedPolicyStr);
      if (parsedPolicyStr.Find("default-src") >= 0) {
        foundDefaultSrc = true;
      }
      if (parsedPolicyStr.Find("object-src 'none'") >= 0) {
        foundObjectSrc = true;
      }
      if (parsedPolicyStr.Find("'unsafe-eval'") >= 0) {
        foundUnsafeEval = true;
      }
      if (parsedPolicyStr.Find("'unsafe-inline'") >= 0) {
        foundUnsafeInline = true;
      }
    }
  }

  // Check if we should skip the allowlist and assert right away. Please note
  // that this pref can and should only be set for automated testing.
#ifdef DEBUG
  if (StaticPrefs::dom_security_skip_about_page_csp_allowlist_and_assert()) {
    NS_ASSERTION(foundDefaultSrc, "about: page must have a CSP");
    return;
  }
#endif

  nsAutoCString aboutSpec;
  documentURI->GetSpec(aboutSpec);
  ToLowerCase(aboutSpec);

  // This allowlist contains about: pages that are permanently allowed to
  // render without a CSP applied.
  static nsLiteralCString sAllowedAboutPagesWithNoCSP[] = {
    // about:blank is a special about page -> no CSP
    "about:blank"_ns,
    // about:srcdoc is a special about page -> no CSP
    "about:srcdoc"_ns,
    // about:sync-log displays plain text only -> no CSP
    "about:sync-log"_ns,
    // about:printpreview displays plain text only -> no CSP
    "about:printpreview"_ns,
    // about:logo just displays the firefox logo -> no CSP
    "about:logo"_ns,
#  if defined(ANDROID)
    "about:config"_ns,
#  endif
  };

  for (const nsLiteralCString& allowlistEntry : sAllowedAboutPagesWithNoCSP) {
    // please note that we perform a substring match here on purpose,
    // so we don't have to deal and parse out all the query arguments
    // the various about pages rely on.
    if (StringBeginsWith(aboutSpec, allowlistEntry)) {
      return;
    }
  }

  MOZ_ASSERT(foundDefaultSrc,
             "about: page must contain a CSP including default-src");
  MOZ_ASSERT(foundObjectSrc,
             "about: page must contain a CSP denying object-src");

  if (aDocument->IsExtensionPage()) {
    // Extensions have two CSP policies applied where the baseline CSP
    // includes 'unsafe-eval' and 'unsafe-inline', hence we have to skip
    // the 'unsafe-eval' and 'unsafe-inline' assertions for extension
    // pages.
    return;
  }

  MOZ_ASSERT(!foundUnsafeEval,
             "about: page must not contain a CSP including 'unsafe-eval'");

  static nsLiteralCString sLegacyUnsafeInlineAllowList[] = {
      // Bug 1579160: Remove 'unsafe-inline' from style-src within
      // about:preferences
      "about:preferences"_ns,
      // Bug 1571346: Remove 'unsafe-inline' from style-src within about:addons
      "about:addons"_ns,
      // Bug 1584485: Remove 'unsafe-inline' from style-src within:
      // * about:newtab
      // * about:welcome
      // * about:home
      "about:newtab"_ns,
      "about:welcome"_ns,
      "about:home"_ns,
  };

  for (const nsLiteralCString& aUnsafeInlineEntry :
       sLegacyUnsafeInlineAllowList) {
    // please note that we perform a substring match here on purpose,
    // so we don't have to deal and parse out all the query arguments
    // the various about pages rely on.
    if (StringBeginsWith(aboutSpec, aUnsafeInlineEntry)) {
      return;
    }
  }

  MOZ_ASSERT(!foundUnsafeInline,
             "about: page must not contain a CSP including 'unsafe-inline'");
}
#endif

/* static */
bool nsContentSecurityUtils::ValidateScriptFilename(JSContext* cx,
                                                    const char* aFilename) {
  static Maybe<bool> sGeneralConfigFilenameSet;
  // If the pref is permissive, allow everything
  if (StaticPrefs::security_allow_parent_unrestricted_js_loads()) {
    return true;
  }

  // If we're not in the parent process allow everything (presently)
  if (!XRE_IsE10sParentProcess()) {
    return true;
  }

  // We only perform a check of this preference on the Main Thread
  // (because a String-based preference check is only safe on Main Thread.)
  // The consequence of this is that if a user is using userChromeJS _and_
  // the scripts they use start a worker - we will enter this function,
  // skip over this pref check that would normally cause us to allow the
  // load - and we will block it.
  // While not ideal, we do not officially support userChromeJS, and hopefully
  // the usage of workers is even lower than userChromeJS usage.
  if (NS_IsMainThread()) {
    // This preference is a file used for autoconfiguration of Firefox
    // by administrators. It will also run in the parent process and throw
    // assumptions about what can run where out of the window.
    if (!sGeneralConfigFilenameSet.isSome()) {
      nsAutoString jsConfigPref;
      Preferences::GetString("general.config.filename", jsConfigPref);
      sGeneralConfigFilenameSet.emplace(!jsConfigPref.IsEmpty());
    }
    if (sGeneralConfigFilenameSet.value()) {
      MOZ_LOG(sCSMLog, LogLevel::Debug,
              ("Allowing a javascript load of %s because "
               "general.config.filename is set",
               aFilename));
      return true;
    }
  }

  if (XRE_IsE10sParentProcess() &&
      !StaticPrefs::extensions_webextensions_remote()) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing a javascript load of %s because the web extension "
             "process is disabled.",
             aFilename));
    return true;
  }

  NS_ConvertUTF8toUTF16 filenameU(aFilename);
  if (StringBeginsWith(filenameU, NS_LITERAL_STRING("chrome://"))) {
    // If it's a chrome:// url, allow it
    return true;
  }
  if (StringBeginsWith(filenameU, NS_LITERAL_STRING("resource://"))) {
    // If it's a resource:// url, allow it
    return true;
  }
  if (StringBeginsWith(filenameU, NS_LITERAL_STRING("file://"))) {
    // We will temporarily allow all file:// URIs through for now
    return true;
  }
  if (StringBeginsWith(filenameU, NS_LITERAL_STRING("jar:file://"))) {
    // We will temporarily allow all jar URIs through for now
    return true;
  }

  // Log to MOZ_LOG
  MOZ_LOG(sCSMLog, LogLevel::Info,
          ("ValidateScriptFilename Failed: %s\n", aFilename));

  // If we got here we are going to return false, so set the error context
  const char* utf8Filename;
  if (mozilla::IsUtf8(mozilla::MakeStringSpan(aFilename))) {
    utf8Filename = aFilename;
  } else {
    utf8Filename = "(invalid UTF-8 filename)";
  }
  JS_ReportErrorNumberUTF8(cx, js::GetErrorMessage, nullptr,
                           JSMSG_UNSAFE_FILENAME, utf8Filename);

  // Presently we are not enforcing any restrictions for the script filename,
  // we're only reporting Telemetry. In the future we will assert in debug
  // builds and return false to prevent execution in non-debug builds.
  return true;
}