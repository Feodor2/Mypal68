/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

"use strict";

var EXPORTED_SYMBOLS = ["PluginParent", "PluginManager"];

const { AppConstants } = ChromeUtils.import(
  "resource://gre/modules/AppConstants.jsm"
);
const { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");
const { XPCOMUtils } = ChromeUtils.import(
  "resource://gre/modules/XPCOMUtils.jsm"
);

XPCOMUtils.defineLazyServiceGetter(
  this,
  "gPluginHost",
  "@mozilla.org/plugin/host;1",
  "nsIPluginHost"
);

ChromeUtils.defineModuleGetter(
  this,
  "BrowserUtils",
  "resource://gre/modules/BrowserUtils.jsm"
);
ChromeUtils.defineModuleGetter(
  this,
  "CrashSubmit",
  "resource://gre/modules/CrashSubmit.jsm"
);
XPCOMUtils.defineLazyGetter(this, "gNavigatorBundle", function() {
  const url = "chrome://browser/locale/browser.properties";
  return Services.strings.createBundle(url);
});
XPCOMUtils.defineLazyModuleGetters(this, {
  PrivateBrowsingUtils: "resource://gre/modules/PrivateBrowsingUtils.jsm",
});

const kNotificationId = "click-to-play-plugins";

const {
  PLUGIN_ACTIVE,
  PLUGIN_VULNERABLE_NO_UPDATE,
  PLUGIN_VULNERABLE_UPDATABLE,
  PLUGIN_CLICK_TO_PLAY_QUIET,
} = Ci.nsIObjectLoadingContent;

const PluginManager = {
  _initialized: false,

  pluginMap: new Map(),
  crashReports: new Map(),
  gmpCrashes: new Map(),
  _pendingCrashQueries: new Map(),

  ensureInitialized() {
    if (this._initialized) {
      return;
    }
    this._initialized = true;
    this._updatePluginMap();
    Services.obs.addObserver(this, "plugins-list-updated");
    Services.obs.addObserver(this, "profile-after-change");
  },

  destroy() {
    if (!this._initialized) {
      return;
    }
    Services.obs.removeObserver(this, "plugins-list-updated");
    Services.obs.removeObserver(this, "profile-after-change");
    this.crashReports = new Map();
    this.gmpCrashes = new Map();
    this.pluginMap = new Map();
  },

  observe(subject, topic, data) {
    switch (topic) {
      case "plugins-list-updated":
        this._updatePluginMap();
        break;
      case "plugin-crashed":
        this.ensureInitialized();
        this._registerNPAPICrash(subject);
        break;
      case "gmp-plugin-crash":
        this.ensureInitialized();
        this._registerGMPCrash(subject);
        break;
      case "profile-after-change":
        this.destroy();
        break;
    }
  },

  getPluginTagById(id) {
    return this.pluginMap.get(id);
  },

  _updatePluginMap() {
    this.pluginMap = new Map();
    let plugins = gPluginHost.getPluginTags();
    for (let plugin of plugins) {
      this.pluginMap.set(plugin.id, plugin);
    }
  },

  _registerNPAPICrash(subject) {
    let propertyBag = subject;
    if (
      !(propertyBag instanceof Ci.nsIWritablePropertyBag2) ||
      !propertyBag.hasKey("runID") ||
      !propertyBag.hasKey("pluginName")
    ) {
      Cu.reportError(
        "A NPAPI plugin crashed, but the notification is incomplete."
      );
      return;
    }

    let runID = propertyBag.getPropertyAsUint32("runID");
    let uglyPluginName = propertyBag.getPropertyAsAString("pluginName");
    let pluginName = BrowserUtils.makeNicePluginName(uglyPluginName);
    let pluginDumpID = propertyBag.getPropertyAsAString("pluginDumpID");
    let browserDumpID = propertyBag.getPropertyAsAString("browserDumpID");

    let state;
    let crashReporter = Services.appinfo.QueryInterface(Ci.nsICrashReporter);
    if (!AppConstants.MOZ_CRASHREPORTER || !crashReporter.enabled) {
      state = "noSubmit";
    } else if (!pluginDumpID) {
      state = "noReport";
    } else {
      state = "please";
    }

    let crashInfo = { runID, state, pluginName, pluginDumpID, browserDumpID };
    this.crashReports.set(runID, crashInfo);
    let listeners = this._pendingCrashQueries.get(runID) || [];
    for (let listener of listeners) {
      listener(crashInfo);
    }
    this._pendingCrashQueries.delete(runID);
  },

  _registerGMPCrash(subject) {
    let propertyBag = subject;
    if (
      !(propertyBag instanceof Ci.nsIWritablePropertyBag2) ||
      !propertyBag.hasKey("pluginID") ||
      !propertyBag.hasKey("pluginDumpID") ||
      !propertyBag.hasKey("pluginName")
    ) {
      Cu.reportError("PluginManager can not read plugin information.");
      return;
    }

    let pluginID = propertyBag.getPropertyAsUint32("pluginID");
    let pluginDumpID = propertyBag.getPropertyAsAString("pluginDumpID");
    let pluginName = propertyBag.getPropertyAsACString("pluginName");
    if (pluginDumpID) {
      this.gmpCrashes.set(pluginID, { pluginDumpID, pluginID, pluginName });
    }

    if (Services.ppmm) {
      Services.ppmm.broadcastAsyncMessage("gmp-plugin-crash", {
        pluginName,
        pluginID,
      });
    }
  },

  submitCrashReport(pluginCrashID, keyVals = {}) {
    let report = this.getCrashReport(pluginCrashID);
    if (!report) {
      Cu.reportError(
        `Could not find plugin dump IDs for ${JSON.stringify(pluginCrashID)}.` +
          `It is possible that a report was already submitted.`
      );
      return;
    }

    let { pluginDumpID, browserDumpID } = report;
    let submissionPromise = CrashSubmit.submit(pluginDumpID, {
      recordSubmission: true,
      extraExtraKeyVals: keyVals,
    });

    if (browserDumpID) {
      CrashSubmit.submit(browserDumpID).catch(Cu.reportError);
    }

    this.broadcastState(pluginCrashID, "submitting");

    submissionPromise.then(
      () => {
        this.broadcastState(pluginCrashID, "success");
      },
      () => {
        this.broadcastState(pluginCrashID, "failed");
      }
    );

    if (pluginCrashID.hasOwnProperty("runID")) {
      this.crashReports.delete(pluginCrashID.runID);
    } else {
      this.gmpCrashes.delete(pluginCrashID.pluginID);
    }
  },

  broadcastState(pluginCrashID, state) {
    if (!pluginCrashID.hasOwnProperty("runID")) {
      return;
    }
    let { runID } = pluginCrashID;
    Services.ppmm.broadcastAsyncMessage(
      "PluginParent:NPAPIPluginCrashReportSubmitted",
      { runID, state }
    );
  },

  getCrashReport(pluginCrashID) {
    if (pluginCrashID.hasOwnProperty("pluginID")) {
      return this.gmpCrashes.get(pluginCrashID.pluginID);
    }
    return this.crashReports.get(pluginCrashID.runID);
  },

  awaitPluginCrashInfo(runID) {
    if (this.crashReports.has(runID)) {
      return this.crashReports.get(runID);
    }
    let listeners = this._pendingCrashQueries.get(runID);
    if (!listeners) {
      listeners = [];
      this._pendingCrashQueries.set(runID, listeners);
    }
    return new Promise(resolve => listeners.push(resolve));
  },

  mockResponse(browser, handler) {
    let { currentWindowGlobal } = browser.frameLoader.browsingContext;
    currentWindowGlobal.getActor("Plugin")._mockedResponder = handler;
  },
};

class PluginParent extends JSWindowActorParent {
  constructor() {
    super();
    PluginManager.ensureInitialized();
  }

  receiveMessage(msg) {
    let browser = this.manager.rootFrameLoader.ownerElement;
    let win = browser.ownerGlobal;
    switch (msg.name) {
      case "PluginContent:ShowClickToPlayNotification":
        this.showClickToPlayNotification(
          browser,
          msg.data.plugins,
          msg.data.showNow,
          msg.data.location
        );
        break;
      case "PluginContent:RemoveNotification":
        this.removeNotification(browser);
        break;
      case "PluginContent:ShowPluginCrashedNotification":
        this.showPluginCrashedNotification(browser, msg.data.pluginCrashID);
        break;
      case "PluginContent:SubmitReport":
        if (AppConstants.MOZ_CRASHREPORTER) {
          this.submitReport(
            msg.data.runID,
            msg.data.keyVals,
            msg.data.submitURLOptIn
          );
        }
        break;
      case "PluginContent:LinkClickCallback":
        switch (msg.data.name) {
          case "managePlugins":
          case "openHelpPage":
            this[msg.data.name](win);
            break;
          case "openPluginUpdatePage":
            this.openPluginUpdatePage(win, msg.data.pluginId);
            break;
        }
        break;
      case "PluginContent:GetCrashData":
        if (this._mockedResponder) {
          let rv = this._mockedResponder(msg.data);
          delete this._mockedResponder;
          return rv;
        }
        return PluginManager.awaitPluginCrashInfo(msg.data.runID);

      default:
        Cu.reportError(
          "PluginParent did not expect to handle message " + msg.name
        );
        break;
    }

    return null;
  }

  managePlugins(window) {
    window.BrowserOpenAddonsMgr("addons://list/plugin");
  }

  async openPluginUpdatePage(window, pluginId) {
    let pluginTag = PluginManager.getPluginTagById(pluginId);
    if (!pluginTag) {
      return;
    }
    let { Blocklist } = ChromeUtils.import(
      "resource://gre/modules/Blocklist.jsm"
    );
    let url = await Blocklist.getPluginBlockURL(pluginTag);
    window.openTrustedLinkIn(url, "tab");
  }

  submitReport(runID, keyVals, submitURLOptIn) {
    if (!AppConstants.MOZ_CRASHREPORTER) {
      return;
    }
    Services.prefs.setBoolPref(
      "dom.ipc.plugins.reportCrashURL",
      !!submitURLOptIn
    );
    PluginManager.submitCrashReport({ runID }, keyVals);
  }

  reloadPage(browser) {
    browser.reload();
  }

  openHelpPage(window) {
    window.openHelpLink("plugin-crashed", false);
  }

  _clickToPlayNotificationEventCallback(event) {
    if (event == "dismissed") {
      this.options.showNow = false;
    }
  }

  _updatePluginPermission(aBrowser, aActivationInfo, aNewState) {
    let permission;
    let expireType = Ci.nsIPermissionManager.EXPIRE_NEVER;

    let window = aBrowser.ownerGlobal;
    let notification = window.PopupNotifications.getNotification(
      kNotificationId,
      aBrowser
    );

    switch (aNewState) {
      case "allownow":
        permission = Ci.nsIPermissionManager.ALLOW_ACTION;
        expireType = Ci.nsIPermissionManager.EXPIRE_SESSION;
        aActivationInfo.fallbackType = PLUGIN_ACTIVE;
        notification.options.extraAttr = "active";
        break;

      case "allowalways":
        permission = Ci.nsIPermissionManager.ALLOW_ACTION;
        aActivationInfo.fallbackType = PLUGIN_ACTIVE;
        notification.options.extraAttr = "active";
        break;

      case "block":
        permission = Ci.nsIPermissionManager.PROMPT_ACTION;
        let pluginTag = PluginManager.getPluginTagById(aActivationInfo.id);
        switch (pluginTag.blocklistState) {
          case Ci.nsIBlocklistService.STATE_VULNERABLE_UPDATE_AVAILABLE:
            aActivationInfo.fallbackType = PLUGIN_VULNERABLE_UPDATABLE;
            break;
          case Ci.nsIBlocklistService.STATE_VULNERABLE_NO_UPDATE:
            aActivationInfo.fallbackType = PLUGIN_VULNERABLE_NO_UPDATE;
            break;
          default:
            aActivationInfo.fallbackType = PLUGIN_CLICK_TO_PLAY_QUIET;
        }
        notification.options.extraAttr = "inactive";
        break;

      case "blockalways":
        permission =
          Ci.nsIObjectLoadingContent.PLUGIN_PERMISSION_PROMPT_ACTION_QUIET;
        aActivationInfo.fallbackType = PLUGIN_CLICK_TO_PLAY_QUIET;
        notification.options.extraAttr = "inactive";
        break;

      case "continue":
        aActivationInfo.fallbackType = PLUGIN_ACTIVE;
        notification.options.extraAttr = "active";
        break;

      case "continueblocking":
        aActivationInfo.fallbackType = PLUGIN_CLICK_TO_PLAY_QUIET;
        notification.options.extraAttr = "inactive";
        break;

      default:
        Cu.reportError(Error("Unexpected plugin state: " + aNewState));
        return;
    }

    if (aNewState != "continue" && aNewState != "continueblocking") {
      let { principal } = notification.options;
      Services.perms.addFromPrincipal(
        principal,
        aActivationInfo.permissionString,
        permission,
        expireType,
        0
      );
    }

    this.sendAsyncMessage("PluginParent:ActivatePlugins", {
      activationInfo: aActivationInfo,
      newState: aNewState,
    });
  }

  showClickToPlayNotification(browser, plugins, showNow, location) {
    let window = browser.ownerGlobal;
    let notification = window.PopupNotifications.getNotification(
      kNotificationId,
      browser
    );

    if (!plugins) {
      this.removeNotification(browser);
      return;
    }

    // If this is a new notification, create a pluginData map, otherwise append
    let pluginData;
    if (notification) {
      pluginData = notification.options.pluginData;
    } else {
      pluginData = new Map();
    }

    for (let pluginInfo of plugins) {
      if (pluginData.has(pluginInfo.permissionString)) {
        continue;
      }
      pluginData.set(pluginInfo.permissionString, pluginInfo);
    }

    let primaryPluginPermission = null;
    if (showNow) {
      primaryPluginPermission = plugins[0].permissionString;
    }

    if (notification) {
      // Don't modify the notification UI while it's on the screen, that would be
      // jumpy and might allow clickjacking.
      if (showNow) {
        notification.options.primaryPlugin = primaryPluginPermission;
        notification.reshow();
      }
      return;
    }

    let { id, fallbackType } = plugin;
    let pluginTag = PluginManager.getPluginTagById(id);
    if (!pluginTag) {
      return;
    }
    let permissionString = gPluginHost.getPermissionStringForTag(pluginTag);
    let isWindowPrivate = PrivateBrowsingUtils.isWindowPrivate(window);
    let active = fallbackType == PLUGIN_ACTIVE;

    let { top } = this.browsingContext;
    if (!top.currentWindowGlobal) {
      return;
    }
    let principal = top.currentWindowGlobal.documentPrincipal;

    let options = {
      dismissed: !showNow,
      hideClose: true,
      persistent: showNow,
      eventCallback: this._clickToPlayNotificationEventCallback,
      showNow,
      popupIconClass: "plugin-icon",
      extraAttr: active ? "active" : "inactive",
      principal,
    };

    let description;
    if (
      fallbackType == Ci.nsIObjectLoadingContent.PLUGIN_VULNERABLE_UPDATABLE
    ) {
      description = gNavigatorBundle.GetStringFromName(
        "flashActivate.outdated.message"
      );
    } else {
      description = gNavigatorBundle.GetStringFromName("flashActivate.message");
    }

    let badge = window.document.getElementById("plugin-icon-badge");
    badge.setAttribute("animate", "true");
    badge.addEventListener("animationend", function animListener(event) {
      if (
        event.animationName == "blink-badge" &&
        badge.hasAttribute("animate")
      ) {
        badge.removeAttribute("animate");
        badge.removeEventListener("animationend", animListener);
      }
    });

    let weakBrowser = Cu.getWeakReference(browser);

    let activationInfo = { id, fallbackType, permissionString };
    let mainAction = {
      callback: ({ checkboxChecked }) => {
        let browserRef = weakBrowser.get();
        if (!browserRef) {
          return;
        }
        let perm;
        if (checkboxChecked) {
          perm = "allowalways";
        } else {
          perm =
            activationInfo.fallbackType == PLUGIN_ACTIVE
              ? "continue"
              : "allownow";
        }
        this._updatePluginPermission(browserRef, activationInfo, perm);
      },
      label: gNavigatorBundle.GetStringFromName("flashActivate.allow"),
      accessKey: gNavigatorBundle.GetStringFromName(
        "flashActivate.allow.accesskey"
      ),
      dismiss: true,
    };

    let secondaryActions = null;
    if (!isWindowPrivate) {
      options.checkbox = {
        label: gNavigatorBundle.GetStringFromName("flashActivate.remember"),
      };
      secondaryActions = [
        {
          callback: ({ checkboxChecked }) => {
            let browserRef = weakBrowser.get();
            if (!browserRef) {
              return;
            }
            let perm;
            if (checkboxChecked) {
              perm = "blockalways";
            } else {
              perm =
                activationInfo.fallbackType == PLUGIN_ACTIVE
                  ? "block"
                  : "continueblocking";
            }
            this._updatePluginPermission(browserRef, activationInfo, perm);
          },
          label: gNavigatorBundle.GetStringFromName("flashActivate.noAllow"),
          accessKey: gNavigatorBundle.GetStringFromName(
            "flashActivate.noAllow.accesskey"
          ),
          dismiss: true,
        },
      ];
    }

    window.PopupNotifications.show(
      browser,
      kNotificationId,
      description,
      "plugins-notification-icon",
      mainAction,
      secondaryActions,
      options
    );

    let haveInsecure = false;
    switch (fallbackType) {
      case PLUGIN_VULNERABLE_UPDATABLE:
      case PLUGIN_VULNERABLE_NO_UPDATE:
        haveInsecure = true;
    }

    window.document
      .getElementById("plugins-notification-icon")
      .classList.toggle("plugin-blocked", haveInsecure);
  }

  removeNotification(browser) {
    let { PopupNotifications } = browser.ownerGlobal;
    let notification = PopupNotifications.getNotification(
      kNotificationId,
      browser
    );
    if (notification) {
      PopupNotifications.remove(notification);
    }
  }

  showPluginCrashedNotification(browser, pluginCrashID) {
    // If there's already an existing notification bar, don't do anything.
    let notificationBox = browser.getTabBrowser().getNotificationBox(browser);
    let notification = notificationBox.getNotificationWithValue(
      "plugin-crashed"
    );

    let report = PluginManager.getCrashReport(pluginCrashID);
    if (notification || !report) {
      return;
    }

    let priority = notificationBox.PRIORITY_WARNING_MEDIUM;
    let iconURL = "chrome://global/skin/plugins/pluginGeneric.svg";
    let reloadLabel = gNavigatorBundle.GetStringFromName(
      "crashedpluginsMessage.reloadButton.label"
    );
    let reloadKey = gNavigatorBundle.GetStringFromName(
      "crashedpluginsMessage.reloadButton.accesskey"
    );

    let buttons = [
      {
        label: reloadLabel,
        accessKey: reloadKey,
        popup: null,
        callback() {
          browser.reload();
        },
      },
    ];

    if (AppConstants.MOZ_CRASHREPORTER) {
      let submitLabel = gNavigatorBundle.GetStringFromName(
        "crashedpluginsMessage.submitButton.label"
      );
      let submitKey = gNavigatorBundle.GetStringFromName(
        "crashedpluginsMessage.submitButton.accesskey"
      );
      let submitButton = {
        label: submitLabel,
        accessKey: submitKey,
        popup: null,
        callback: () => {
          PluginManager.submitCrashReport(pluginCrashID);
        },
      };

      buttons.push(submitButton);
    }

    let messageString = gNavigatorBundle.formatStringFromName(
      "crashedpluginsMessage.title",
      [report.pluginName]
    );
    notification = notificationBox.appendNotification(
      messageString,
      "plugin-crashed",
      iconURL,
      priority,
      buttons
    );

    // Add the "learn more" link.
    let link = notification.ownerDocument.createXULElement("label", {
      is: "text-link",
    });
    link.setAttribute(
      "value",
      gNavigatorBundle.GetStringFromName("crashedpluginsMessage.learnMore")
    );
    let crashurl = Services.urlFormatter.formatURLPref("app.support.baseURL");
    crashurl += "plugin-crashed-notificationbar";
    link.href = crashurl;
    notification.messageText.appendChild(link);
  }
}
