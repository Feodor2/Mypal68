Services.prefs.setBoolPref("dom.serviceWorkers.enabled", true);
Services.prefs.setBoolPref("dom.push.enabled", true);
ChromeUtils.import("resource:///modules/SitePermissions.jsm", this);
const { PermissionTestUtils } = ChromeUtils.import(
  "resource://testing-common/PermissionTestUtils.jsm"
);
