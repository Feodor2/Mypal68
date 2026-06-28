"use strict";

const { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");

const { XPCOMUtils } = ChromeUtils.import(
  "resource://gre/modules/XPCOMUtils.jsm"
);

const { MockRegistrar } = ChromeUtils.import(
  "resource://testing-common/MockRegistrar.jsm"
);

XPCOMUtils.defineLazyModuleGetters(this, {
  AddonManager: "resource://gre/modules/AddonManager.jsm",
  AddonTestUtils: "resource://testing-common/AddonTestUtils.jsm",
  ExtensionTestUtils: "resource://testing-common/ExtensionXPCShellUtils.jsm",
});

var EXPORTED_SYMBOLS = ["SearchTestUtils"];

var gTestGlobals;

var SearchTestUtils = Object.freeze({
  init(Assert, registerCleanupFunction) {
    gTestGlobals = {
      Assert,
      registerCleanupFunction,
    };
  },

  /**
   * Adds a search engine to the search service. It will remove the engine
   * at the end of the test.
   *
   * @param {string}   url                     The URL of the engine to add.
   * @param {Function} registerCleanupFunction Pass the registerCleanupFunction
   *                                           from the test's scope.
   * @returns {Promise} Returns a promise that is resolved with the new engine
   *                    or rejected if it fails.
   */
  async promiseNewSearchEngine(url) {
    let engine = await Services.search.addOpenSearchEngine(url, "");
    gTestGlobals.registerCleanupFunction(async () =>
      Services.search.removeEngine(engine)
    );
    return engine;
  },

  /**
   * Returns a promise that is resolved when an observer notification from the
   * search service fires with the specified data.
   *
   * @param {*} expectedData
   *        The value the observer notification sends that causes us to resolve
   *        the promise.
   * @param {string} topic
   *        The notification topic to observe. Defaults to 'browser-search-service'.
   * @returns {Promise}
   *        Returns a promise that is resolved with the subject of the
   *        topic once the topic with the data has been observed.
   */
  promiseSearchNotification(expectedData, topic = "browser-search-service") {
    return new Promise(resolve => {
      Services.obs.addObserver(function observer(aSubject, aTopic, aData) {
        if (aData != expectedData) {
          return;
        }

        Services.obs.removeObserver(observer, topic);
        resolve(aSubject);
      }, topic);
    });
  },

  /**
   * Provides various setup for xpcshell-tests installing WebExtensions. Should
   * be called from the global scope of the test.
   *
   * @param {object} scope
   *  The global scope of the test being run.
   * @param {*} usePrivilegedSignatures
   *  How to sign created addons.
   */
  initXPCShellAddonManager(scope, usePrivilegedSignatures = false) {
    let scopes = AddonManager.SCOPE_PROFILE | AddonManager.SCOPE_APPLICATION;
    Services.prefs.setIntPref("extensions.enabledScopes", scopes);
    Services.prefs.setBoolPref(
      "extensions.webextensions.background-delayed-startup",
      false
    );
    ExtensionTestUtils.init(scope);
    AddonTestUtils.usePrivilegedSignatures = usePrivilegedSignatures;
    AddonTestUtils.overrideCertDB();
  },

  /**
   * Add a search engine as a WebExtension. For xpcshell-tests only.
   *
   * Note: You should call `initXPCShellAddonManager` before calling this.
   *
   * @param {object} [options]
   */
  async installSearchExtension(options = {}) {
    options.id = (options.id ?? "example") + "@tests.mozilla.org";
    let extensionInfo = {
      useAddonManager: "permanent",
      manifest: this.createEngineManifest(options),
    };

    let extension = ExtensionTestUtils.loadExtension(extensionInfo);
    await extension.startup();
    await AddonTestUtils.waitForSearchProviderStartup(extension);
    return extension;
  },

  /**
   * Create a search engine extension manifest.
   *
   * @param {object} [options]
   * @param {string} [options.id]
   *   The id to use for the WebExtension.
   * @param {string} [options.name]
   *   The display name to use for the WebExtension.
   * @param {string} [options.version]
   *   The version to use for the WebExtension.
   * @param {string} [options.keyword]
   *   The keyword to use for the WebExtension.
   * @returns {object}
   *   The generated manifest.
   */
  createEngineManifest(options = {}) {
    options.id = options.id ?? "example@tests.mozilla.org";
    options.name = options.name ?? "Example";
    options.version = options.version ?? "1.0";
    let manifest = {
      version: options.version,
      applications: {
        gecko: {
          id: options.id,
        },
      },
      chrome_settings_overrides: {
        search_provider: {
          name: options.name,
          search_url: "https://example.com/",
          search_url_get_params: "?q={searchTerms}",
        },
      },
    };
    if (options.keyword) {
      manifest.chrome_settings_overrides.search_provider.keyword =
        options.keyword;
    }
    return manifest;
  },

  },
});
