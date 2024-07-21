/* import-globals-from ../shared-head.js */

Services.scriptloader.loadSubScript(
  "chrome://mochitests/content/browser/tools/profiler/tests/browser/shared-head.js",
  this
);

const { BrowserTestUtils } = ChromeUtils.import(
  "resource://testing-common/BrowserTestUtils.jsm"
);

const BASE_URL = "http://example.com/browser/tools/profiler/tests/browser/";

