/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

"use strict";

ChromeUtils.import("resource://gre/modules/Promise.jsm", this);
const { AppConstants } = ChromeUtils.import(
  "resource://gre/modules/AppConstants.jsm"
);
var GMPScope = ChromeUtils.import(
  "resource://gre/modules/addons/GMPProvider.jsm",
  null
);

const TEST_DATE = new Date(2013, 0, 1, 12);

var gManagerWindow;
var gCategoryUtilities;

var gMockAddons = [];

for (let plugin of GMPScope.GMP_PLUGINS) {
  let mockAddon = Object.freeze({
    id: plugin.id,
    isValid: true,
    isInstalled: false,
  });
  gMockAddons.push(mockAddon);
}

var gInstalledAddonId = "";
var gInstallDeferred = null;
var gPrefs = Services.prefs;
var getKey = GMPScope.GMPPrefs.getPrefKey;

function MockGMPInstallManager() {}

MockGMPInstallManager.prototype = {
  checkForAddons: () =>
    Promise.resolve({
      usedFallback: true,
      gmpAddons: gMockAddons,
    }),

  installAddon: addon => {
    gInstalledAddonId = addon.id;
    gInstallDeferred.resolve();
    return Promise.resolve();
  },
};

function openDetailsView(aId) {
  let view = get_current_view(gManagerWindow);
  if (gManagerWindow.useHtmlViews) {
    Assert.equal(
      view.id,
      "html-view",
      "Should be in the list view to use this function"
    );
  } else {
    Assert.equal(
      view.id,
      "list-view",
      "Should be in the list view to use this function"
    );
  }

  let item = get_addon_element(gManagerWindow, aId);
  Assert.ok(item, "Should have got add-on element.");
  is_element_visible(item, "Add-on element should be visible.");

  item.scrollIntoView();
  EventUtils.synthesizeMouseAtCenter(item, { clickCount: 1 }, item.ownerGlobal);
  EventUtils.synthesizeMouseAtCenter(item, { clickCount: 2 }, item.ownerGlobal);

  return new Promise(resolve => {
    wait_for_view_load(gManagerWindow, resolve);
  });
}

async function initializeState({ useHtmlViews }) {
  await SpecialPowers.pushPrefEnv({
    set: [["extensions.htmlaboutaddons.enabled", useHtmlViews]],
  });

  gPrefs.setBoolPref(GMPScope.GMPPrefs.KEY_LOGGING_DUMP, true);
  gPrefs.setIntPref(GMPScope.GMPPrefs.KEY_LOGGING_LEVEL, 0);

  gManagerWindow = await open_manager();

  gCategoryUtilities = new CategoryUtilities(gManagerWindow);

  registerCleanupFunction(async function() {
    for (let addon of gMockAddons) {
      gPrefs.clearUserPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_ENABLED, addon.id)
      );
      gPrefs.clearUserPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_LAST_UPDATE, addon.id)
      );
      gPrefs.clearUserPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_AUTOUPDATE, addon.id)
      );
      gPrefs.clearUserPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_VERSION, addon.id)
      );
      gPrefs.clearUserPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_VISIBLE, addon.id)
      );
      gPrefs.clearUserPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_FORCE_SUPPORTED, addon.id)
      );
    }
    gPrefs.clearUserPref(GMPScope.GMPPrefs.KEY_LOGGING_DUMP);
    gPrefs.clearUserPref(GMPScope.GMPPrefs.KEY_LOGGING_LEVEL);
    gPrefs.clearUserPref(GMPScope.GMPPrefs.KEY_UPDATE_LAST_CHECK);
    await GMPScope.GMPProvider.shutdown();
    GMPScope.GMPProvider.startup();
  });

  // Start out with plugins not being installed, disabled and automatic updates
  // disabled.
  for (let addon of gMockAddons) {
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_ENABLED, addon.id),
      false
    );
    gPrefs.setIntPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_LAST_UPDATE, addon.id),
      0
    );
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_AUTOUPDATE, addon.id),
      false
    );
    gPrefs.setCharPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_VERSION, addon.id),
      ""
    );
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_VISIBLE, addon.id),
      true
    );
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_FORCE_SUPPORTED, addon.id),
      true
    );
  }
  await GMPScope.GMPProvider.shutdown();
  GMPScope.GMPProvider.startup();
}

