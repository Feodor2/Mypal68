/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

/* import-globals-from extensionControlled.js */
/* import-globals-from preferences.js */

var { AppConstants } = ChromeUtils.import(
  "resource://gre/modules/AppConstants.jsm"
);

ChromeUtils.defineModuleGetter(
  this,
  "DownloadUtils",
  "resource://gre/modules/DownloadUtils.jsm"
);
ChromeUtils.defineModuleGetter(
  this,
  "LoginHelper",
  "resource://gre/modules/LoginHelper.jsm"
);
ChromeUtils.defineModuleGetter(
  this,
  "SiteDataManager",
  "resource:///modules/SiteDataManager.jsm"
);

var { PrivateBrowsingUtils } = ChromeUtils.import(
  "resource://gre/modules/PrivateBrowsingUtils.jsm"
);

const PREF_PASSWORD_GENERATION_AVAILABLE = "signon.generation.available";

XPCOMUtils.defineLazyGetter(this, "AlertsServiceDND", function() {
  try {
    let alertsService = Cc["@mozilla.org/alerts-service;1"]
      .getService(Ci.nsIAlertsService)
      .QueryInterface(Ci.nsIAlertsDoNotDisturb);
    // This will throw if manualDoNotDisturb isn't implemented.
    alertsService.manualDoNotDisturb;
    return alertsService;
  } catch (ex) {
    return undefined;
  }
});

XPCOMUtils.defineLazyServiceGetter(
  this,
  "listManager",
  "@mozilla.org/url-classifier/listmanager;1",
  "nsIUrlListManager"
);

Preferences.addAll([
  // Button prefs
  { id: "pref.privacy.disable_button.cookie_exceptions", type: "bool" },
  { id: "pref.privacy.disable_button.view_cookies", type: "bool" },
  { id: "pref.privacy.disable_button.change_blocklist", type: "bool" },

  // Location Bar
  { id: "browser.urlbar.suggest.bookmark", type: "bool" },
  { id: "browser.urlbar.suggest.history", type: "bool" },
  { id: "browser.urlbar.suggest.openpage", type: "bool" },

  // History
  { id: "places.history.enabled", type: "bool" },
  { id: "browser.formfill.enable", type: "bool" },
  { id: "privacy.history.custom", type: "bool" },
  // Cookies
  { id: "network.cookie.cookieBehavior", type: "int" },
  { id: "network.cookie.lifetimePolicy", type: "int" },
  { id: "network.cookie.blockFutureCookies", type: "bool" },

  // Clear Private Data
  { id: "privacy.sanitize.sanitizeOnShutdown", type: "bool" },
  { id: "privacy.sanitize.timeSpan", type: "int" },
  // Do not track
  { id: "privacy.donottrackheader.enabled", type: "bool" },

  // Media
  { id: "media.autoplay.default", type: "int" },

  // Popups
  { id: "dom.disable_open_during_load", type: "bool" },
  // Passwords
  { id: "signon.rememberSignons", type: "bool" },
  { id: "signon.generation.enabled", type: "bool" },
  { id: "signon.autofillForms", type: "bool" },

  // Buttons
  { id: "pref.privacy.disable_button.view_passwords", type: "bool" },
  { id: "pref.privacy.disable_button.view_passwords_exceptions", type: "bool" },

  /* Certificates tab
   * security.default_personal_cert
   *   - a string:
   *       "Select Automatically"   select a certificate automatically when a site
   *                                requests one
   *       "Ask Every Time"         present a dialog to the user so he can select
   *                                the certificate to use on a site which
   *                                requests one
   */
  { id: "security.default_personal_cert", type: "string" },

  { id: "security.disable_button.openCertManager", type: "bool" },

  { id: "security.disable_button.openDeviceManager", type: "bool" },

  { id: "security.OCSP.enabled", type: "int" },
  { id: "xpinstall.whitelist.required", type: "bool" },
]);

function setEventListener(aId, aEventType, aCallback) {
  document
    .getElementById(aId)
    .addEventListener(aEventType, aCallback.bind(gPrivacyPane));
}

function setSyncFromPrefListener(aId, aCallback) {
  Preferences.addSyncFromPrefListener(document.getElementById(aId), aCallback);
}

function setSyncToPrefListener(aId, aCallback) {
  Preferences.addSyncToPrefListener(document.getElementById(aId), aCallback);
}

