/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

"use strict";

/**
 * Initialize the require function through a BrowserLoader. This loader ensures
 * that the popup can use require and has access to the window object.
 */
const { BrowserLoader } = ChromeUtils.import(
  "resource://devtools/client/shared/browser-loader.js"
);
const { require } = BrowserLoader({
  baseURI: "resource://devtools/client/performance-new/popup/",
  window,
});

/**
 * The background.jsm manages the profiler state, and can be loaded multiple time
 * for various components. This pop-up needs a copy, and it is also used by the
 * profiler shortcuts. In order to do this, the background code needs to live in a
 * JSM module, that can be shared with the DevTools keyboard shortcut manager.
 */
const {
  getRecordingPreferencesFromBrowser,
  setRecordingPreferencesOnBrowser,
  getSymbolsFromThisBrowser,
} = ChromeUtils.import(
  "resource://devtools/client/performance-new/popup/background.jsm"
);

const { receiveProfile } = require("devtools/client/performance-new/browser");

const Perf = require("devtools/client/performance-new/components/Perf");
const ReactDOM = require("devtools/client/shared/vendor/react-dom");
const React = require("devtools/client/shared/vendor/react");
const createStore = require("devtools/client/shared/redux/create-store");
const reducers = require("devtools/client/performance-new/store/reducers");
const actions = require("devtools/client/performance-new/store/actions");
const { Provider } = require("devtools/client/shared/vendor/react-redux");
const {
  ActorReadyGeckoProfilerInterface,
} = require("devtools/shared/performance-new/gecko-profiler-interface");

/* Force one of our two themes depending on what theme the browser is
 * currently using. This might be different from the selected theme in
 * the devtools panel. By forcing a theme here, we're unaffected by
 * the devtools setting when we show the popup.
 */
document.documentElement.setAttribute(
  "force-theme",
  window.gIsDarkMode ? "dark" : "light"
);

document.addEventListener("DOMContentLoaded", () => {
  gInit();
});

async function gInit() {
  const store = createStore(reducers);
  const perfFrontInterface = new ActorReadyGeckoProfilerInterface();
  const supportedFeatures = await perfFrontInterface.getSupportedFeatures();

  // Do some initialization, especially with privileged things that are part of the
  // the browser.
  store.dispatch(
    actions.initializeStore({
      perfFront: perfFrontInterface,
      receiveProfile,
      supportedFeatures,
      // Get the preferences from the current browser
      recordingPreferences: getRecordingPreferencesFromBrowser(),
      // In the popup, the preferences are stored directly on the current browser.
      setRecordingPreferences: setRecordingPreferencesOnBrowser,
      // The popup doesn't need to support remote symbol tables from the debuggee.
      // Only get the symbols from this browser.
      getSymbolTableGetter: () => getSymbolsFromThisBrowser,
      isPopup: true,
    })
  );

  ReactDOM.render(
    React.createElement(Provider, { store }, React.createElement(Perf)),
    document.querySelector("#root")
  );

  window.addEventListener("unload", function() {
    // The perf front interface needs to be unloaded in order to remove event handlers.
    // Not doing so leads to leaks.
    perfFrontInterface.destroy();
  });

  resizeWindow();
}

function resizeWindow() {
  window.requestAnimationFrame(() => {
    if (window.gResizePopup) {
      window.gResizePopup(document.body.clientHeight);
    }
  });
}
