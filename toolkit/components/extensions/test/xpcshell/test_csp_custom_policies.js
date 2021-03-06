"use strict";

const { Preferences } = ChromeUtils.import(
  "resource://gre/modules/Preferences.jsm"
);

const ADDON_ID = "test@web.extension";

const aps = Cc["@mozilla.org/addons/policy-service;1"].getService(
  Ci.nsIAddonPolicyService
);

let policy = null;

function setAddonCSP(csp) {
  if (policy) {
    policy.active = false;
  }

  policy = new WebExtensionPolicy({
    id: ADDON_ID,
    mozExtensionHostname: ADDON_ID,
    baseURL: "file:///",

    allowedOrigins: new MatchPatternSet([]),
    localizeCallback() {},

    contentSecurityPolicy: csp,
  });

  policy.active = true;
}

registerCleanupFunction(() => {
  policy.active = false;
});

add_task(async function test_addon_csp() {
  equal(
    aps.baseCSP,
    Preferences.get("extensions.webextensions.base-content-security-policy"),
    "Expected base CSP value"
  );

  equal(
    aps.defaultCSP,
    Preferences.get("extensions.webextensions.default-content-security-policy"),
    "Expected default CSP value"
  );

  const CUSTOM_POLICY =
    "script-src: 'self' https://xpcshell.test.custom.csp; object-src: 'none'";

  setAddonCSP(CUSTOM_POLICY);

  equal(
    aps.getAddonCSP(ADDON_ID),
    CUSTOM_POLICY,
    "CSP should point to add-on's custom policy"
  );

  setAddonCSP(null);

  equal(
    aps.getAddonCSP(ADDON_ID),
    aps.defaultCSP,
    "CSP should revert to default when set to null"
  );
});
