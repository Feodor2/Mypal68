/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A namespace class for static content security utilities. */

#include "nsContentSecurityUtils.h"

#include "nsIContentSecurityPolicy.h"
#include "nsIURI.h"

#include "mozilla/dom/Document.h"

/* static */
bool nsContentSecurityUtils::IsEvalAllowed(JSContext* cx,
                                           nsIPrincipal* aSubjectPrincipal,
                                           const nsAString& aScript) {
  // This allowlist contains files that are permanently allowed to use
  // eval()-like functions. It is supposed to be restricted to files that are
  // exclusively used in testing contexts.
  static nsLiteralCString evalAllowlist[] = {
      // Test-only third-party library
      NS_LITERAL_CSTRING("resource://testing-common/sinon-7.2.7.js"),
      // Test-only third-party library
      NS_LITERAL_CSTRING("resource://testing-common/ajv-4.1.1.js"),
      // Test-only utility
      NS_LITERAL_CSTRING("resource://testing-common/content-task.js"),

      // The Browser Toolbox/Console
      NS_LITERAL_CSTRING("debugger"),
  };

  // We also permit two specific idioms in eval()-like contexts. We'd like to
  // elminate these too; but there are in-the-wild Mozilla privileged extensions
  // that use them.
  static NS_NAMED_LITERAL_STRING(sAllowedEval1, "this");
  static NS_NAMED_LITERAL_STRING(sAllowedEval2,
                                 "function anonymous(\n) {\nreturn this\n}");

  if (!aSubjectPrincipal->IsSystemPrincipal()) {
    return true;
  }

  // Use static pref for performance reasons.
  if (StaticPrefs::security_allow_eval_with_system_principal()) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() with SystemPrincipal because allowing pref is "
             "enabled"));
    return true;
  }

  // This preference is a file used for autoconfiguration of Firefox
  // by administrators. It has also been (ab)used by the userChromeJS
  // project to run legacy-style 'extensions', some of which use eval,
  // all of which run in the System Principal context.
  nsAutoString jsConfigPref;
  Preferences::GetString("general.config.filename", jsConfigPref);
  if (!jsConfigPref.IsEmpty()) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() with SystemPrincipal because of "
             "general.config.filename"));
    return true;
  }

  // This preference is better known as userchrome.css which allows
  // customization of the Firefox UI. Believe it or not, you can also
  // use XBL bindings to get it to run Javascript in the same manner
  // as userChromeJS above, so even though 99.9% of people using
  // userchrome.css aren't doing that, we're still going to need to
  // disable the eval() assertion for them.
  if (Preferences::GetBool(
          "toolkit.legacyUserProfileCustomizations.stylesheets")) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() with SystemPrincipal because of "
             "general.config.filename"));
    return true;
  }

  // We permit these two common idioms to get access to the global JS object
  if (!aScript.IsEmpty() &&
      (aScript == sAllowedEval1 || aScript == sAllowedEval2)) {
    MOZ_LOG(sCSMLog, LogLevel::Debug,
            ("Allowing eval() with SystemPrincipal because a key string is "
             "provided"));
    return true;
  }

  nsAutoCString fileName;
  uint32_t lineNumber = 0, columnNumber = 0;
  JS::AutoFilename rawScriptFilename;
  if (JS::DescribeScriptedCaller(cx, &rawScriptFilename, &lineNumber,
                                 &columnNumber)) {
    nsDependentCSubstring fileName_(rawScriptFilename.get(),
                                    strlen(rawScriptFilename.get()));
    ToLowerCase(fileName_);
    // Extract file name alone if scriptFilename contains line number
    // separated by multiple space delimiters in few cases.
    int32_t fileNameIndex = fileName_.FindChar(' ');
    if (fileNameIndex != -1) {
      fileName_.SetLength(fileNameIndex);
    }
    fileName = std::move(fileName_);
  } else {
    fileName = NS_LITERAL_CSTRING("unknown-file");
  }

  NS_ConvertUTF8toUTF16 fileNameA(fileName);
  for (const nsLiteralCString& allowlistEntry : evalAllowlist) {
    if (fileName.Equals(allowlistEntry)) {
      MOZ_LOG(
          sCSMLog, LogLevel::Debug,
          ("Allowing eval() with SystemPrincipal because the containing "
           "file is in the allowlist"));
      return true;
    }
  }

  // Log to MOZ_LOG
  MOZ_LOG(sCSMLog, LogLevel::Warning,
          ("Blocking eval() with SystemPrincipal from file %s and script "
           "provided %s",
           fileName.get(), NS_ConvertUTF16toUTF8(aScript).get()));


  // Report an error to console
  nsCOMPtr<nsIConsoleService> console(
      do_GetService(NS_CONSOLESERVICE_CONTRACTID));
  if (!console) {
    return false;
  }
  nsCOMPtr<nsIScriptError> error(do_CreateInstance(NS_SCRIPTERROR_CONTRACTID));
  if (!error) {
    return false;
  }
  nsCOMPtr<nsIStringBundle> bundle;
  nsCOMPtr<nsIStringBundleService> stringService =
      mozilla::services::GetStringBundleService();
  if (!stringService) {
    return false;
  }
  stringService->CreateBundle(
      "chrome://global/locale/security/security.properties",
      getter_AddRefs(bundle));
  if (!bundle) {
    return false;
  }
  nsAutoString message;
  AutoTArray<nsString, 1> formatStrings = {fileNameA};
  nsresult rv = bundle->FormatStringFromName("RestrictBrowserEvalUsage",
                                             formatStrings, message);
  if (NS_FAILED(rv)) {
    return false;
  }

  uint64_t windowID = nsJSUtils::GetCurrentlyRunningCodeInnerWindowID(cx);
  rv = error->InitWithWindowID(message, fileNameA, EmptyString(), lineNumber,
                               columnNumber, nsIScriptError::errorFlag,
                               "BrowserEvalUsage", windowID,
                               true /* From chrome context */);
  if (NS_FAILED(rv)) {
    return false;
  }
  console->LogMessage(error);

  // Maybe Crash