async function testNotInstalledDisabled() {
  Assert.ok(gCategoryUtilities.isTypeVisible("plugin"), "Plugin tab visible.");
  await gCategoryUtilities.openType("plugin");

  for (let addon of gMockAddons) {
    let item = get_addon_element(gManagerWindow, addon.id);
    Assert.ok(item, "Got add-on element:" + addon.id);
    if (gManagerWindow.useHtmlViews) {
      is(item.parentNode.getAttribute("section"), "1", "Should be disabled");
      // Open the options menu (needed to check the disabled buttons).
      const pluginOptions = item.querySelector("plugin-options");
      pluginOptions.querySelector("panel-list").open = true;
      const neverActivate = pluginOptions.querySelector(
        "panel-item[action=never-activate]"
      );
      ok(
        neverActivate.hasAttribute("checked"),
        "Plugin state should be never-activate"
      );
      pluginOptions.querySelector("panel-list").open = false;
    } else {
      item.parentNode.ensureElementIsVisible(item);
      is(item.getAttribute("active"), "false", "Should be disabled");

      let el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "warning"
      );
      is_element_hidden(el, "Warning notification is hidden.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "class",
        "disabled-postfix"
      );
      is_element_visible(el, "disabled-postfix is visible.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "disable-btn"
      );
      is_element_hidden(el, "Disable button not visible.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "enable-btn"
      );
      is_element_hidden(el, "Enable button not visible.");

      let menu = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "state-menulist"
      );
      is_element_visible(menu, "State menu should be visible.");

      let neverActivate = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "never-activate-menuitem"
      );
      is(
        menu.selectedItem,
        neverActivate,
        "Plugin state should be never-activate."
      );
    }
  }
}

async function testNotInstalledDisabledDetails() {
  for (let addon of gMockAddons) {
    await openDetailsView(addon.id);
    let doc = gManagerWindow.document;

    let el = doc.getElementsByClassName("disabled-postfix")[0];
    is_element_visible(el, "disabled-postfix is visible.");
    el = doc.getElementById("detail-findUpdates-btn");
    is_element_visible(el, "Find updates link is visible.");
    el = doc.getElementById("detail-warning");
    is_element_hidden(el, "Warning notification is hidden.");
    el = doc.getElementsByTagName("setting")[0];

    await gCategoryUtilities.openType("plugin");
  }
}

async function testNotInstalled() {
  for (let addon of gMockAddons) {
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_ENABLED, addon.id),
      true
    );
    let item = get_addon_element(gManagerWindow, addon.id);
    Assert.ok(item, "Got add-on element:" + addon.id);
    if (gManagerWindow.useHtmlViews) {
      is(item.parentNode.getAttribute("section"), "0", "Should be enabled");
      // Open the options menu (needed to check the disabled buttons).
      const pluginOptions = item.querySelector("plugin-options");
      pluginOptions.querySelector("panel-list").open = true;
      const alwaysActivate = pluginOptions.querySelector(
        "panel-item[action=always-activate]"
      );
      ok(
        alwaysActivate.hasAttribute("checked"),
        "Plugin state should be always-activate"
      );
      pluginOptions.querySelector("panel-list").open = false;
    } else {
      item.parentNode.ensureElementIsVisible(item);
      is(item.getAttribute("active"), "true");

      let el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "warning"
      );
      is_element_visible(el, "Warning notification is visible.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "class",
        "disabled-postfix"
      );
      is_element_hidden(el, "disabled-postfix is hidden.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "disable-btn"
      );
      is_element_hidden(el, "Disable button not visible.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "enable-btn"
      );
      is_element_hidden(el, "Enable button not visible.");

      let menu = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "state-menulist"
      );
      is_element_visible(menu, "State menu should be visible.");

      let alwaysActivate = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "always-activate-menuitem"
      );
      is(
        menu.selectedItem,
        alwaysActivate,
        "Plugin state should be always-activate."
      );
    }
  }
}

async function testNotInstalledDetails() {
  for (let addon of gMockAddons) {
    await openDetailsView(addon.id);
    if (gManagerWindow.useHtmlViews) {
      const addonCard = get_addon_element(gManagerWindow, addon.id);
      let el = addonCard.querySelector("[action=update-check]");
      is_element_visible(el, "Check for Updates action is visible");
      // TODO: add assertion for the warning notification once implemented
      // as part of Bug 1544950.
    } else {
      let doc = gManagerWindow.document;
      let el = doc.getElementsByClassName("disabled-postfix")[0];
      is_element_hidden(el, "disabled-postfix is hidden.");
      el = doc.getElementById("detail-findUpdates-btn");
      is_element_visible(el, "Find updates link is visible.");
      el = doc.getElementById("detail-warning");
      is_element_visible(el, "Warning notification is visible.");
    }

    await gCategoryUtilities.openType("plugin");
  }
}

