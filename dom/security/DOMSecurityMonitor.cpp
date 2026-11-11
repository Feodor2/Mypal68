/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "DOMSecurityMonitor.h"
#include "nsContentUtils.h"

#include "mozilla/Preferences.h"
#include "nsIPrincipal.h"
#include "nsIURI.h"

#include "mozilla/StaticPrefs_dom.h"

/* static */
void DOMSecurityMonitor::AuditParsingOfHTMLXMLFragments(
    nsIPrincipal* aPrincipal, const nsAString& aFragment) {
  // if the fragment parser (e.g. innerHTML()) is not called in chrome: code
  // or any of our about: pages, then there is nothing to do here.
  if (!aPrincipal->IsSystemPrincipal() && !aPrincipal->SchemeIs("about")) {
    return;
  }

  // check if the fragment is empty, if so, we can return early.
  if (aFragment.IsEmpty()) {
    return;
  }

  // check if there is a JS caller, if not, then we can can return early here
  // because we only care about calls to the fragment parser (e.g. innerHTML)
  // originating from JS code.
  nsAutoString filename;
  uint32_t lineNum = 0;
  uint32_t columnNum = 0;
  JSContext* cx = nsContentUtils::GetCurrentJSContext();
  if (!cx ||
      !nsJSUtils::GetCallingLocation(cx, filename, &lineNum, &columnNum)) {
    return;
  }

  // check if we should skip assertion. Please only ever set this pref to
  // true if really needed for testing purposes.
#ifdef DEBUG
  if (StaticPrefs::dom_security_skip_html_fragment_assertion()) {
    return;
  }
#endif

  /*
   * WARNING: Do not add any new entries to the htmlFragmentAllowlist
   * withiout proper review from a dom:security peer!
   */
  static nsLiteralCString htmlFragmentAllowlist[] = {
      "chrome://global/content/elements/marquee.js"_ns,
      nsLiteralCString(
          "chrome://pocket/content/panels/js/vendor/jquery-2.1.1.min.js"),
      "chrome://browser/content/aboutNetError.js"_ns,
      nsLiteralCString("chrome://devtools/content/shared/sourceeditor/"
                       "codemirror/codemirror.bundle.js"),
      nsLiteralCString(
          "chrome://devtools-startup/content/aboutdevtools/aboutdevtools.js"),
      nsLiteralCString(
          "resource://activity-stream/data/content/activity-stream.bundle.js"),
      nsLiteralCString("resource://devtools/client/debugger/src/components/"
                       "Editor/Breakpoint.js"),
      nsLiteralCString("resource://devtools/client/debugger/src/components/"
                       "Editor/ColumnBreakpoint.js"),
      nsLiteralCString(
          "resource://devtools/client/shared/vendor/fluent-react.js"),
      "resource://devtools/client/shared/vendor/react-dom.js"_ns,
      nsLiteralCString(
          "resource://devtools/client/shared/vendor/react-dom-dev.js"),
      nsLiteralCString(
          "resource://devtools/client/shared/widgets/FilterWidget.js"),
      nsLiteralCString("resource://devtools/client/shared/widgets/tooltip/"
                       "inactive-css-tooltip-helper.js"),
      "resource://devtools/client/shared/widgets/Spectrum.js"_ns,
      "resource://gre/modules/narrate/VoiceSelect.jsm"_ns,
      "resource://normandy-vendor/ReactDOM.js"_ns,
      // ------------------------------------------------------------------
      // test pages
      // ------------------------------------------------------------------
      "chrome://mochikit/content/harness.xhtml"_ns,
      "chrome://mochikit/content/tests/"_ns,
      "chrome://mochitests/content/"_ns,
      "chrome://reftest/content/"_ns,
  };

  for (const nsLiteralCString& allowlistEntry : htmlFragmentAllowlist) {
    if (StringBeginsWith(NS_ConvertUTF16toUTF8(filename), allowlistEntry)) {
      return;
    }
  }

  nsAutoCString uriSpec;
  aPrincipal->GetAsciiSpec(uriSpec);

  // Ideally we should not call the fragment parser (e.g. innerHTML()) in
  // chrome: code or any of our about: pages. If you hit that assertion,
  // please do *not* add your filename to the allowlist above, but rather
  // refactor your code.
  fprintf(stderr,
          "Do not call the fragment parser (e.g innerHTML()) in chrome code "
          "or in about: pages, (uri: %s), (caller: %s, line: %d, col: %d), "
          "(fragment: %s)",
          uriSpec.get(), NS_ConvertUTF16toUTF8(filename).get(), lineNum,
          columnNum, NS_ConvertUTF16toUTF8(aFragment).get());
  MOZ_ASSERT(false);
}