#ifdef DEBUG
  MOZ_CRASH_UNSAFE_PRINTF(
      "Blocking eval() with SystemPrincipal from file %s and script provided "
      "%s",
      fileName.get(), NS_ConvertUTF16toUTF8(aScript).get());
#endif

  return false;
}

#if defined(DEBUG) && !defined(ANDROID)
/* static */
void nsContentSecurityUtils::AssertAboutPageHasCSP(Document* aDocument) {
  // We want to get to a point where all about: pages ship with a CSP. This
  // assertion ensures that we can not deploy new about: pages without a CSP.
  // Initially we will whitelist legacy about: pages which not yet have a CSP
  // attached, but ultimately that whitelist should disappear.
  // Please note that any about: page should not use inline JS or inline CSS,
  // and instead should load JS and CSS from an external file (*.js, *.css)
  // which allows us to apply a strong CSP omitting 'unsafe-inline'. Ideally,
  // the CSP allows precisely the resources that need to be loaded; but it
  // should at least be as strong as:
  // <meta http-equiv="Content-Security-Policy" content="default-src chrome:"/>

  // Check if we are loading an about: URI at all
  nsCOMPtr<nsIURI> documentURI = aDocument->GetDocumentURI();
  if (!documentURI->SchemeIs("about") ||
      Preferences::GetBool("csp.skip_about_page_has_csp_assert")) {
    return;
  }

  // Potentially init the legacy whitelist of about URIs without a CSP.
  static StaticAutoPtr<nsTArray<nsCString>> sLegacyAboutPagesWithNoCSP;
  if (!sLegacyAboutPagesWithNoCSP ||
      Preferences::GetBool("csp.overrule_about_uris_without_csp_whitelist")) {
    sLegacyAboutPagesWithNoCSP = new nsTArray<nsCString>();
    nsAutoCString legacyAboutPages;
    Preferences::GetCString("csp.about_uris_without_csp", legacyAboutPages);
    for (const nsACString& hostString : legacyAboutPages.Split(',')) {
      // please note that for the actual whitelist we only store the path of
      // about: URI. Let's reassemble the full about URI here so we don't
      // have to remove query arguments later.
      nsCString aboutURI;
      aboutURI.AppendLiteral("about:");
      aboutURI.Append(hostString);
      sLegacyAboutPagesWithNoCSP->AppendElement(aboutURI);
    }
    ClearOnShutdown(&sLegacyAboutPagesWithNoCSP);
  }

  // Check if the about URI is whitelisted
  nsAutoCString aboutSpec;
  documentURI->GetSpec(aboutSpec);
  ToLowerCase(aboutSpec);
  for (auto& legacyPageEntry : *sLegacyAboutPagesWithNoCSP) {
    // please note that we perform a substring match here on purpose,
    // so we don't have to deal and parse out all the query arguments
    // the various about pages rely on.
    if (aboutSpec.Find(legacyPageEntry) == 0) {
      return;
    }
  }

  nsCOMPtr<nsIContentSecurityPolicy> csp = aDocument->GetCsp();
  bool foundDefaultSrc = false;
  if (csp) {
    uint32_t policyCount = 0;
    csp->GetPolicyCount(&policyCount);
    nsAutoString parsedPolicyStr;
    for (uint32_t i = 0; i < policyCount; ++i) {
      csp->GetPolicyString(i, parsedPolicyStr);
      if (parsedPolicyStr.Find("default-src") >= 0) {
        foundDefaultSrc = true;
        break;
      }
    }
  }
  if (Preferences::GetBool("csp.overrule_about_uris_without_csp_whitelist")) {
    NS_ASSERTION(foundDefaultSrc, "about: page must have a CSP");
    return;
  }
  MOZ_ASSERT(foundDefaultSrc,
             "about: page must contain a CSP including default-src");
}
#endif