async function testInstalled() {
  for (let addon of gMockAddons) {
    gPrefs.setIntPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_LAST_UPDATE, addon.id),
      TEST_DATE.getTime()
    );
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_AUTOUPDATE, addon.id),
      false
    );
    gPrefs.setCharPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_VERSION, addon.id),
      "1.2.3.4"
    );

    let item = get_addon_element(gManagerWindow, addon.id);
    Assert.ok(item, "Got add-on element.");
    if (gManagerWindow.useHtmlViews) {
      is(item.parentNode.getAttribute("section"), "0", "Should be enabled");
      // Open the options menu (needed to check the disabled buttons).
      const pluginOptions = item.querySelector("plugin-options");
      pluginOptions.querySelector("panel-list").open = true;
      const alwaysActivate = pluginOptions.querySelector(
        "panel-item[action=always-activate]"
      );
      ok(
        alwaysActivate.hasAttribute("checked"),
        "Plugin state should be always-activate"
      );
      pluginOptions.querySelector("panel-list").open = false;
    } else {
      item.parentNode.ensureElementIsVisible(item);
      is(item.getAttribute("active"), "true");

      let el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "warning"
      );
      is_element_hidden(el, "Warning notification is hidden.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "class",
        "disabled-postfix"
      );
      is_element_hidden(el, "disabled-postfix is hidden.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "disable-btn"
      );
      is_element_hidden(el, "Disable button not visible.");
      el = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "enable-btn"
      );
      is_element_hidden(el, "Enable button not visible.");

      let menu = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "state-menulist"
      );
      is_element_visible(menu, "State menu should be visible.");

      let alwaysActivate = item.ownerDocument.getAnonymousElementByAttribute(
        item,
        "anonid",
        "always-activate-menuitem"
      );
      is(
        menu.selectedItem,
        alwaysActivate,
        "Plugin state should be always-activate."
      );
    }
  }
}

async function testInstalledDetails() {
  for (let addon of gMockAddons) {
    await openDetailsView(addon.id);

    if (gManagerWindow.useHtmlViews) {
      let card = get_addon_element(gManagerWindow, addon.id);
      is_element_visible(
        card.querySelector("[action=update-check]"),
        "Find updates link is bisible"
      );
    } else {
      let doc = gManagerWindow.document;

      let el = doc.getElementsByClassName("disabled-postfix")[0];
      is_element_hidden(el, "disabled-postfix is hidden.");
      el = doc.getElementById("detail-findUpdates-btn");
      is_element_visible(el, "Find updates link is visible.");
      el = doc.getElementById("detail-warning");
      is_element_hidden(el, "Warning notification is hidden.");
      el = doc.getElementsByTagName("setting")[0];

      let contextMenu = doc.getElementById("addonitem-popup");
      await new Promise(resolve => {
        let listener = () => {
          contextMenu.removeEventListener("popupshown", listener);
          resolve();
        };
        contextMenu.addEventListener("popupshown", listener);
        el = doc.getElementsByClassName("detail-view-container")[0];
        EventUtils.synthesizeMouse(el, 4, 4, {}, gManagerWindow);
        EventUtils.synthesizeMouse(
          el,
          4,
          4,
          { type: "contextmenu", button: 2 },
          gManagerWindow
        );
      });
      let menuSep = doc.getElementById("addonitem-menuseparator");
      is_element_hidden(menuSep, "Menu separator is hidden.");
      contextMenu.hidePopup();
    }

    await gCategoryUtilities.openType("plugin");
  }
}

async function testInstalledGlobalEmeDisabled() {
  gPrefs.setBoolPref(GMPScope.GMPPrefs.KEY_EME_ENABLED, false);
  for (let addon of gMockAddons) {
    let item = get_addon_element(gManagerWindow, addon.id);
    if (addon.isEME) {
      if (gManagerWindow.useHtmlViews) {
        is(item.parentNode.getAttribute("section"), "1", "Should be disabled");
        // Open the options menu (needed to check the disabled buttons).
        const pluginOptions = item.querySelector("plugin-options");
        pluginOptions.querySelector("panel-list").open = true;
        const askActivate = pluginOptions.querySelector(
          "panel-item[action=ask-to-activate]"
        );
        ok(
          askActivate.shadowRoot.querySelector("button").disabled,
          "ask-to-activate should be disabled"
        );
        pluginOptions.querySelector("panel-list").open = false;
      } else {
        Assert.ok(!item, "Couldn't get add-on element.");
      }
    } else {
      Assert.ok(item, "Got add-on element.");
    }
  }
  gPrefs.setBoolPref(GMPScope.GMPPrefs.KEY_EME_ENABLED, true);
}