var gPrivacyPane = {
  _pane: null,

  /**
   * Whether the prompt to restart Firefox should appear when changing the autostart pref.
   */
  _shouldPromptForRestart: true,

  /**
   * Initialize autocomplete to ensure prefs are in sync.
   */
  _initAutocomplete() {
    Cc["@mozilla.org/autocomplete/search;1?name=unifiedcomplete"].getService(
      Ci.mozIPlacesAutoComplete
    );
  },

  /**
   * Sets up the UI for the number of days of history to keep, and updates the
   * label of the "Clear Now..." button.
   */
  init() {
    this._updateSanitizeSettingsButton();
    this.initializeHistoryMode();
    this.updateHistoryModePane();
    this.updatePrivacyMicroControls();
    this.initAutoStartPrivateBrowsingReverter();
    this._initAutocomplete();

    this.networkCookieBehaviorReadPrefs();
    // Watch all of the prefs that the new Cookies & Site Data UI depends on
    Preferences.get("network.cookie.cookieBehavior").on(
      "change",
      gPrivacyPane.networkCookieBehaviorReadPrefs.bind(gPrivacyPane)
    );
    Preferences.get("network.cookie.lifetimePolicy").on(
      "change",
      gPrivacyPane.networkCookieBehaviorReadPrefs.bind(gPrivacyPane)
    );
    Preferences.get("browser.privatebrowsing.autostart").on(
      "change",
      gPrivacyPane.networkCookieBehaviorReadPrefs.bind(gPrivacyPane)
    );

    setEventListener("a11yPrivacyCheckbox", "command", ev => {
      this.updateA11yPrefs(ev.target.checked);
    });

    Preferences.get("privacy.sanitize.sanitizeOnShutdown").on(
      "change",
      gPrivacyPane._updateSanitizeSettingsButton.bind(gPrivacyPane)
    );
    Preferences.get("browser.privatebrowsing.autostart").on(
      "change",
      gPrivacyPane.updatePrivacyMicroControls.bind(gPrivacyPane)
    );
    setEventListener("historyMode", "command", function() {
      gPrivacyPane.updateHistoryModePane();
      gPrivacyPane.updateHistoryModePrefs();
      gPrivacyPane.updatePrivacyMicroControls();
      gPrivacyPane.updateAutostart();
    });
    setEventListener("clearHistoryButton", "command", function() {
      let historyMode = document.getElementById("historyMode");
      // Select "everything" in the clear history dialog if the
      // user has set their history mode to never remember history.
      gPrivacyPane.clearPrivateDataNow(historyMode.value == "dontremember");
    });
    setEventListener("openSearchEnginePreferences", "click", function(event) {
      if (event.button == 0) {
        gotoPref("search");
      }
      return false;
    });
    setEventListener(
      "privateBrowsingAutoStart",
      "command",
      gPrivacyPane.updateAutostart
    );
    setEventListener(
      "cookieExceptions",
      "command",
      gPrivacyPane.showCookieExceptions
    );
    setEventListener(
      "clearDataSettings",
      "command",
      gPrivacyPane.showClearPrivateDataSettings
    );
    setEventListener(
      "passwordExceptions",
      "command",
      gPrivacyPane.showPasswordExceptions
    );
    setEventListener(
      "useMasterPassword",
      "command",
      gPrivacyPane.updateMasterPasswordButton
    );
    setEventListener(
      "changeMasterPassword",
      "command",
      gPrivacyPane.changeMasterPassword
    );
    setEventListener("showPasswords", "command", gPrivacyPane.showPasswords);
    setEventListener(
      "addonExceptions",
      "command",
      gPrivacyPane.showAddonExceptions
    );
    setEventListener(
      "viewCertificatesButton",
      "command",
      gPrivacyPane.showCertificates
    );
    setEventListener(
      "viewSecurityDevicesButton",
      "command",
      gPrivacyPane.showSecurityDevices
    );

    this._pane = document.getElementById("panePrivacy");

    this._initPasswordGenerationUI();
    this._initMasterPasswordUI();

    setEventListener(
      "autoplaySettingsButton",
      "command",
      gPrivacyPane.showAutoplayMediaExceptions
    );
    setEventListener(
      "notificationSettingsButton",
      "command",
      gPrivacyPane.showNotificationExceptions
    );
    setEventListener(
      "locationSettingsButton",
      "command",
      gPrivacyPane.showLocationExceptions
    );
    setEventListener(
      "cameraSettingsButton",
      "command",
      gPrivacyPane.showCameraExceptions
    );
    setEventListener(
      "microphoneSettingsButton",
      "command",
      gPrivacyPane.showMicrophoneExceptions
    );
    setEventListener(
      "popupPolicyButton",
      "command",
      gPrivacyPane.showPopupExceptions
    );
    setEventListener(
      "notificationsDoNotDisturb",
      "command",
      gPrivacyPane.toggleDoNotDisturbNotifications
    );

    setSyncFromPrefListener("blockCookiesMenu", () =>
      this.readBlockCookiesFrom()
    );
    setSyncToPrefListener("blockCookiesMenu", () =>
      this.writeBlockCookiesFrom()
    );
    setSyncFromPrefListener("deleteOnClose", () => this.readDeleteOnClose());
    setSyncToPrefListener("deleteOnClose", () => this.writeDeleteOnClose());
    setSyncFromPrefListener("savePasswords", () => this.readSavePasswords());

    let microControlHandler = el =>
      this.ensurePrivacyMicroControlUncheckedWhenDisabled(el);
    setSyncFromPrefListener("rememberHistory", microControlHandler);
    setSyncFromPrefListener("rememberForms", microControlHandler);
    setSyncFromPrefListener("alwaysClear", microControlHandler);

    setSyncFromPrefListener("popupPolicy", () =>
      this.updateButtons("popupPolicyButton", "dom.disable_open_during_load")
    );
    setSyncFromPrefListener("warnAddonInstall", () =>
      this.readWarnAddonInstall()
    );
    setSyncFromPrefListener("enableOCSP", () => this.readEnableOCSP());
    setSyncToPrefListener("enableOCSP", () => this.writeEnableOCSP());

    if (AlertsServiceDND) {
      let notificationsDoNotDisturbBox = document.getElementById(
        "notificationsDoNotDisturbBox"
      );
      notificationsDoNotDisturbBox.removeAttribute("hidden");
      let checkbox = document.getElementById("notificationsDoNotDisturb");
      document.l10n.setAttributes(checkbox, "permissions-notification-pause");
      if (AlertsServiceDND.manualDoNotDisturb) {
        let notificationsDoNotDisturb = document.getElementById(
          "notificationsDoNotDisturb"
        );
        notificationsDoNotDisturb.setAttribute("checked", true);
      }
    }

    this.initSiteDataControls();
    setEventListener(
      "clearSiteDataButton",
      "command",
      gPrivacyPane.clearSiteData
    );
    setEventListener(
      "siteDataSettings",
      "command",
      gPrivacyPane.showSiteDataSettings
    );
    let url =
      Services.urlFormatter.formatURLPref("app.support.baseURL") +
      "storage-permissions";
    document.getElementById("siteDataLearnMoreLink").setAttribute("href", url);

    let notificationInfoURL =
      Services.urlFormatter.formatURLPref("app.support.baseURL") + "push";
    document
      .getElementById("notificationPermissionsLearnMore")
      .setAttribute("href", notificationInfoURL);

    this._initA11yState();
    let signonBundle = document.getElementById("signonBundle");
    let pkiBundle = document.getElementById("pkiBundle");
    appendSearchKeywords("showPasswords", [
      signonBundle.getString("loginsDescriptionAll2"),
    ]);
    appendSearchKeywords("viewSecurityDevicesButton", [
      pkiBundle.getString("enable_fips"),
    ]);

    if (!PrivateBrowsingUtils.enabled) {
      document.getElementById("privateBrowsingAutoStart").hidden = true;
      document.querySelector("menuitem[value='dontremember']").hidden = true;
    }

    // Notify observers that the UI is now ready
    Services.obs.notifyObservers(window, "privacy-pane-loaded");
  },

  initSiteDataControls() {
    Services.obs.addObserver(this, "sitedatamanager:sites-updated");
    Services.obs.addObserver(this, "sitedatamanager:updating-sites");
    let unload = () => {
      window.removeEventListener("unload", unload);
      Services.obs.removeObserver(this, "sitedatamanager:sites-updated");
      Services.obs.removeObserver(this, "sitedatamanager:updating-sites");
    };
    window.addEventListener("unload", unload);
    SiteDataManager.updateSites();
  },

  /**
   * Selects the right items of the new Cookies & Site Data UI.
   */
  networkCookieBehaviorReadPrefs() {
    let behavior = Preferences.get("network.cookie.cookieBehavior").value;
    let blockCookiesMenu = document.getElementById("blockCookiesMenu");
    let deleteOnCloseCheckbox = document.getElementById("deleteOnClose");
    let deleteOnCloseNote = document.getElementById("deleteOnCloseNote");

    let blockCookies = behavior != Ci.nsICookieService.BEHAVIOR_ACCEPT;
    let cookieBehaviorLocked = Services.prefs.prefIsLocked(
      "network.cookie.cookieBehavior"
    );
    blockCookiesMenu.disabled = cookieBehaviorLocked;

    let completelyBlockCookies =
      behavior == Ci.nsICookieService.BEHAVIOR_REJECT;
    let privateBrowsing = Preferences.get("browser.privatebrowsing.autostart")
      .value;
    let cookieExpirationLocked = Services.prefs.prefIsLocked(
      "network.cookie.lifetimePolicy"
    );
    deleteOnCloseCheckbox.disabled =
      privateBrowsing || completelyBlockCookies || cookieExpirationLocked;
    deleteOnCloseNote.hidden = !privateBrowsing;

    switch (behavior) {
      case Ci.nsICookieService.BEHAVIOR_ACCEPT:
        blockCookiesMenu.value = "nothing";
        break;
      case Ci.nsICookieService.BEHAVIOR_REJECT_FOREIGN:
        blockCookiesMenu.value = "all-third-parties";
        break;
      case Ci.nsICookieService.BEHAVIOR_REJECT:
        blockCookiesMenu.value = "always";
        break;
      case Ci.nsICookieService.BEHAVIOR_LIMIT_FOREIGN:
        blockCookiesMenu.value = "unvisited";
        break;
    }
  },

  toggleExpansion(e) {
    let carat = e.target;
    carat.classList.toggle("up");
    carat.closest(".content-blocking-category").classList.toggle("expanded");
    carat.setAttribute(
      "aria-expanded",
      carat.getAttribute("aria-expanded") === "false"
    );
  },

  // HISTORY MODE

  /**
   * The list of preferences which affect the initial history mode settings.
   * If the auto start private browsing mode pref is active, the initial
   * history mode would be set to "Don't remember anything".
   * If ALL of these preferences are set to the values that correspond
   * to keeping some part of history, and the auto-start
   * private browsing mode is not active, the initial history mode would be
   * set to "Remember everything".
   * Otherwise, the initial history mode would be set to "Custom".
   *
   * Extensions adding their own preferences can set values here if needed.
   */
  prefsForKeepingHistory: {
    "places.history.enabled": true, // History is enabled
    "browser.formfill.enable": true, // Form information is saved
    "privacy.sanitize.sanitizeOnShutdown": false, // Private date is NOT cleared on shutdown
  },

  /**
   * The list of control IDs which are dependent on the auto-start private
   * browsing setting, such that in "Custom" mode they would be disabled if
   * the auto-start private browsing checkbox is checked, and enabled otherwise.
   *
   * Extensions adding their own controls can append their IDs to this array if needed.
   */
  dependentControls: [
    "rememberHistory",
    "rememberForms",
    "alwaysClear",
    "clearDataSettings",
  ],

  /**
   * Check whether preferences values are set to keep history
   *
   * @param aPrefs an array of pref names to check for
   * @returns boolean true if all of the prefs are set to keep history,
   *                  false otherwise
   */
  _checkHistoryValues(aPrefs) {
    for (let pref of Object.keys(aPrefs)) {
      if (Preferences.get(pref).value != aPrefs[pref]) {
        return false;
      }
    }
    return true;
  },

  /**
   * Initialize the history mode menulist based on the privacy preferences
   */
  initializeHistoryMode() {
    let mode;
    let getVal = aPref => Preferences.get(aPref).value;

    if (getVal("privacy.history.custom")) {
      mode = "custom";
    } else if (this._checkHistoryValues(this.prefsForKeepingHistory)) {
      if (getVal("browser.privatebrowsing.autostart")) {
        mode = "dontremember";
      } else {
        mode = "remember";
      }
    } else {
      mode = "custom";
    }

    document.getElementById("historyMode").value = mode;
  },

  /**
   * Update the selected pane based on the history mode menulist
   */
  updateHistoryModePane() {
    let selectedIndex = -1;
    switch (document.getElementById("historyMode").value) {
      case "remember":
        selectedIndex = 0;
        break;
      case "dontremember":
        selectedIndex = 1;
        break;
      case "custom":
        selectedIndex = 2;
        break;
    }
    document.getElementById("historyPane").selectedIndex = selectedIndex;
    Preferences.get("privacy.history.custom").value = selectedIndex == 2;
  },

  /**
   * Update the private browsing auto-start pref and the history mode
   * micro-management prefs based on the history mode menulist
   */
  updateHistoryModePrefs() {
    let pref = Preferences.get("browser.privatebrowsing.autostart");
    switch (document.getElementById("historyMode").value) {
      case "remember":
        if (pref.value) {
          pref.value = false;
        }

        // select the remember history option if needed
        Preferences.get("places.history.enabled").value = true;

        // select the remember forms history option
        Preferences.get("browser.formfill.enable").value = true;

        // select the clear on close option
        Preferences.get("privacy.sanitize.sanitizeOnShutdown").value = false;
        break;
      case "dontremember":
        if (!pref.value) {
          pref.value = true;
        }
        break;
    }
  },

  /**
   * Update the privacy micro-management controls based on the
   * value of the private browsing auto-start preference.
   */
  updatePrivacyMicroControls() {
    // Check the "Delete cookies when Firefox is closed" checkbox and disable the setting
    // when we're in auto private mode (or reset it back otherwise).
    document.getElementById("deleteOnClose").checked = this.readDeleteOnClose();

    let clearDataSettings = document.getElementById("clearDataSettings");

    if (document.getElementById("historyMode").value == "custom") {
      let disabled = Preferences.get("browser.privatebrowsing.autostart").value;
      this.dependentControls.forEach(aElement => {
        let control = document.getElementById(aElement);
        let preferenceId = control.getAttribute("preference");
        if (!preferenceId) {
          let dependentControlId = control.getAttribute("control");
          if (dependentControlId) {
            let dependentControl = document.getElementById(dependentControlId);
            preferenceId = dependentControl.getAttribute("preference");
          }
        }

        let preference = preferenceId ? Preferences.get(preferenceId) : {};
        control.disabled = disabled || preference.locked;
        if (control != clearDataSettings) {
          this.ensurePrivacyMicroControlUncheckedWhenDisabled(control);
        }
      });

      clearDataSettings.removeAttribute("hidden");

      if (!disabled) {
        // adjust the Settings button for sanitizeOnShutdown
        this._updateSanitizeSettingsButton();
      }
    } else {
      clearDataSettings.setAttribute("hidden", "true");
    }
  },

  ensurePrivacyMicroControlUncheckedWhenDisabled(el) {
    if (Preferences.get("browser.privatebrowsing.autostart").value) {
      // Set checked to false when called from updatePrivacyMicroControls
      el.checked = false;
      // return false for the onsyncfrompreference case:
      return false;
    }
    return undefined; // tell preferencesBindings to assign the 'right' value.
  },

  // CLEAR PRIVATE DATA

  /*
   * Preferences:
   *
   * privacy.sanitize.sanitizeOnShutdown
   * - true if the user's private data is cleared on startup according to the
   *   Clear Private Data settings, false otherwise
   */

  /**
   * Displays the Clear Private Data settings dialog.
   */
  showClearPrivateDataSettings() {
    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/sanitize.xhtml",
      "resizable=no"
    );
  },

  /**
   * Displays a dialog from which individual parts of private data may be
   * cleared.
   */
  clearPrivateDataNow(aClearEverything) {
    var ts = Preferences.get("privacy.sanitize.timeSpan");
    var timeSpanOrig = ts.value;

    if (aClearEverything) {
      ts.value = 0;
    }

    gSubDialog.open(
      "chrome://browser/content/sanitize.xhtml",
      "resizable=no",
      null,
      () => {
        // reset the timeSpan pref
        if (aClearEverything) {
          ts.value = timeSpanOrig;
        }

        Services.obs.notifyObservers(null, "clear-private-data");
      }
    );
  },

  /**
   * Enables or disables the "Settings..." button depending
   * on the privacy.sanitize.sanitizeOnShutdown preference value
   */
  _updateSanitizeSettingsButton() {
    var settingsButton = document.getElementById("clearDataSettings");
    var sanitizeOnShutdownPref = Preferences.get(
      "privacy.sanitize.sanitizeOnShutdown"
    );

    settingsButton.disabled = !sanitizeOnShutdownPref.value;
  },

  toggleDoNotDisturbNotifications(event) {
    AlertsServiceDND.manualDoNotDisturb = event.target.checked;
  },

  // PRIVATE BROWSING

  /**
   * Initialize the starting state for the auto-start private browsing mode pref reverter.
   */
  initAutoStartPrivateBrowsingReverter() {
    // We determine the mode in initializeHistoryMode, which is guaranteed to have been
    // called before now, so this is up-to-date.
    let mode = document.getElementById("historyMode");
    this._lastMode = mode.selectedIndex;
    // The value of the autostart pref, on the other hand, is gotten from Preferences,
    // which updates the DOM asynchronously, so we can't rely on the DOM. Get it directly
    // from the prefs.
    this._lastCheckState = Preferences.get(
      "browser.privatebrowsing.autostart"
    ).value;
  },

  _lastMode: null,
  _lastCheckState: null,
  async updateAutostart() {
    let mode = document.getElementById("historyMode");
    let autoStart = document.getElementById("privateBrowsingAutoStart");
    let pref = Preferences.get("browser.privatebrowsing.autostart");
    if (
      (mode.value == "custom" && this._lastCheckState == autoStart.checked) ||
      (mode.value == "remember" && !this._lastCheckState) ||
      (mode.value == "dontremember" && this._lastCheckState)
    ) {
      // These are all no-op changes, so we don't need to prompt.
      this._lastMode = mode.selectedIndex;
      this._lastCheckState = autoStart.hasAttribute("checked");
      return;
    }

    if (!this._shouldPromptForRestart) {
      // We're performing a revert. Just let it happen.
      return;
    }

    let buttonIndex = await confirmRestartPrompt(
      autoStart.checked,
      1,
      true,
      false
    );
    if (buttonIndex == CONFIRM_RESTART_PROMPT_RESTART_NOW) {
      pref.value = autoStart.hasAttribute("checked");
      Services.startup.quit(
        Ci.nsIAppStartup.eAttemptQuit | Ci.nsIAppStartup.eRestart
      );
      return;
    }

    this._shouldPromptForRestart = false;

    if (this._lastCheckState) {
      autoStart.checked = "checked";
    } else {
      autoStart.removeAttribute("checked");
    }
    pref.value = autoStart.hasAttribute("checked");
    mode.selectedIndex = this._lastMode;
    mode.doCommand();

    this._shouldPromptForRestart = true;
  },

  // COOKIES AND SITE DATA

  /*
   * Preferences:
   *
   * network.cookie.cookieBehavior
   * - determines how the browser should handle cookies:
   *     0   means enable all cookies
   *     1   means reject all third party cookies
   *     2   means disable all cookies
   *     3   means reject third party cookies unless at least one is already set for the eTLD
   *         see netwerk/cookie/src/CookieService.cpp for details
   * network.cookie.lifetimePolicy
   * - determines how long cookies are stored:
   *     0   means keep cookies until they expire
   *     2   means keep cookies until the browser is closed
   */

  readDeleteOnClose() {
    let privateBrowsing = Preferences.get("browser.privatebrowsing.autostart")
      .value;
    if (privateBrowsing) {
      return true;
    }

    let lifetimePolicy = Preferences.get("network.cookie.lifetimePolicy").value;
    return lifetimePolicy == Ci.nsICookieService.ACCEPT_SESSION;
  },

  writeDeleteOnClose() {
    let checkbox = document.getElementById("deleteOnClose");
    return checkbox.checked
      ? Ci.nsICookieService.ACCEPT_SESSION
      : Ci.nsICookieService.ACCEPT_NORMALLY;
  },

  readBlockCookiesFrom() {
    let pref = Preferences.get("network.cookie.cookieBehavior");
    switch (pref.value) {
      case Ci.nsICookieService.BEHAVIOR_REJECT_FOREIGN:
        return "all-third-parties";
      case Ci.nsICookieService.BEHAVIOR_REJECT:
        return "always";
      case Ci.nsICookieService.BEHAVIOR_LIMIT_FOREIGN:
        return "unvisited";
      case Ci.nsICookieService.BEHAVIOR_ACCEPT:
        return "nothing";
      default:
        return undefined;
    }
  },

  writeBlockCookiesFrom() {
    let block = document.getElementById("blockCookiesMenu").selectedItem;
    switch (block.value) {
      case "nothing":
        return Ci.nsICookieService.BEHAVIOR_ACCEPT;
      case "unvisited":
        return Ci.nsICookieService.BEHAVIOR_LIMIT_FOREIGN;
      case "always":
        return Ci.nsICookieService.BEHAVIOR_REJECT;
      case "all-third-parties":
        return Ci.nsICookieService.BEHAVIOR_REJECT_FOREIGN;
      default:
        return undefined;
    }
  },

  /**
   * Reload all tabs in all windows, except the active Preferences tab.
   */
  reloadAllOtherTabs() {
    let activeWindow = window.BrowserWindowTracker.getTopWindow();
    let selectedPrefTab = activeWindow.gBrowser.selectedTab;
    for (let win of window.BrowserWindowTracker.orderedWindows) {
      let tabbrowser = win.gBrowser;
      let tabsToReload = [...tabbrowser.tabs];
      if (win == activeWindow) {
        tabsToReload = tabsToReload.filter(tab => tab !== selectedPrefTab);
      }
      tabbrowser.reloadTabs(tabsToReload);
    }
    for (let notification of document.querySelectorAll(".reload-tabs")) {
      notification.hidden = true;
    }
  },

  /**
   * If there are more tabs than just the preferences tab, show a warning to the user that
   * they need to reload their tabs to apply the setting.
   */
  maybeNotifyUserToReload() {
    let shouldShow = false;
    if (window.BrowserWindowTracker.orderedWindows.length > 1) {
      shouldShow = true;
    } else {
      let tabbrowser = window.BrowserWindowTracker.getTopWindow().gBrowser;
      if (tabbrowser.tabs.length > 1) {
        shouldShow = true;
      }
    }
    if (shouldShow) {
      for (let notification of document.querySelectorAll(".reload-tabs")) {
        notification.hidden = false;
      }
    }
  },

  /**
   * Displays fine-grained, per-site preferences for cookies.
   */
  showCookieExceptions() {
    var params = {
      blockVisible: true,
      sessionVisible: true,
      allowVisible: true,
      prefilledHost: "",
      permissionType: "cookie",
    };
    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/permissions.xhtml",
      null,
      params
    );
  },

  showSiteDataSettings() {
    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/siteDataSettings.xhtml"
    );
  },

  toggleSiteData(shouldShow) {
    let clearButton = document.getElementById("clearSiteDataButton");
    let settingsButton = document.getElementById("siteDataSettings");
    clearButton.disabled = !shouldShow;
    settingsButton.disabled = !shouldShow;
  },

  showSiteDataLoading() {
    let totalSiteDataSizeLabel = document.getElementById("totalSiteDataSize");
    document.l10n.setAttributes(
      totalSiteDataSizeLabel,
      "sitedata-total-size-calculating"
    );
  },

  updateTotalDataSizeLabel(siteDataUsage) {
    SiteDataManager.getCacheSize().then(function(cacheUsage) {
      let totalSiteDataSizeLabel = document.getElementById("totalSiteDataSize");
      let totalUsage = siteDataUsage + cacheUsage;
      let [value, unit] = DownloadUtils.convertByteUnits(totalUsage);
      document.l10n.setAttributes(
        totalSiteDataSizeLabel,
        "sitedata-total-size",
        {
          value,
          unit,
        }
      );
    });
  },

  clearSiteData() {
    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/clearSiteData.xhtml"
    );
  },

  // GEOLOCATION

  /**
   * Displays the location exceptions dialog where specific site location
   * preferences can be set.
   */
  showLocationExceptions() {
    let params = { permissionType: "geo" };

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/sitePermissions.xhtml",
      "resizable=yes",
      params
    );
  },

  // CAMERA

  /**
   * Displays the camera exceptions dialog where specific site camera
   * preferences can be set.
   */
  showCameraExceptions() {
    let params = { permissionType: "camera" };

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/sitePermissions.xhtml",
      "resizable=yes",
      params
    );
  },

  // MICROPHONE

  /**
   * Displays the microphone exceptions dialog where specific site microphone
   * preferences can be set.
   */
  showMicrophoneExceptions() {
    let params = { permissionType: "microphone" };

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/sitePermissions.xhtml",
      "resizable=yes",
      params
    );
  },

  // NOTIFICATIONS

  /**
   * Displays the notifications exceptions dialog where specific site notification
   * preferences can be set.
   */
  showNotificationExceptions() {
    let params = { permissionType: "desktop-notification" };

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/sitePermissions.xhtml",
      "resizable=yes",
      params
    );
  },

  // MEDIA

  showAutoplayMediaExceptions() {
    var params = { permissionType: "autoplay-media" };

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/sitePermissions.xhtml",
      "resizable=yes",
      params
    );
  },

  // POP-UPS

  /**
   * Displays the popup exceptions dialog where specific site popup preferences
   * can be set.
   */
  showPopupExceptions() {
    var params = {
      blockVisible: false,
      sessionVisible: false,
      allowVisible: true,
      prefilledHost: "",
      permissionType: "popup",
    };

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/permissions.xhtml",
      "resizable=yes",
      params
    );
  },

  // UTILITY FUNCTIONS

  /**
   * Utility function to enable/disable the button specified by aButtonID based
   * on the value of the Boolean preference specified by aPreferenceID.
   */
  updateButtons(aButtonID, aPreferenceID) {
    var button = document.getElementById(aButtonID);
    var preference = Preferences.get(aPreferenceID);
    button.disabled = !preference.value || preference.locked;
    return undefined;
  },

  // BEGIN UI CODE

  /*
   * Preferences:
   *
   * dom.disable_open_during_load
   * - true if popups are blocked by default, false otherwise
   */

  // POP-UPS

  /**
   * Displays a dialog in which the user can view and modify the list of sites
   * where passwords are never saved.
   */
  showPasswordExceptions() {
    var params = {
      blockVisible: true,
      sessionVisible: false,
      allowVisible: false,
      hideStatusColumn: true,
      prefilledHost: "",
      permissionType: "login-saving",
    };

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/permissions.xhtml",
      null,
      params
    );
  },

  /**
   * Initializes master password UI: the "use master password" checkbox, selects
   * the master password button to show, and enables/disables it as necessary.
   * The master password is controlled by various bits of NSS functionality, so
   * the UI for it can't be controlled by the normal preference bindings.
   */
  _initMasterPasswordUI() {
    var noMP = !LoginHelper.isMasterPasswordSet();

    var button = document.getElementById("changeMasterPassword");
    button.disabled = noMP;

    var checkbox = document.getElementById("useMasterPassword");
    checkbox.checked = !noMP;
    checkbox.disabled =
      noMP && !Services.policies.isAllowed("createMasterPassword");
  },

  /**
   * Enables/disables the master password button depending on the state of the
   * "use master password" checkbox, and prompts for master password removal if
   * one is set.
   */
  updateMasterPasswordButton() {
    var checkbox = document.getElementById("useMasterPassword");
    var button = document.getElementById("changeMasterPassword");
    button.disabled = !checkbox.checked;

    // unchecking the checkbox should try to immediately remove the master
    // password, because it's impossible to non-destructively remove the master
    // password used to encrypt all the passwords without providing it (by
    // design), and it would be extremely odd to pop up that dialog when the
    // user closes the prefwindow and saves his settings
    if (!checkbox.checked) {
      this._removeMasterPassword();
    } else {
      this.changeMasterPassword();
    }

    this._initMasterPasswordUI();
  },

  /**
   * Displays the "remove master password" dialog to allow the user to remove
   * the current master password.  When the dialog is dismissed, master password
   * UI is automatically updated.
   */
  _removeMasterPassword() {
    var secmodDB = Cc["@mozilla.org/security/pkcs11moduledb;1"].getService(
      Ci.nsIPKCS11ModuleDB
    );
    if (secmodDB.isFIPSEnabled) {
      let title = document.getElementById("fips-title").textContent;
      let desc = document.getElementById("fips-desc").textContent;
      Services.prompt.alert(window, title, desc);
      this._initMasterPasswordUI();
    } else {
      gSubDialog.open(
        "chrome://mozapps/content/preferences/removemp.xhtml",
        null,
        null,
        this._initMasterPasswordUI.bind(this)
      );
    }
  },

  /**
   * Displays a dialog in which the master password may be changed.
   */
  changeMasterPassword() {
    gSubDialog.open(
      "chrome://mozapps/content/preferences/changemp.xhtml",
      "resizable=no",
      null,
      this._initMasterPasswordUI.bind(this)
    );
  },

  /**
   * Set up the initial state for the password generation UI.
   * It will be hidden unless the .available pref is true
   */
  _initPasswordGenerationUI() {
    // we don't watch the .available pref for runtime changes
    let prefValue = Services.prefs.getBoolPref(
      PREF_PASSWORD_GENERATION_AVAILABLE,
      false
    );
    document.getElementById("generatePasswordsBox").hidden = !prefValue;
  },

  /**
   * Shows the sites where the user has saved passwords and the associated login
   * information.
   */
  showPasswords() {
    if (LoginHelper.managementURI) {
      window.docShell.messageManager.sendAsyncMessage(
        "PasswordManager:OpenPreferences",
        {
          entryPoint: "preferences",
        }
      );
      return;
    }
    gSubDialog.open("chrome://passwordmgr/content/passwordManager.xhtml");
  },

  /**
   * Enables/disables dependent controls related to password saving
   * When password saving is not enabled, we need to also disable the password generation checkbox
   * The Exceptions button is used to configure sites where
   * passwords are never saved. When browser is set to start in Private
   * Browsing mode, the "Remember passwords" UI is useless, so we disable it.
   */
  readSavePasswords() {
    var pref = Preferences.get("signon.rememberSignons");
    var excepts = document.getElementById("passwordExceptions");
    var generatePasswords = document.getElementById("generatePasswords");
    var autofillCheckbox = document.getElementById("passwordAutofillCheckbox");

    if (PrivateBrowsingUtils.permanentPrivateBrowsing) {
      document.getElementById("savePasswords").disabled = true;
      excepts.disabled = true;
      generatePasswords.disabled = true;
      autofillCheckbox.disabled = true;
      return false;
    }
    excepts.disabled = !pref.value;
    generatePasswords.disabled = !pref.value;
    autofillCheckbox.disabled = !pref.value;

    // don't override pref value in UI
    return undefined;
  },

  /**
   * Enables/disables the add-ons Exceptions button depending on whether
   * or not add-on installation warnings are displayed.
   */
  readWarnAddonInstall() {
    var warn = Preferences.get("xpinstall.whitelist.required");
    var exceptions = document.getElementById("addonExceptions");

    exceptions.disabled = !warn.value;

    // don't override the preference value
    return undefined;
  },

  /**
   * Displays the exceptions lists for add-on installation warnings.
   */
  showAddonExceptions() {
    var params = this._addonParams;

    gSubDialog.open(
      "chrome://browser/content/preferences/dialogs/permissions.xhtml",
      null,
      params
    );
  },

  /**
   * Parameters for the add-on install permissions dialog.
   */
  _addonParams: {
    blockVisible: false,
    sessionVisible: false,
    allowVisible: true,
    prefilledHost: "",
    permissionType: "install",
  },

  /**
   * readEnableOCSP is used by the preferences UI to determine whether or not
   * the checkbox for OCSP fetching should be checked (it returns true if it
   * should be checked and false otherwise). The about:config preference
   * "security.OCSP.enabled" is an integer rather than a boolean, so it can't be
   * directly mapped from {true,false} to {checked,unchecked}. The possible
   * values for "security.OCSP.enabled" are:
   * 0: fetching is disabled
   * 1: fetch for all certificates
   * 2: fetch only for EV certificates
   * Hence, if "security.OCSP.enabled" is non-zero, the checkbox should be
   * checked. Otherwise, it should be unchecked.
   */
  readEnableOCSP() {
    var preference = Preferences.get("security.OCSP.enabled");
    // This is the case if the preference is the default value.
    if (preference.value === undefined) {
      return true;
    }
    return preference.value != 0;
  },

  /**
   * writeEnableOCSP is used by the preferences UI to map the checked/unchecked
   * state of the OCSP fetching checkbox to the value that the preference
   * "security.OCSP.enabled" should be set to (it returns that value). See the
   * readEnableOCSP documentation for more background. We unfortunately don't
   * have enough information to map from {true,false} to all possible values for
   * "security.OCSP.enabled", but a reasonable alternative is to map from
   * {true,false} to {<the default value>,0}. That is, if the box is checked,
   * "security.OCSP.enabled" will be set to whatever default it should be, given
   * the platform and channel. If the box is unchecked, the preference will be
   * set to 0. Obviously this won't work if the default is 0, so we will have to
   * revisit this if we ever set it to 0.
   */
  writeEnableOCSP() {
    var checkbox = document.getElementById("enableOCSP");
    var defaults = Services.prefs.getDefaultBranch(null);
    var defaultValue = defaults.getIntPref("security.OCSP.enabled");
    return checkbox.checked ? defaultValue : 0;
  },

  /**
   * Displays the user's certificates and associated options.
   */
  showCertificates() {
    gSubDialog.open("chrome://pippki/content/certManager.xhtml");
  },

  /**
   * Displays a dialog from which the user can manage his security devices.
   */
  showSecurityDevices() {
    gSubDialog.open("chrome://pippki/content/device_manager.xhtml");
  },

  /**
   * Set up or hide the Learn More links for various data collection options
   */
  _setupLearnMoreLink(pref, element) {
    // set up the Learn More link with the correct URL
    let url = Services.urlFormatter.formatURLPref(pref);
    let el = document.getElementById(element);

    if (url) {
      el.setAttribute("href", url);
    } else {
      el.setAttribute("hidden", "true");
    }
  },

  observe(aSubject, aTopic, aData) {
    switch (aTopic) {
      case "sitedatamanager:updating-sites":
        // While updating, we want to disable this section and display loading message until updated
        this.toggleSiteData(false);
        this.showSiteDataLoading();
        break;

      case "sitedatamanager:sites-updated":
        this.toggleSiteData(true);
        SiteDataManager.getTotalUsage().then(
          this.updateTotalDataSizeLabel.bind(this)
        );
        break;
    }
  },

  // Accessibility checkbox helpers
  _initA11yState() {
    this._initA11yString();
    let checkbox = document.getElementById("a11yPrivacyCheckbox");
    switch (Services.prefs.getIntPref("accessibility.force_disabled")) {
      case 1: // access blocked
        checkbox.checked = true;
        break;
      case -1: // a11y is forced on for testing
      case 0: // access allowed
        checkbox.checked = false;
        break;
    }
  },

  _initA11yString() {
    let a11yLearnMoreLink = Services.urlFormatter.formatURLPref(
      "accessibility.support.url"
    );
    document
      .getElementById("a11yLearnMoreLink")
      .setAttribute("href", a11yLearnMoreLink);
  },

  async updateA11yPrefs(checked) {
    let buttonIndex = await confirmRestartPrompt(checked, 0, true, false);
    if (buttonIndex == CONFIRM_RESTART_PROMPT_RESTART_NOW) {
      Services.prefs.setIntPref(
        "accessibility.force_disabled",
        checked ? 1 : 0
      );
      Services.startup.quit(
        Ci.nsIAppStartup.eAttemptQuit | Ci.nsIAppStartup.eRestart
      );
    }

    // Revert the checkbox in case we didn't quit
    document.getElementById("a11yPrivacyCheckbox").checked = !checked;
  },
};