async function testPreferencesButton() {
  let prefValues = [
    { enabled: false, version: "" },
    { enabled: false, version: "1.2.3.4" },
    { enabled: true, version: "" },
    { enabled: true, version: "1.2.3.4" },
  ];

  for (let preferences of prefValues) {
    dump(
      "Testing preferences button with pref settings: " +
        JSON.stringify(preferences) +
        "\n"
    );
    for (let addon of gMockAddons) {
      await close_manager(gManagerWindow);
      gManagerWindow = await open_manager();
      gCategoryUtilities = new CategoryUtilities(gManagerWindow);
      gPrefs.setCharPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_VERSION, addon.id),
        preferences.version
      );
      gPrefs.setBoolPref(
        getKey(GMPScope.GMPPrefs.KEY_PLUGIN_ENABLED, addon.id),
        preferences.enabled
      );

      await gCategoryUtilities.openType("plugin");
      let doc = gManagerWindow.document;
      let item = get_addon_element(gManagerWindow, addon.id);

      if (gManagerWindow.useHtmlViews) {
        // Open the options menu (needed to check the more options action is enabled).
        const pluginOptions = item.querySelector("plugin-options");
        pluginOptions.querySelector("panel-list").open = true;
        const moreOptions = pluginOptions.querySelector(
          "panel-item[action=expand]"
        );
        ok(
          !moreOptions.shadowRoot.querySelector("button").disabled,
          "more options action should be enabled"
        );
        moreOptions.click();
      } else {
        let button = doc.getAnonymousElementByAttribute(
          item,
          "anonid",
          "preferences-btn"
        );
        is_element_visible(button);
        EventUtils.synthesizeMouseAtCenter(
          button,
          { clickCount: 1 },
          gManagerWindow
        );
      }

      await wait_for_view_load(gManagerWindow);
    }
  }
}

async function testUpdateButton() {
  gPrefs.clearUserPref(GMPScope.GMPPrefs.KEY_UPDATE_LAST_CHECK);

  let originalInstallManager = GMPScope.GMPInstallManager;
  Object.defineProperty(GMPScope, "GMPInstallManager", {
    value: MockGMPInstallManager,
    writable: true,
    enumerable: true,
    configurable: true,
  });

  for (let addon of gMockAddons) {
    await gCategoryUtilities.openType("plugin");
    let doc = gManagerWindow.document;
    let item = get_addon_element(gManagerWindow, addon.id);

    gInstalledAddonId = "";
    gInstallDeferred = Promise.defer();

    if (gManagerWindow.useHtmlViews) {
      item.querySelector("[action=expand]").click();
      await wait_for_view_load(gManagerWindow);
      let detail = get_addon_element(gManagerWindow, addon.id);
      detail.querySelector("[action=update-check]").click();
    } else {
      let button = doc.getAnonymousElementByAttribute(
        item,
        "anonid",
        "preferences-btn"
      );
      EventUtils.synthesizeMouseAtCenter(
        button,
        { clickCount: 1 },
        gManagerWindow
      );
      await wait_for_view_load(gManagerWindow);

      button = doc.getElementById("detail-findUpdates-btn");
      Assert.ok(button != null, "Got detail-findUpdates-btn");
      EventUtils.synthesizeMouseAtCenter(
        button,
        { clickCount: 1 },
        gManagerWindow
      );
    }

    await gInstallDeferred.promise;
    Assert.equal(gInstalledAddonId, addon.id);
  }
  Object.defineProperty(GMPScope, "GMPInstallManager", {
    value: originalInstallManager,
    writable: true,
    enumerable: true,
    configurable: true,
  });
}

async function testEmeSupport() {
  for (let addon of gMockAddons) {
    gPrefs.clearUserPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_FORCE_SUPPORTED, addon.id)
    );
  }
  await GMPScope.GMPProvider.shutdown();
  GMPScope.GMPProvider.startup();

  for (let addon of gMockAddons) {
    await gCategoryUtilities.openType("plugin");
    let item = get_addon_element(gManagerWindow, addon.id);
    Assert.ok(item, "Found add-on element.");
  }

  for (let addon of gMockAddons) {
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_VISIBLE, addon.id),
      true
    );
    gPrefs.setBoolPref(
      getKey(GMPScope.GMPPrefs.KEY_PLUGIN_FORCE_SUPPORTED, addon.id),
      true
    );
  }
  await GMPScope.GMPProvider.shutdown();
  GMPScope.GMPProvider.startup();
}

async function testCleanupState() {
  await SpecialPowers.popPrefEnv();
  await close_manager(gManagerWindow);
}

// This function run the sequence of all the gmpProvider tests
// under the same initializeStateOptions (which will enable or disable
// the HTML about:addons views).
async function test_gmpProvider(initializeStateOptions) {
  await initializeState(initializeStateOptions);
  await testNotInstalledDisabled();
  await testNotInstalledDisabledDetails();
  await testNotInstalled();
  await testNotInstalledDetails();
  await testInstalled();
  await testInstalledDetails();
  await testInstalledGlobalEmeDisabled();
  await testPreferencesButton();
  await testUpdateButton();
  await testEmeSupport();
  await testCleanupState();
}

add_task(function test_gmpProvider_on_XUL_aboutaddons() {
  return test_gmpProvider({ useHtmlViews: false });
});

add_task(async function test_gmpProvider_on_HTML_aboutaddons() {
  return test_gmpProvider({ useHtmlViews: true });
});
