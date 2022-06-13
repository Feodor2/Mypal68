/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/* eslint max-len: ["error", 80] */
/* exported initialize, hide, show */
/* import-globals-from aboutaddonsCommon.js */
/* import-globals-from abuse-reports.js */
/* global MozXULElement, windowRoot */

"use strict";

XPCOMUtils.defineLazyModuleGetters(this, {
  AddonManager: "resource://gre/modules/AddonManager.jsm",
  AMTelemetry: "resource://gre/modules/AddonManager.jsm",
  ClientID: "resource://gre/modules/ClientID.jsm",
  DeferredTask: "resource://gre/modules/DeferredTask.jsm",
  E10SUtils: "resource://gre/modules/E10SUtils.jsm",
  ExtensionCommon: "resource://gre/modules/ExtensionCommon.jsm",
  ExtensionParent: "resource://gre/modules/ExtensionParent.jsm",
  ExtensionPermissions: "resource://gre/modules/ExtensionPermissions.jsm",
  PrivateBrowsingUtils: "resource://gre/modules/PrivateBrowsingUtils.jsm",
});

XPCOMUtils.defineLazyGetter(this, "browserBundle", () => {
  return Services.strings.createBundle(
    "chrome://browser/locale/browser.properties"
  );
});
XPCOMUtils.defineLazyGetter(this, "brandBundle", () => {
  return Services.strings.createBundle(
    "chrome://branding/locale/brand.properties"
  );
});
XPCOMUtils.defineLazyGetter(this, "extBundle", function() {
  return Services.strings.createBundle(
    "chrome://mozapps/locale/extensions/extensions.properties"
  );
});
XPCOMUtils.defineLazyGetter(this, "extensionStylesheets", () => {
  const { ExtensionParent } = ChromeUtils.import(
    "resource://gre/modules/ExtensionParent.jsm"
  );
  return ExtensionParent.extensionStylesheets;
});

XPCOMUtils.defineLazyPreferenceGetter(
  this,
  "allowPrivateBrowsingByDefault",
  "extensions.allowPrivateBrowsingByDefault",
  true
);
XPCOMUtils.defineLazyPreferenceGetter(
  this,
  "SUPPORT_URL",
  "app.support.baseURL",
  "",
  null,
  val => Services.urlFormatter.formatURL(val)
);

const UPDATES_RECENT_TIMESPAN = 2 * 24 * 3600000; // 2 days (in milliseconds)

XPCOMUtils.defineLazyPreferenceGetter(
  this,
  "ABUSE_REPORT_ENABLED",
  "extensions.abuseReport.enabled",
  false
);

const PLUGIN_ICON_URL = "chrome://global/skin/plugins/pluginGeneric.svg";
const EXTENSION_ICON_URL =
  "chrome://mozapps/skin/extensions/extensionGeneric.svg";
const PERMISSION_MASKS = {
  "ask-to-activate": AddonManager.PERM_CAN_ASK_TO_ACTIVATE,
  enable: AddonManager.PERM_CAN_ENABLE,
  "always-activate": AddonManager.PERM_CAN_ENABLE,
  disable: AddonManager.PERM_CAN_DISABLE,
  "never-activate": AddonManager.PERM_CAN_DISABLE,
  uninstall: AddonManager.PERM_CAN_UNINSTALL,
  upgrade: AddonManager.PERM_CAN_UPGRADE,
  "change-privatebrowsing": AddonManager.PERM_CAN_CHANGE_PRIVATEBROWSING_ACCESS,
};

const PREF_TELEMETRY_ENABLED = "datareporting.healthreport.uploadEnabled";
const PRIVATE_BROWSING_PERM_NAME = "internal:privateBrowsingAllowed";
const PRIVATE_BROWSING_PERMS = {
  permissions: [PRIVATE_BROWSING_PERM_NAME],
  origins: [],
};

const AddonCardListenerHandler = {
  ADDON_EVENTS: new Set([
    "onDisabled",
    "onEnabled",
    "onInstalled",
    "onPropertyChanged",
    "onUninstalling",
  ]),
  MANAGER_EVENTS: new Set(["onUpdateModeChanged"]),
  INSTALL_EVENTS: new Set(["onNewInstall", "onInstallEnded"]),

  delegateAddonEvent(name, args) {
    this.delegateEvent(name, args[0], args);
  },

  delegateInstallEvent(name, args) {
    let addon = args[0].addon || args[0].existingAddon;
    if (!addon) {
      return;
    }
    this.delegateEvent(name, addon, args);
  },

  delegateEvent(name, addon, args) {
    let cards;
    if (this.MANAGER_EVENTS.has(name)) {
      cards = document.querySelectorAll("addon-card");
    } else {
      let card = document.querySelector(`addon-card[addon-id="${addon.id}"]`);
      cards = card ? [card] : [];
    }
    for (let card of cards) {
      try {
        if (name in card) {
          card[name](...args);
        }
      } catch (e) {
        Cu.reportError(e);
      }
    }
  },

  startup() {
    for (let name of this.ADDON_EVENTS) {
      this[name] = (...args) => this.delegateAddonEvent(name, args);
    }
    for (let name of this.INSTALL_EVENTS) {
      this[name] = (...args) => this.delegateInstallEvent(name, args);
    }
    for (let name of this.MANAGER_EVENTS) {
      this[name] = (...args) => this.delegateEvent(name, null, args);
    }
    AddonManager.addAddonListener(this);
    AddonManager.addInstallListener(this);
    AddonManager.addManagerListener(this);
  },

  shutdown() {
    AddonManager.removeAddonListener(this);
    AddonManager.removeInstallListener(this);
    AddonManager.removeManagerListener(this);
  },
};

function isAbuseReportSupported(addon) {
  return (
    ABUSE_REPORT_ENABLED &&
    ["extension", "theme"].includes(addon.type) &&
    !(addon.isBuiltin || addon.isSystem)
  );
}

async function isAllowedInPrivateBrowsing(addon) {
  // Use the Promise directly so this function stays sync for the other case.
  let perms = await ExtensionPermissions.get(addon.id);
  return perms.permissions.includes(PRIVATE_BROWSING_PERM_NAME);
}

function hasPermission(addon, permission) {
  return !!(addon.permissions & PERMISSION_MASKS[permission]);
}

function isPending(addon, action) {
  const amAction = AddonManager["PENDING_" + action.toUpperCase()];
  return !!(addon.pendingOperations & amAction);
}

async function getAddonMessageInfo(addon) {
  const { name } = addon;
  const appName = brandBundle.GetStringFromName("brandShortName");
  const {
    STATE_BLOCKED,
    STATE_OUTDATED,
    STATE_SOFTBLOCKED,
    STATE_VULNERABLE_UPDATE_AVAILABLE,
    STATE_VULNERABLE_NO_UPDATE,
  } = Ci.nsIBlocklistService;

  const formatString = (name, args) =>
    extBundle.formatStringFromName(
      `details.notification.${name}`,
      args,
      args.length
    );
  const getString = name =>
    extBundle.GetStringFromName(`details.notification.${name}`);

  if (addon.blocklistState === STATE_BLOCKED) {
    return {
      linkText: getString("blocked.link"),
      linkUrl: await addon.getBlocklistURL(),
      message: formatString("blocked", [name]),
      type: "error",
    };
  } else if (isDisabledUnsigned(addon)) {
    return {
      linkText: getString("unsigned.link"),
      linkUrl: SUPPORT_URL + "unsigned-addons",
      message: formatString("unsignedAndDisabled", [name, appName]),
      type: "error",
    };
  } else if (
    !addon.isCompatible &&
    (AddonManager.checkCompatibility ||
      addon.blocklistState !== STATE_SOFTBLOCKED)
  ) {
    return {
      message: formatString("incompatible", [
        name,
        appName,
        Services.appinfo.version,
      ]),
      type: "warning",
    };
  } else if (!isCorrectlySigned(addon)) {
    return {
      linkText: getString("unsigned.link"),
      linkUrl: SUPPORT_URL + "unsigned-addons",
      message: formatString("unsigned", [name, appName]),
      type: "warning",
    };
  } else if (addon.blocklistState === STATE_SOFTBLOCKED) {
    return {
      linkText: getString("softblocked.link"),
      linkUrl: await addon.getBlocklistURL(),
      message: formatString("softblocked", [name]),
      type: "warning",
    };
  } else if (addon.blocklistState === STATE_OUTDATED) {
    return {
      linkText: getString("outdated.link"),
      linkUrl: await addon.getBlocklistURL(),
      message: formatString("outdated", [name]),
      type: "warning",
    };
  } else if (addon.blocklistState === STATE_VULNERABLE_UPDATE_AVAILABLE) {
    return {
      linkText: getString("vulnerableUpdatable.link"),
      linkUrl: await addon.getBlocklistURL(),
      message: formatString("vulnerableUpdatable", [name]),
      type: "error",
    };
  } else if (addon.blocklistState === STATE_VULNERABLE_NO_UPDATE) {
    return {
      linkText: getString("vulnerableNoUpdate.link"),
      linkUrl: await addon.getBlocklistURL(),
      message: formatString("vulnerableNoUpdate", [name]),
      type: "error",
    };
  } else if (addon.isGMPlugin && !addon.isInstalled && addon.isActive) {
    return {
      message: formatString("gmpPending", [name]),
      type: "warning",
    };
  }
  return {};
}

// Don't change how we handle this while the page is open.
const INLINE_OPTIONS_ENABLED = Services.prefs.getBoolPref(
  "extensions.htmlaboutaddons.inline-options.enabled"
);
const OPTIONS_TYPE_MAP = {
  [AddonManager.OPTIONS_TYPE_TAB]: "tab",
  [AddonManager.OPTIONS_TYPE_INLINE_BROWSER]: INLINE_OPTIONS_ENABLED
    ? "inline"
    : "tab",
};

// Check if an add-on has the provided options type, accounting for the pref
// to disable inline options.
function getOptionsType(addon, type) {
  return OPTIONS_TYPE_MAP[addon.optionsType];
}

// Check whether the options page can be loaded in the current browser window.
async function isAddonOptionsUIAllowed(addon) {
  if (addon.type !== "extension" || !getOptionsType(addon)) {
    // Themes never have options pages.
    // Some plugins have preference pages, and they can always be shown.
    // Extensions do not need to be checked if they do not have options pages.
    return true;
  }
  if (!PrivateBrowsingUtils.isContentWindowPrivate(window)) {
    return true;
  }
  if (addon.incognito === "not_allowed") {
    return false;
  }
  // The current page is in a private browsing window, and the add-on does not
  // have the permission to access private browsing windows. Block access.
  return (
    allowPrivateBrowsingByDefault ||
    // Note: This function is async because isAllowedInPrivateBrowsing is async.
    isAllowedInPrivateBrowsing(addon)
  );
}

/**
 * This function is set in initialize() by the parent about:addons window. It
 * is a helper for gViewController.loadView().
 *
 * @param {string} type The view type to load.
 * @param {string} param The (optional) param for the view.
 */
let loadViewFn;
let replaceWithDefaultViewFn;
let setCategoryFn;

let _templates = {};

/**
 * Import a template from the main document.
 */
function importTemplate(name) {
  if (!_templates.hasOwnProperty(name)) {
    _templates[name] = document.querySelector(`template[name="${name}"]`);
  }
  let template = _templates[name];
  if (template) {
    return document.importNode(template.content, true);
  }
  throw new Error(`Unknown template: ${name}`);
}

function nl2br(text) {
  let frag = document.createDocumentFragment();
  let hasAppended = false;
  for (let part of text.split("\n")) {
    if (hasAppended) {
      frag.appendChild(document.createElement("br"));
    }
    frag.appendChild(new Text(part));
    hasAppended = true;
  }
  return frag;
}

/**
 * Select the screeenshot to display above an add-on card.
 *
 * @param {AddonWrapper|DiscoAddonWrapper} addon
 * @returns {string|null}
 *          The URL of the best fitting screenshot, if any.
 */
function getScreenshotUrlForAddon(addon) {
  let { screenshots } = addon;
  if (!screenshots || !screenshots.length) {
    return null;
  }

  // The image size is defined at .card-heading-image in aboutaddons.css, and
  // is based on the aspect ratio for a 680x92 image. Use the image if possible,
  // and otherwise fall back to the first image and hope for the best.
  let screenshot = screenshots.find(s => s.width === 680 && s.height === 92);
  if (!screenshot) {
    console.warn(`Did not find screenshot with desired size for ${addon.id}.`);
    screenshot = screenshots[0];
  }
  return screenshot.url;
}

/**
 * Adds UTM parameters to a given URL, if it is an AMO URL.
 *
 * @param {string} contentAttribute
 *        Identifies the part of the UI with which the link is associated.
 * @param {string} url
 * @returns {string}
 *          The url with UTM parameters if it is an AMO URL.
 *          Otherwise the url in unmodified form.
 */
function formatAmoUrl(contentAttribute, url) {
  let parsedUrl = new URL(url);
  let domain = `.${parsedUrl.hostname}`;
  if (
    !domain.endsWith(".addons.mozilla.org") &&
    // For testing: addons-dev.allizom.org and addons.allizom.org
    !domain.endsWith(".allizom.org")
  ) {
    return url;
  }

  parsedUrl.searchParams.set("utm_source", "firefox-browser");
  parsedUrl.searchParams.set("utm_medium", "firefox-browser");
  parsedUrl.searchParams.set("utm_content", contentAttribute);
  return parsedUrl.href;
}

class PanelList extends HTMLElement {
  static get observedAttributes() {
    return ["open"];
  }

  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    this.shadowRoot.appendChild(importTemplate("panel-list"));
    this.setAttribute("role", "menu");
  }

  attributeChangedCallback(name, oldVal, newVal) {
    if (name == "open" && newVal != oldVal) {
      if (this.open) {
        this.onShow();
      } else {
        this.onHide();
      }
    }
  }

  get open() {
    return this.hasAttribute("open");
  }

  set open(val) {
    this.toggleAttribute("open", val);
  }

  show(triggeringEvent) {
    this.triggeringEvent = triggeringEvent;
    this.open = true;
  }

  hide(triggeringEvent) {
    this.triggeringEvent = triggeringEvent;
    this.open = false;
  }

  toggle(triggeringEvent) {
    if (this.open) {
      this.hide(triggeringEvent);
    } else {
      this.show(triggeringEvent);
    }
  }

  async setAlign() {
    // Set the showing attribute to hide the panel until its alignment is set.
    this.setAttribute("showing", "true");
    // Tell the parent node to hide any overflow in case the panel extends off
    // the page before the alignment is set.
    this.parentNode.style.overflow = "hidden";

    // Wait for a layout flush, then find the bounds.
    let {
      height,
      width,
      y,
      left,
      right,
      winHeight,
      winWidth,
    } = await new Promise(resolve => {
      requestAnimationFrame(() =>
        setTimeout(() => {
          // Use y since top is reserved.
          let { y, left, right } = window.windowUtils.getBoundsWithoutFlushing(
            this.parentNode
          );
          let { height, width } = window.windowUtils.getBoundsWithoutFlushing(
            this
          );
          resolve({
            height,
            width,
            y,
            left,
            right,
            winHeight: innerHeight,
            winWidth: innerWidth,
          });
        }, 0)
      );
    });

    // Calculate the left/right alignment.
    let align;
    if (Services.locale.isAppLocaleRTL) {
      // Prefer aligning on the right.
      align = right - width + 14 < 0 ? "left" : "right";
    } else {
      // Prefer aligning on the left.
      align = left + width - 14 > winWidth ? "right" : "left";
    }

    // "bottom" style will move the panel down 30px from the top of the parent.
    let valign = y + height + 30 > winHeight ? "top" : "bottom";

    // Set the alignments and show the panel.
    this.setAttribute("align", align);
    this.setAttribute("valign", valign);
    this.parentNode.style.overflow = "";
    this.removeAttribute("showing");
  }

  addHideListeners() {
    // Hide when a panel-item is clicked in the list.
    this.addEventListener("click", this);
    document.addEventListener("keydown", this);
    // Hide when a click is initiated outside the panel.
    document.addEventListener("mousedown", this);
    // Hide if focus changes and the panel isn't in focus.
    document.addEventListener("focusin", this);
    // Reset or focus tracking, we treat the first focusin differently.
    this.focusHasChanged = false;
    // Hide on resize, scroll or losing window focus.
    window.addEventListener("resize", this);
    window.addEventListener("scroll", this);
    window.addEventListener("blur", this);
  }

  removeHideListeners() {
    this.removeEventListener("click", this);
    document.removeEventListener("keydown", this);
    document.removeEventListener("mousedown", this);
    document.removeEventListener("focusin", this);
    window.removeEventListener("resize", this);
    window.removeEventListener("scroll", this);
    window.removeEventListener("blur", this);
  }

  handleEvent(e) {
    // Ignore the event if it caused the panel to open.
    if (e == this.triggeringEvent) {
      return;
    }

    switch (e.type) {
      case "resize":
      case "scroll":
      case "blur":
        this.hide();
        break;
      case "click":
        if (e.target.tagName == "PANEL-ITEM") {
          this.hide();
        } else {
          // Avoid falling through to the default click handler of the
          // add-on card, which would expand the add-on card.
          e.stopPropagation();
        }
        break;
      case "keydown":
        if (e.key === "ArrowDown" || e.key === "ArrowUp" || e.key === "Tab") {
          // Ignore tabbing with a modifer other than shift.
          if (e.key === "Tab" && (e.altKey || e.ctrlKey || e.metaKey)) {
            return;
          }

          // Don't scroll the page or let the regular tab order take effect.
          e.preventDefault();

          // Keep moving to the next/previous element sibling until we find a
          // panel-item that isn't hidden.
          let moveForward =
            e.key === "ArrowDown" || (e.key === "Tab" && !e.shiftKey);

          // If the menu is opened with the mouse, the active element might be
          // somewhere else in the document. In that case we should ignore it
          // to avoid walking unrelated DOM nodes.
          this.walker.currentNode = this.contains(document.activeElement)
            ? document.activeElement
            : this;
          let nextItem = moveForward
            ? this.walker.nextNode()
            : this.walker.previousNode();

          // If the next item wasn't found, try looping to the top/bottom.
          if (!nextItem) {
            this.walker.currentNode = this;
            if (moveForward) {
              nextItem = this.walker.firstChild();
            } else {
              nextItem = this.walker.lastChild();
            }
          }

          if (nextItem) {
            nextItem.focus();
          }
          break;
        } else if (e.key === "Escape") {
          let { triggeringEvent } = this;
          this.hide();
          if (triggeringEvent && triggeringEvent.target) {
            triggeringEvent.target.focus();
          }
        }
        break;
      case "mousedown":
      case "focusin":
        // There will be a focusin after the mousedown that opens the panel
        // using the mouse. Ignore the first focusin event if it's on the
        // triggering target.
        if (
          this.triggeringEvent &&
          e.target == this.triggeringEvent.target &&
          !this.focusHasChanged
        ) {
          this.focusHasChanged = true;
          // If the target isn't in the panel, hide. This will close when focus
          // moves out of the panel, or there's a click started outside the
          // panel.
        } else if (!e.target || e.target.closest("panel-list") != this) {
          this.hide();
          // Just record that there was a focusin event.
        } else {
          this.focusHasChanged = true;
        }
        break;
    }
  }

  get walker() {
    if (!this._walker) {
      this._walker = document.createTreeWalker(this, NodeFilter.SHOW_ELEMENT, {
        acceptNode: node => {
          if (node.disabled || node.hidden || node.localName !== "panel-item") {
            return NodeFilter.FILTER_REJECT;
          }

          return NodeFilter.FILTER_ACCEPT;
        },
      });
    }
    return this._walker;
  }

  async onShow() {
    let { triggeringEvent } = this;

    this.addHideListeners();
    await this.setAlign();

    // Wait until the next paint for the alignment to be set and panel to be
    // visible.
    requestAnimationFrame(() => {
      // Focus the first visible panel-item if we were opened with the keyboard.
      if (
        triggeringEvent &&
        triggeringEvent.mozInputSource === MouseEvent.MOZ_SOURCE_KEYBOARD
      ) {
        this.walker.currentNode = this;
        let firstItem = this.walker.nextNode();
        if (firstItem) {
          firstItem.focus();
        }
      }

      this.sendEvent("shown");
    });
  }

  onHide() {
    requestAnimationFrame(() => this.sendEvent("hidden"));
    this.removeHideListeners();
  }

  sendEvent(name, detail) {
    this.dispatchEvent(new CustomEvent(name, { detail }));
  }
}
customElements.define("panel-list", PanelList);

class PanelItem extends HTMLElement {
  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    this.shadowRoot.appendChild(importTemplate("panel-item"));
    this.button = this.shadowRoot.querySelector("button");
    this.button.setAttribute("role", "menuitem");
  }

  get disabled() {
    return this.button.hasAttribute("disabled");
  }

  set disabled(val) {
    this.button.toggleAttribute("disabled", val);
  }

  get checked() {
    return this.hasAttribute("checked");
  }

  set checked(val) {
    this.toggleAttribute("checked", val);
  }

  focus() {
    this.button.focus();
  }
}
customElements.define("panel-item", PanelItem);

class AddonOptions extends HTMLElement {
  connectedCallback() {
    if (this.children.length == 0) {
      this.render();
    }
  }

  get panel() {
    return this.querySelector("panel-list");
  }

  updateSeparatorsVisibility() {
    let lastSeparator;
    let elWasVisible = false;

    // Collect the panel-list children that are not already hidden.
    const children = Array.from(this.panel.children).filter(el => !el.hidden);

    for (let child of children) {
      if (child.tagName == "PANEL-ITEM-SEPARATOR") {
        child.hidden = !elWasVisible;
        if (!child.hidden) {
          lastSeparator = child;
        }
        elWasVisible = false;
      } else {
        elWasVisible = true;
      }
    }
    if (!elWasVisible && lastSeparator) {
      lastSeparator.hidden = true;
    }
  }

  get template() {
    return "addon-options";
  }

  render() {
    this.appendChild(importTemplate(this.template));
  }

  setElementState(el, card, addon, updateInstall) {
    switch (el.getAttribute("action")) {
      case "remove":
        el.hidden = !hasPermission(addon, "uninstall");
        break;
      case "report":
        el.hidden = !isAbuseReportSupported(addon);
        break;
      case "toggle-disabled": {
        let toggleDisabledAction = addon.userDisabled ? "enable" : "disable";
        document.l10n.setAttributes(el, `${toggleDisabledAction}-addon-button`);
        el.hidden = !hasPermission(addon, toggleDisabledAction);
        break;
      }
      case "install-update":
        el.hidden = !updateInstall;
        break;
      case "expand":
        el.hidden = card.expanded;
        break;
      case "preferences":
        el.hidden =
          getOptionsType(addon) !== "tab" &&
          (getOptionsType(addon) !== "inline" || card.expanded);
        if (!el.hidden) {
          isAddonOptionsUIAllowed(addon).then(allowed => {
            el.hidden = !allowed;
          });
        }
        break;
    }
  }

  update(card, addon, updateInstall) {
    for (let el of this.items) {
      this.setElementState(el, card, addon, updateInstall);
    }

    // Update the separators visibility based on the updated visibility
    // of the actions in the panel-list.
    this.updateSeparatorsVisibility();
  }

  get items() {
    return this.querySelectorAll("panel-item");
  }

  get visibleItems() {
    return Array.from(this.items).filter(item => !item.hidden);
  }
}
customElements.define("addon-options", AddonOptions);

class PluginOptions extends AddonOptions {
  get template() {
    return "plugin-options";
  }

  setElementState(el, card, addon) {
    const userDisabledStates = {
      "ask-to-activate": AddonManager.STATE_ASK_TO_ACTIVATE,
      "always-activate": false,
      "never-activate": true,
    };
    const action = el.getAttribute("action");
    if (action in userDisabledStates) {
      let userDisabled = userDisabledStates[action];
      el.checked = addon.userDisabled === userDisabled;
      el.disabled = !(el.checked || hasPermission(addon, action));
    } else {
      super.setElementState(el, card, addon);
    }
  }
}
customElements.define("plugin-options", PluginOptions);

class FiveStarRating extends HTMLElement {
  static get observedAttributes() {
    return ["rating"];
  }

  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    this.shadowRoot.append(importTemplate("five-star-rating"));
  }

  set rating(v) {
    this.setAttribute("rating", v);
  }

  get rating() {
    let v = parseFloat(this.getAttribute("rating"), 10);
    if (v >= 0 && v <= 5) {
      return v;
    }
    return 0;
  }

  get ratingBuckets() {
    // 0    <= x <  0.25 = empty
    // 0.25 <= x <  0.75 = half
    // 0.75 <= x <= 1    = full
    // ... et cetera, until x <= 5.
    let { rating } = this;
    return [0, 1, 2, 3, 4].map(ratingStart => {
      let distanceToFull = rating - ratingStart;
      if (distanceToFull < 0.25) {
        return "empty";
      }
      if (distanceToFull < 0.75) {
        return "half";
      }
      return "full";
    });
  }

  connectedCallback() {
    this.renderRating();
  }

  attributeChangedCallback() {
    this.renderRating();
  }

  renderRating() {
    let starElements = this.shadowRoot.querySelectorAll(".rating-star");
    for (let [i, part] of this.ratingBuckets.entries()) {
      starElements[i].setAttribute("fill", part);
    }
    document.l10n.setAttributes(this, "five-star-rating", {
      rating: this.rating,
    });
  }
}
customElements.define("five-star-rating", FiveStarRating);

class ContentSelectDropdown extends HTMLElement {
  connectedCallback() {
    if (this.children.length > 0) {
      return;
    }
    // This creates the menulist and menupopup elements needed for the inline
    // browser to support <select> elements and context menus.
    this.appendChild(
      MozXULElement.parseXULToFragment(`
      <menulist popuponly="true" id="ContentSelectDropdown" hidden="true">
        <menupopup rolluponmousewheel="true" activateontab="true"
                   position="after_start" level="parent"/>
      </menulist>
    `)
    );
  }
}
customElements.define("content-select-dropdown", ContentSelectDropdown);

class ProxyContextMenu extends HTMLElement {
  openPopupAtScreen(...args) {
    // prettier-ignore
    const parentContextMenuPopup =
      windowRoot.ownerGlobal.document.getElementById("contentAreaContextMenu");
    return parentContextMenuPopup.openPopupAtScreen(...args);
  }
}
customElements.define("proxy-context-menu", ProxyContextMenu);

class InlineOptionsBrowser extends HTMLElement {
  constructor() {
    super();
    // Force the options_ui remote browser to recompute window.mozInnerScreenX
    // and window.mozInnerScreenY when the "addon details" page has been
    // scrolled (See Bug 1390445 for rationale).
    // Also force a repaint to fix an issue where the click location was
    // getting out of sync (see bug 1548687).
    this.updatePositionTask = new DeferredTask(() => {
      if (this.browser && this.browser.isRemoteBrowser) {
        // Select boxes can appear in the wrong spot after scrolling, this will
        // clear that up. Bug 1390445.
        this.browser.frameLoader.requestUpdatePosition();
        // Sometimes after scrolling the inner brower needs to repaint to get
        // the right mouse position. Force that to happen. Bug 1548687.
        window.windowUtils.flushApzRepaints();
      }
    }, 100);
  }

  connectedCallback() {
    window.addEventListener("scroll", this, true);
  }

  disconnectedCallback() {
    window.removeEventListener("scroll", this, true);
  }

  handleEvent(e) {
    if (e.type == "scroll") {
      this.updatePositionTask.arm();
    }
  }

  setAddon(addon) {
    this.addon = addon;
  }

  destroyBrowser() {
    this.textContent = "";
  }

  ensureBrowserCreated() {
    if (this.childElementCount === 0) {
      this.render();
    }
  }

  async render() {
    let { addon } = this;
    if (!addon) {
      throw new Error("addon required to create inline options");
    }

    let browser = document.createXULElement("browser");
    browser.setAttribute("type", "content");
    browser.setAttribute("disableglobalhistory", "true");
    browser.setAttribute("id", "addon-inline-options");
    browser.setAttribute("transparent", "true");
    browser.setAttribute("forcemessagemanager", "true");
    browser.setAttribute("selectmenulist", "ContentSelectDropdown");
    browser.setAttribute("autocompletepopup", "PopupAutoComplete");

    // The outer about:addons document listens for key presses to focus
    // the search box when / is pressed.  But if we're focused inside an
    // options page, don't let those keypresses steal focus.
    browser.addEventListener("keypress", event => {
      event.stopPropagation();
    });

    let { optionsURL, optionsBrowserStyle } = addon;
    if (addon.isWebExtension) {
      let policy = ExtensionParent.WebExtensionPolicy.getByID(addon.id);
      browser.sameProcessAsFrameLoader = policy.extension.groupFrameLoader;
    }

    let readyPromise;
    let remoteSubframes = window.docShell.QueryInterface(Ci.nsILoadContext)
      .useRemoteSubframes;
    let loadRemote = E10SUtils.canLoadURIInRemoteType(
      optionsURL,
      remoteSubframes,
      E10SUtils.EXTENSION_REMOTE_TYPE
    );
    if (loadRemote) {
      browser.setAttribute("remote", "true");
      browser.setAttribute("remoteType", E10SUtils.EXTENSION_REMOTE_TYPE);

      readyPromise = promiseEvent("XULFrameLoaderCreated", browser);

      readyPromise.then(() => {
        if (!browser.messageManager) {
          // Early exit if the the extension page's XUL browser has been
          // destroyed in the meantime (e.g. because the extension has been
          // reloaded while the options page was still loading).
          return;
        }

        // Subscribe a "contextmenu" listener to handle the context menus for
        // the extension option page running in the extension process (the
        // context menu will be handled only for extension running in OOP mode,
        // but that's ok as it is the default on any platform that uses these
        // extensions options pages).
        browser.messageManager.addMessageListener("contextmenu", message => {
          windowRoot.ownerGlobal.openContextMenu(message);
        });
      });
    } else {
      readyPromise = promiseEvent("load", browser, true);
    }

    let stack = document.createXULElement("stack");
    stack.classList.add("inline-options-stack");
    stack.appendChild(browser);
    this.appendChild(stack);
    this.browser = browser;

    // Force bindings to apply synchronously.
    browser.clientTop;

    await readyPromise;

    if (!browser.messageManager) {
      // If the browser.messageManager is undefined, the browser element has
      // been removed from the document in the meantime (e.g. due to a rapid
      // sequence of addon reload), return null.
      return;
    }

    ExtensionParent.apiManager.emit("extension-browser-inserted", browser);

    await new Promise(resolve => {
      let messageListener = {
        receiveMessage({ name, data }) {
          if (name === "Extension:BrowserResized") {
            // Add a pixel to work around a scrolling issue, bug 1548687.
            browser.style.height = `${data.height + 1}px`;
          } else if (name === "Extension:BrowserContentLoaded") {
            resolve();
          }
        },
      };

      let mm = browser.messageManager;

      if (!mm) {
        // If the browser.messageManager is undefined, the browser element has
        // been removed from the document in the meantime (e.g. due to a rapid
        // sequence of addon reload), return null.
        resolve();
        return;
      }

      mm.loadFrameScript(
        "chrome://extensions/content/ext-browser-content.js",
        false,
        true
      );
      mm.loadFrameScript("chrome://browser/content/content.js", false, true);
      mm.addMessageListener("Extension:BrowserContentLoaded", messageListener);
      mm.addMessageListener("Extension:BrowserResized", messageListener);

      let browserOptions = {
        fixedWidth: true,
        isInline: true,
      };

      if (optionsBrowserStyle) {
        browserOptions.stylesheets = extensionStylesheets;
      }

      mm.sendAsyncMessage("Extension:InitBrowser", browserOptions);

      // prettier-ignore
      browser.loadURI(optionsURL, {
        triggeringPrincipal:
          Services.scriptSecurityManager.getSystemPrincipal(),
      });
    });
  }
}
customElements.define("inline-options-browser", InlineOptionsBrowser);

class UpdateReleaseNotes extends HTMLElement {
  connectedCallback() {
    this.addEventListener("click", this);
  }

  disconnectedCallback() {
    this.removeEventListener("click", this);
  }

  handleEvent(e) {
    // We used to strip links, but ParserUtils.parseFragment() leaves them in,
    // so just make sure we open them using the null principal in a new tab.
    if (e.type == "click" && e.target.localName == "a" && e.target.href) {
      e.preventDefault();
      e.stopPropagation();
      windowRoot.ownerGlobal.openWebLinkIn(e.target.href, "tab");
    }
  }

  async loadForUri(uri) {
    // Can't load the release notes without a URL to load.
    if (!uri || !uri.spec) {
      this.setErrorMessage();
      this.dispatchEvent(new CustomEvent("release-notes-error"));
      return;
    }

    // Don't try to load for the same update a second time.
    if (this.url == uri.spec) {
      this.dispatchEvent(new CustomEvent("release-notes-cached"));
      return;
    }

    // Store the URL to skip the network if loaded again.
    this.url = uri.spec;

    // Set the loading message before hitting the network.
    this.setLoadingMessage();
    this.dispatchEvent(new CustomEvent("release-notes-loading"));

    try {
      // loadReleaseNotes will fetch and sanitize the release notes.
      let fragment = await loadReleaseNotes(uri);
      this.textContent = "";
      this.appendChild(fragment);
      this.dispatchEvent(new CustomEvent("release-notes-loaded"));
    } catch (e) {
      this.setErrorMessage();
      this.dispatchEvent(new CustomEvent("release-notes-error"));
    }
  }

  setMessage(id) {
    this.textContent = "";
    let message = document.createElement("p");
    document.l10n.setAttributes(message, id);
    this.appendChild(message);
  }

  setLoadingMessage() {
    this.setMessage("release-notes-loading");
  }

  setErrorMessage() {
    this.setMessage("release-notes-error");
  }
}
customElements.define("update-release-notes", UpdateReleaseNotes);

class AddonPermissionsList extends HTMLElement {
  setAddon(addon) {
    this.addon = addon;
    this.render();
  }

  render() {
    let appName = brandBundle.GetStringFromName("brandShortName");
    let { msgs } = Extension.formatPermissionStrings(
      {
        permissions: this.addon.userPermissions,
        appName,
      },
      browserBundle
    );

    this.textContent = "";

    if (msgs.length > 0) {
      // Add a row for each permission message.
      for (let msg of msgs) {
        let row = document.createElement("div");
        row.classList.add("addon-detail-row", "permission-info");
        row.textContent = msg;
        this.appendChild(row);
      }
    } else {
      let emptyMessage = document.createElement("div");
      emptyMessage.classList.add("addon-detail-row");
      document.l10n.setAttributes(emptyMessage, "addon-permissions-empty");
      this.appendChild(emptyMessage);
    }

    // Add a learn more link.
    let learnMoreRow = document.createElement("div");
    learnMoreRow.classList.add("addon-detail-row");
    let learnMoreLink = document.createElement("a");
    learnMoreLink.setAttribute("target", "_blank");
    learnMoreLink.href = SUPPORT_URL + "extension-permissions";
    learnMoreLink.textContent = browserBundle.GetStringFromName(
      "webextPerms.learnMore"
    );
    learnMoreRow.appendChild(learnMoreLink);
    this.appendChild(learnMoreRow);
  }
}
customElements.define("addon-permissions-list", AddonPermissionsList);

class AddonDetails extends HTMLElement {
  connectedCallback() {
    if (this.children.length == 0) {
      this.render();
    }
    this.deck.addEventListener("view-changed", this);
    this.addEventListener("keypress", this);
  }

  disconnectedCallback() {
    this.inlineOptions.destroyBrowser();
    this.deck.removeEventListener("view-changed", this);
    this.removeEventListener("keypress", this);
  }

  handleEvent(e) {
    if (e.type == "view-changed" && e.target == this.deck) {
      switch (this.deck.selectedViewName) {
        case "release-notes":
          AMTelemetry.recordActionEvent({
            object: "aboutAddons",
            view: getTelemetryViewName(this),
            action: "releaseNotes",
            addon: this.addon,
          });
          let releaseNotes = this.querySelector("update-release-notes");
          let uri = this.releaseNotesUri;
          if (uri) {
            releaseNotes.loadForUri(uri);
          }
          break;
        case "preferences":
          if (getOptionsType(this.addon) == "inline") {
            this.inlineOptions.ensureBrowserCreated();
          }
          break;
      }
    } else if (e.type == "keypress") {
      if (
        e.keyCode == KeyEvent.DOM_VK_RETURN &&
        e.target.getAttribute("action") === "pb-learn-more"
      ) {
        e.target.click();
      }
    }
  }

  get releaseNotesUri() {
    return this.addon.updateInstall
      ? this.addon.updateInstall.releaseNotesURI
      : this.addon.releaseNotesURI;
  }

  setAddon(addon) {
    this.addon = addon;
  }

  update() {
    let { addon } = this;

    // Hide tab buttons that won't have any content.
    let getButtonByName = name =>
      this.tabGroup.querySelector(`[name="${name}"]`);
    let permsBtn = getButtonByName("permissions");
    permsBtn.hidden = addon.type != "extension";
    let notesBtn = getButtonByName("release-notes");
    notesBtn.hidden = !this.releaseNotesUri;
    let prefsBtn = getButtonByName("preferences");
    prefsBtn.hidden = getOptionsType(addon) !== "inline";
    if (!prefsBtn.hidden) {
      isAddonOptionsUIAllowed(addon).then(allowed => {
        prefsBtn.hidden = !allowed;
      });
    }

    // Hide the tab group if "details" is the only visible button.
    let tabGroupButtons = this.tabGroup.querySelectorAll("named-deck-button");
    this.tabGroup.hidden = Array.from(tabGroupButtons).every(button => {
      return button.name == "details" || button.hidden;
    });

    // Show the update check button if necessary. The button might not exist if
    // the add-on doesn't support updates.
    let updateButton = this.querySelector('[action="update-check"]');
    if (updateButton) {
      updateButton.hidden =
        this.addon.updateInstall || AddonManager.shouldAutoUpdate(this.addon);
    }

    // Set the value for auto updates.
    let inputs = this.querySelectorAll(".addon-detail-row-updates input");
    for (let input of inputs) {
      input.checked = input.value == addon.applyBackgroundUpdates;
    }
  }

  async render() {
    let { addon } = this;
    if (!addon) {
      throw new Error("addon-details must be initialized by setAddon");
    }

    this.textContent = "";
    this.appendChild(importTemplate("addon-details"));

    this.deck = this.querySelector("named-deck");
    this.tabGroup = this.querySelector(".deck-tab-group");

    // Set the add-on for the permissions section.
    this.permissionsList = this.querySelector("addon-permissions-list");
    this.permissionsList.setAddon(addon);

    // Set the add-on for the preferences section.
    this.inlineOptions = this.querySelector("inline-options-browser");
    this.inlineOptions.setAddon(addon);

    // Full description.
    let description = this.querySelector(".addon-detail-description");
    if (addon.getFullDescription) {
      description.appendChild(addon.getFullDescription(document));
    } else if (addon.fullDescription) {
      description.appendChild(nl2br(addon.fullDescription));
    }

    this.querySelector(
      ".addon-detail-contribute"
    ).hidden = !addon.contributionURL;
    this.querySelector(".addon-detail-row-updates").hidden = !hasPermission(
      addon,
      "upgrade"
    );

    // By default, all private browsing rows are hidden. Possibly show one.
    if (allowPrivateBrowsingByDefault || addon.type != "extension") {
      // All add-addons of this type are allowed in private browsing mode, so
      // do not show any UI.
    } else if (addon.incognito == "not_allowed") {
      let pbRowNotAllowed = this.querySelector(
        ".addon-detail-row-private-browsing-disallowed"
      );
      pbRowNotAllowed.hidden = false;
      pbRowNotAllowed.nextElementSibling.hidden = false;
    } else if (!hasPermission(addon, "change-privatebrowsing")) {
      let pbRowRequired = this.querySelector(
        ".addon-detail-row-private-browsing-required"
      );
      pbRowRequired.hidden = false;
      pbRowRequired.nextElementSibling.hidden = false;
    } else {
      let pbRow = this.querySelector(".addon-detail-row-private-browsing");
      pbRow.hidden = false;
      pbRow.nextElementSibling.hidden = false;
      let isAllowed = await isAllowedInPrivateBrowsing(addon);
      pbRow.querySelector(`[value="${isAllowed ? 1 : 0}"]`).checked = true;
      let learnMore = pbRow.nextElementSibling.querySelector(
        'a[data-l10n-name="learn-more"]'
      );
      learnMore.href = SUPPORT_URL + "extensions-pb";
    }

    // Author.
    let creatorRow = this.querySelector(".addon-detail-row-author");
    if (addon.creator) {
      let link = creatorRow.querySelector("a");
      link.hidden = !addon.creator.url;
      if (link.hidden) {
        creatorRow.appendChild(new Text(addon.creator.name));
      } else {
        link.href = addon.creator.url;
        link.target = "_blank";
        link.textContent = addon.creator.name;
      }
    } else {
      creatorRow.hidden = true;
    }

    // Version. Don't show a version for LWTs.
    let version = this.querySelector(".addon-detail-row-version");
    if (addon.version && !/@personas\.mozilla\.org/.test(addon.id)) {
      version.appendChild(new Text(addon.version));
    } else {
      version.hidden = true;
    }

    // Last updated.
    let updateDate = this.querySelector(".addon-detail-row-lastUpdated");
    if (addon.updateDate) {
      let lastUpdated = addon.updateDate.toLocaleDateString(undefined, {
        year: "numeric",
        month: "long",
        day: "numeric",
      });
      updateDate.appendChild(new Text(lastUpdated));
    } else {
      updateDate.hidden = true;
    }

    // Homepage.
    let homepageRow = this.querySelector(".addon-detail-row-homepage");
    if (addon.homepageURL) {
      let homepageURL = homepageRow.querySelector("a");
      homepageURL.href = addon.homepageURL;
      homepageURL.textContent = addon.homepageURL;
    } else {
      homepageRow.hidden = true;
    }

    // Rating.
    let ratingRow = this.querySelector(".addon-detail-row-rating");
    if (addon.averageRating) {
      ratingRow.querySelector("five-star-rating").rating = addon.averageRating;
      let reviews = ratingRow.querySelector("a");
      reviews.href = addon.reviewURL;
      document.l10n.setAttributes(reviews, "addon-detail-reviews-link", {
        numberOfReviews: addon.reviewCount,
      });
    } else {
      ratingRow.hidden = true;
    }

    this.update();
  }

  showPrefs() {
    if (getOptionsType(this.addon) == "inline") {
      this.deck.selectedViewName = "preferences";
      this.inlineOptions.ensureBrowserCreated();
    }
  }
}
customElements.define("addon-details", AddonDetails);

/**
 * A card component for managing an add-on. It should be initialized by setting
 * the add-on with `setAddon()` before being connected to the document.
 *
 *    let card = document.createElement("addon-card");
 *    card.setAddon(addon);
 *    document.body.appendChild(card);
 */
class AddonCard extends HTMLElement {
  connectedCallback() {
    // If we've already rendered we can just update, otherwise render.
    if (this.children.length > 0) {
      this.update();
    } else {
      this.render();
    }
    this.registerListeners();
  }

  disconnectedCallback() {
    this.removeListeners();
  }

  get expanded() {
    return this.hasAttribute("expanded");
  }

  set expanded(val) {
    if (val) {
      this.setAttribute("expanded", "true");
    } else {
      this.removeAttribute("expanded");
    }
  }

  get updateInstall() {
    return this._updateInstall;
  }

  set updateInstall(install) {
    this._updateInstall = install;
    if (this.children.length > 0) {
      this.update();
    }
  }

  get reloading() {
    return this.hasAttribute("reloading");
  }

  set reloading(val) {
    if (val) {
      this.setAttribute("reloading", "true");
    } else {
      this.removeAttribute("reloading");
    }
  }

  /**
   * Set the add-on for this card. The card will be populated based on the
   * add-on when it is connected to the DOM.
   *
   * @param {AddonWrapper} addon The add-on to use.
   */
  setAddon(addon) {
    this.addon = addon;
    let install = addon.updateInstall;
    if (install && install.state == AddonManager.STATE_AVAILABLE) {
      this.updateInstall = install;
    }
    if (this.children.length > 0) {
      this.render();
    }
  }

  async handleEvent(e) {
    let { addon } = this;
    let action = e.target.getAttribute("action");

    if (e.type == "click") {
      switch (action) {
        case "toggle-disabled":
          this.recordActionEvent(addon.userDisabled ? "enable" : "disable");
          if (addon.userDisabled) {
            if (shouldShowPermissionsPrompt(addon)) {
              await showPermissionsPrompt(addon);
            } else {
              await addon.enable();
            }
          } else {
            await addon.disable();
          }
          if (e.mozInputSource == MouseEvent.MOZ_SOURCE_KEYBOARD) {
            // Refocus the open menu button so it's clear where the focus is.
            this.querySelector('[action="more-options"]').focus();
          }
          break;
        case "ask-to-activate":
          if (hasPermission(addon, "ask-to-activate")) {
            addon.userDisabled = AddonManager.STATE_ASK_TO_ACTIVATE;
          }
          break;
        case "always-activate":
          this.recordActionEvent("enable");
          addon.userDisabled = false;
          break;
        case "never-activate":
          this.recordActionEvent("disable");
          addon.userDisabled = true;
          break;
        case "update-check":
          this.recordActionEvent("checkForUpdate");
          let listener = {
            onUpdateAvailable(addon, install) {
              attachUpdateHandler(install);
            },
            onNoUpdateAvailable: () => {
              this.sendEvent("no-update");
            },
          };
          addon.findUpdates(listener, AddonManager.UPDATE_WHEN_USER_REQUESTED);
          break;
        case "install-update":
          this.updateInstall.install().then(
            () => {
              // The card will update with the new add-on when it gets
              // installed.
              this.sendEvent("update-installed");
            },
            () => {
              // Update our state if the install is cancelled.
              this.update();
              this.sendEvent("update-cancelled");
            }
          );
          // Clear the install since it will be removed from the global list of
          // available updates (whether it succeeds or fails).
          this.updateInstall = null;
          break;
        case "contribute":
          this.recordActionEvent("contribute");
          // prettier-ignore
          windowRoot.ownerGlobal.openUILinkIn(addon.contributionURL, "tab", {
            triggeringPrincipal:
              Services.scriptSecurityManager.createNullPrincipal(
                {}
              ),
          });
          break;
        case "preferences":
          if (getOptionsType(addon) == "tab") {
            this.recordActionEvent("preferences", "external");
            openOptionsInTab(addon.optionsURL);
          } else if (getOptionsType(addon) == "inline") {
            this.recordActionEvent("preferences", "inline");
            loadViewFn(`detail/${this.addon.id}/preferences`, e);
          }
          break;
        case "remove":
          {
            this.panel.hide();
            let {
              remove,
              report,
            } = windowRoot.ownerGlobal.promptRemoveExtension(addon);
            let value = remove ? "accepted" : "cancelled";
            this.recordActionEvent("uninstall", value);
            if (remove) {
              await addon.uninstall(true);
              this.sendEvent("remove");
              if (report) {
                openAbuseReport({
                  addonId: addon.id,
                  reportEntryPoint: "uninstall",
                });
              }
            } else {
              this.sendEvent("remove-cancelled");
            }
          }
          break;
        case "expand":
          loadViewFn(`detail/${this.addon.id}`, e);
          break;
        case "more-options":
          // Open panel on click from the keyboard.
          if (e.mozInputSource == MouseEvent.MOZ_SOURCE_KEYBOARD) {
            this.panel.toggle(e);
          }
          break;
        case "report":
          this.panel.hide();
          openAbuseReport({ addonId: addon.id, reportEntryPoint: "menu" });
          break;
        case "link":
          if (e.target.getAttribute("url")) {
            windowRoot.ownerGlobal.openWebLinkIn(
              e.target.getAttribute("url"),
              "tab"
            );
          }
          break;
        case "pb-learn-more":
          windowRoot.ownerGlobal.openTrustedLinkIn(
            SUPPORT_URL + "extensions-pb",
            "tab"
          );
          break;
        default:
          // Handle a click on the card itself.
          if (!this.expanded) {
            loadViewFn(`detail/${this.addon.id}`, e);
          } else if (
            e.target.localName == "a" &&
            e.target.getAttribute("data-telemetry-name")
          ) {
            let value = e.target.getAttribute("data-telemetry-name");
            AMTelemetry.recordLinkEvent({
              object: "aboutAddons",
              addon,
              value,
              extra: {
                view: getTelemetryViewName(this),
              },
            });
          }
          break;
      }
    } else if (e.type == "change") {
      let { name } = e.target;
      let telemetryValue = e.target.getAttribute("data-telemetry-value");
      if (name == "autoupdate") {
        this.recordActionEvent("setAddonUpdate", telemetryValue);
        addon.applyBackgroundUpdates = e.target.value;
      } else if (name == "private-browsing") {
        this.recordActionEvent("privateBrowsingAllowed", telemetryValue);
        let policy = WebExtensionPolicy.getByID(addon.id);
        let extension = policy && policy.extension;

        if (e.target.value == "1") {
          await ExtensionPermissions.add(
            addon.id,
            PRIVATE_BROWSING_PERMS,
            extension
          );
        } else {
          await ExtensionPermissions.remove(
            addon.id,
            PRIVATE_BROWSING_PERMS,
            extension
          );
        }
        // Reload the extension if it is already enabled. This ensures any
        // change on the private browsing permission is properly handled.
        if (addon.isActive) {
          this.reloading = true;
          // Reloading will trigger an enable and update the card.
          addon.reload();
        } else {
          // Update the card if the add-on isn't active.
          this.update();
        }
      }
    } else if (e.type == "mousedown") {
      // Open panel on mousedown when the mouse is used.
      if (action == "more-options") {
        this.panel.toggle(e);
      }
    } else if (e.type === "shown" || e.type === "hidden") {
      let panelOpen = e.type === "shown";
      this.optionsButton.setAttribute("aria-expanded", panelOpen);
    }
  }

  get panel() {
    return this.card.querySelector("panel-list");
  }

  registerListeners() {
    this.addEventListener("change", this);
    this.addEventListener("click", this);
    this.addEventListener("mousedown", this);
    this.panel.addEventListener("shown", this);
    this.panel.addEventListener("hidden", this);
  }

  removeListeners() {
    this.removeEventListener("change", this);
    this.removeEventListener("click", this);
    this.removeEventListener("mousedown", this);
    this.panel.removeEventListener("shown", this);
    this.panel.removeEventListener("hidden", this);
  }

  onNewInstall(install) {
    this.updateInstall = install;
    this.sendEvent("update-found");
  }

  onInstallEnded(install) {
    this.setAddon(install.addon);
  }

  onDisabled(addon) {
    if (!this.reloading) {
      this.update();
    }
  }

  onEnabled(addon) {
    this.reloading = false;
    this.update();
  }

  onInstalled(addon) {
    // When a temporary addon is reloaded, onInstalled is triggered instead of
    // onEnabled.
    this.reloading = false;
    this.update();
  }

  onUninstalling() {
    // Dispatch a remove event, the DetailView is listening for this to get us
    // back to the list view when the current add-on is removed.
    this.sendEvent("remove");
  }

  onUpdateModeChanged() {
    this.update();
  }

  onPropertyChanged(addon, changed) {
    if (this.details && changed.includes("applyBackgroundUpdates")) {
      this.details.update();
    } else if (addon.type == "plugin" && changed.includes("userDisabled")) {
      this.update();
    }
  }

  /**
   * Update the card's contents based on the previously set add-on. This should
   * be called if there has been a change to the add-on.
   */
  update() {
    let { addon, card } = this;

    // Update the icon.
    let icon;
    if (addon.type == "plugin") {
      icon = PLUGIN_ICON_URL;
    } else {
      icon =
        AddonManager.getPreferredIconURL(addon, 32, window) ||
        EXTENSION_ICON_URL;
    }
    card.querySelector(".addon-icon").src = icon;

    // Update the theme preview.
    let preview = card.querySelector(".card-heading-image");
    preview.hidden = true;
    if (addon.type == "theme") {
      let screenshotUrl = getScreenshotUrlForAddon(addon);
      if (screenshotUrl) {
        preview.src = screenshotUrl;
        preview.hidden = false;
      }
    }

    // Update the name.
    let name = this.addonNameEl;
    if (addon.isActive) {
      name.textContent = addon.name;
      name.removeAttribute("data-l10n-id");
    } else {
      document.l10n.setAttributes(name, "addon-name-disabled", {
        name: addon.name,
      });
    }
    name.title = `${addon.name} ${addon.version}`;

    // Set the items in the more options menu.
    this.options.update(this, addon, this.updateInstall);

    // Badge the more options button if there's an update.
    let moreOptionsButton = card.querySelector(".more-options-button");
    moreOptionsButton.classList.toggle(
      "more-options-button-badged",
      !!this.updateInstall
    );

    // Hide the more options button if it's empty.
    moreOptionsButton.hidden = this.options.visibleItems.length === 0;

    // Set the private browsing badge visibility.
    if (
      !allowPrivateBrowsingByDefault &&
      addon.type == "extension" &&
      addon.incognito != "not_allowed"
    ) {
      // Keep update synchronous, the badge can appear later.
      isAllowedInPrivateBrowsing(addon).then(isAllowed => {
        card.querySelector(
          ".addon-badge-private-browsing-allowed"
        ).hidden = !isAllowed;
      });
    }
    // Update description.
    card.querySelector(".addon-description").textContent = addon.description;

    this.updateMessage();

    // Update the details if they're shown.
    if (this.details) {
      this.details.update();
    }

    this.sendEvent("update");
  }

  async updateMessage() {
    let { addon, card } = this;
    let messageBar = card.querySelector(".addon-card-message");
    let link = messageBar.querySelector("button");

    let { message, type = "", linkText, linkUrl } = await getAddonMessageInfo(
      addon
    );

    if (message) {
      messageBar.querySelector("span").textContent = message;
      messageBar.setAttribute("type", type);
      if (linkText) {
        link.textContent = linkText;
        link.setAttribute("url", linkUrl);
      }
    }

    messageBar.hidden = !message;
    link.hidden = !linkText;
  }

  showPrefs() {
    this.details.showPrefs();
  }

  expand() {
    if (this.children.length == 0) {
      this.expanded = true;
    } else {
      throw new Error("expand() is only supported before render()");
    }
  }

  render() {
    this.textContent = "";

    let { addon } = this;
    if (!addon) {
      throw new Error("addon-card must be initialized with setAddon()");
    }

    let headingId = ExtensionCommon.makeWidgetId(`${addon.name}-heading`);
    this.setAttribute("aria-labelledby", headingId);
    this.setAttribute("addon-id", addon.id);

    this.card = importTemplate("card").firstElementChild;

    let nameContainer = this.card.querySelector(".addon-name-container");
    let headingLevel = this.expanded ? "h1" : "h3";
    let nameHeading = document.createElement(headingLevel);
    nameHeading.classList.add("addon-name");
    if (!this.expanded) {
      let name = document.createElement("a");
      name.classList.add("addon-name-link");
      name.href = `addons://detail/${addon.id}`;
      nameHeading.appendChild(name);
      this.addonNameEl = name;
    } else {
      this.addonNameEl = nameHeading;
    }
    nameContainer.prepend(nameHeading);

    let panelType = addon.type == "plugin" ? "plugin-options" : "addon-options";
    this.options = document.createElement(panelType);
    this.options.render();
    this.card.querySelector(".more-options-menu").appendChild(this.options);
    this.optionsButton = this.card.querySelector(".more-options-button");

    // Set the contents.
    this.update();

    let doneRenderPromise = Promise.resolve();
    if (this.expanded) {
      if (!this.details) {
        this.details = document.createElement("addon-details");
      }
      this.details.setAddon(this.addon);
      doneRenderPromise = this.details.render();

      // If we're re-rendering we still need to append the details since the
      // entire card was emptied at the beginning of the render.
      this.card.appendChild(this.details);
    }

    this.appendChild(this.card);

    if (this.expanded && this.keyboardNavigation) {
      requestAnimationFrame(() => this.optionsButton.focus());
    }

    // Return the promise of details rendering to wait on in DetailView.
    return doneRenderPromise;
  }

  sendEvent(name, detail) {
    this.dispatchEvent(new CustomEvent(name, { detail }));
  }

  recordActionEvent(action, value) {
    AMTelemetry.recordActionEvent({
      object: "aboutAddons",
      view: getTelemetryViewName(this),
      action,
      addon: this.addon,
      value,
    });
  }
}
customElements.define("addon-card", AddonCard);

/**
 * A list view for add-ons of a certain type. It should be initialized with the
 * type of add-on to render and have section data set before being connected to
 * the document.
 *
 *    let list = document.createElement("addon-list");
 *    list.type = "plugin";
 *    list.setSections([{
 *      headingId: "plugin-section-heading",
 *      filterFn: addon => !addon.isSystem,
 *    }]);
 *    document.body.appendChild(list);
 */
class AddonList extends HTMLElement {
  constructor() {
    super();
    this.sections = [];
    this.pendingUninstallAddons = new Set();
  }

  async connectedCallback() {
    // Register the listener and get the add-ons, these operations should
    // happpen as close to each other as possible.
    this.registerListener();
    // Don't render again if we were rendered prior to being inserted.
    if (this.children.length == 0) {
      // Render the initial view.
      this.render();
    }
  }

  disconnectedCallback() {
    // Remove content and stop listening until this is connected again.
    this.textContent = "";
    this.removeListener();

    // Process any pending uninstall related to this list.
    for (const addon of this.pendingUninstallAddons) {
      if (isPending(addon, "uninstall")) {
        addon.uninstall();
      }
    }
    this.pendingUninstallAddons.clear();
  }

  /**
   * Configure the sections in the list.
   *
   * @param {object[]} sections
   *        The options for the section. Each entry in the array should have:
   *          headingId: The fluent id for the section's heading.
   *          filterFn: A function that determines if an add-on belongs in
   *                    the section.
   */
  setSections(sections) {
    this.sections = sections.map(section => Object.assign({}, section));
  }

  /**
   * Set the add-on type for this list. This will be used to filter the add-ons
   * that are displayed.
   *
   * @param {string} val The type to filter on.
   */
  set type(val) {
    this.setAttribute("type", val);
  }

  get type() {
    return this.getAttribute("type");
  }

  getSection(index) {
    return this.sections[index].node;
  }

  getCards(section) {
    return section.querySelectorAll("addon-card");
  }

  getCard(addon) {
    return this.querySelector(`addon-card[addon-id="${addon.id}"]`);
  }

  getPendingUninstallBar(addon) {
    return this.querySelector(`message-bar[addon-id="${addon.id}"]`);
  }

  sortByFn(aAddon, bAddon) {
    return aAddon.name.localeCompare(bAddon.name);
  }

  async getAddons() {
    if (!this.type) {
      throw new Error(`type must be set to find add-ons`);
    }

    // Find everything matching our type, null will find all types.
    let type = this.type == "all" ? null : [this.type];
    let addons = await AddonManager.getAddonsByTypes(type);

    // Put the add-ons into the sections, an add-on goes in the first section
    // that it matches the filterFn for. It might not go in any section.
    let sectionedAddons = this.sections.map(() => []);
    for (let addon of addons) {
      let index = this.sections.findIndex(({ filterFn }) => filterFn(addon));
      if (index != -1) {
        sectionedAddons[index].push(addon);
      } else if (isPending(addon, "uninstall")) {
        // A second tab may be opened on "about:addons" (or Firefox may
        // have crashed) while there are still "pending uninstall" add-ons.
        // Ensure to list them in the pendingUninstall message-bar-stack
        // when the AddonList is initially rendered.
        this.pendingUninstallAddons.add(addon);
      }
    }

    // Sort the add-ons in each section.
    for (let section of sectionedAddons) {
      section.sort(this.sortByFn);
    }

    return sectionedAddons;
  }

  createPendingUninstallStack() {
    const stack = document.createElement("message-bar-stack");
    stack.setAttribute("class", "pending-uninstall");
    stack.setAttribute("reverse", "");
    return stack;
  }

  addPendingUninstallBar(addon) {
    const stack = this.pendingUninstallStack;
    const mb = document.createElement("message-bar");
    mb.setAttribute("addon-id", addon.id);
    mb.setAttribute("type", "generic");

    const addonName = document.createElement("span");
    addonName.setAttribute("data-l10n-name", "addon-name");
    const message = document.createElement("span");
    message.append(addonName);
    const undo = document.createElement("button");
    undo.setAttribute("action", "undo");
    undo.addEventListener("click", () => {
      AMTelemetry.recordActionEvent({
        object: "aboutAddons",
        view: getTelemetryViewName(this),
        action: "undo",
        addon,
      });
      addon.cancelUninstall();
    });

    document.l10n.setAttributes(message, "pending-uninstall-description", {
      addon: addon.name,
    });
    document.l10n.setAttributes(undo, "pending-uninstall-undo-button");

    mb.append(message, undo);
    stack.append(mb);
  }

  removePendingUninstallBar(addon) {
    const messagebar = this.getPendingUninstallBar(addon);
    if (messagebar) {
      messagebar.remove();
    }
  }

  createSectionHeading(headingIndex) {
    let { headingId } = this.sections[headingIndex];
    let heading = document.createElement("h2");
    heading.classList.add("list-section-heading");
    document.l10n.setAttributes(heading, headingId);
    return heading;
  }

  updateSectionIfEmpty(section) {
    // The header is added before any add-on cards, so if there's only one
    // child then it's the header. In that case we should empty out the section.
    if (section.children.length == 1) {
      section.textContent = "";
    }
  }

  insertCardInto(card, sectionIndex) {
    let section = this.getSection(sectionIndex);
    let sectionCards = this.getCards(section);

    // If this is the first card in the section, create the heading.
    if (sectionCards.length == 0) {
      section.appendChild(this.createSectionHeading(sectionIndex));
    }

    // Find where to insert the card.
    let insertBefore = Array.from(sectionCards).find(
      otherCard => this.sortByFn(card.addon, otherCard.addon) < 0
    );
    // This will append if insertBefore is null.
    section.insertBefore(card, insertBefore || null);
  }

  addAddon(addon) {
    // Only insert add-ons of the right type.
    if (addon.type != this.type && this.type != "all") {
      this.sendEvent("skip-add", "type-mismatch");
      return;
    }

    let insertSection = this.sections.findIndex(({ filterFn }) =>
      filterFn(addon)
    );

    // Don't add the add-on if it doesn't go in a section.
    if (insertSection == -1) {
      return;
    }

    // Create and insert the card.
    let card = document.createElement("addon-card");
    card.setAddon(addon);
    this.insertCardInto(card, insertSection);
    this.sendEvent("add", { id: addon.id });
  }

  sendEvent(name, detail) {
    this.dispatchEvent(new CustomEvent(name, { detail }));
  }

  removeAddon(addon) {
    let card = this.getCard(addon);
    if (card) {
      let section = card.parentNode;
      card.remove();
      this.updateSectionIfEmpty(section);
      this.sendEvent("remove", { id: addon.id });
    }
  }

  updateAddon(addon) {
    let card = this.getCard(addon);
    if (card) {
      let sectionIndex = this.sections.findIndex(s => s.filterFn(addon));
      if (sectionIndex != -1) {
        // Move the card, if needed. This will allow an animation between
        // page sections and provides clearer events for testing.
        if (card.parentNode.getAttribute("section") != sectionIndex) {
          let oldSection = card.parentNode;
          this.insertCardInto(card, sectionIndex);
          this.updateSectionIfEmpty(oldSection);
          this.sendEvent("move", { id: addon.id });
        }
      } else {
        this.removeAddon(addon);
      }
    } else {
      // Add the add-on, this will do nothing if it shouldn't be in the list.
      this.addAddon(addon);
    }
  }

  renderSection(addons, index) {
    let section = document.createElement("section");
    section.setAttribute("section", index);

    // Render the heading and add-ons if there are any.
    if (addons.length > 0) {
      section.appendChild(this.createSectionHeading(index));

      for (let addon of addons) {
        let card = document.createElement("addon-card");
        card.setAddon(addon);
        card.render();
        section.appendChild(card);
      }
    }

    return section;
  }

  async render() {
    this.textContent = "";

    let sectionedAddons = await this.getAddons();

    let frag = document.createDocumentFragment();

    // Render the pending uninstall message-bar-stack.
    this.pendingUninstallStack = this.createPendingUninstallStack();
    for (let addon of this.pendingUninstallAddons) {
      this.addPendingUninstallBar(addon);
    }
    frag.appendChild(this.pendingUninstallStack);

    // Render the sections.
    for (let i = 0; i < sectionedAddons.length; i++) {
      this.sections[i].node = this.renderSection(sectionedAddons[i], i);
      frag.appendChild(this.sections[i].node);
    }

    // Make sure fluent has set all the strings before we render. This will
    // avoid the height changing as strings go from 0 height to having text.
    await document.l10n.translateFragment(frag);
    this.appendChild(frag);
  }

  registerListener() {
    AddonManager.addAddonListener(this);
  }

  removeListener() {
    AddonManager.removeAddonListener(this);
  }

  onOperationCancelled(addon) {
    if (
      this.pendingUninstallAddons.has(addon) &&
      !isPending(addon, "uninstall")
    ) {
      this.pendingUninstallAddons.delete(addon);
      this.removePendingUninstallBar(addon);
    }
    this.updateAddon(addon);
  }

  onEnabled(addon) {
    this.updateAddon(addon);
  }

  onDisabled(addon) {
    this.updateAddon(addon);
  }

  onUninstalling(addon) {
    if (
      isPending(addon, "uninstall") &&
      (this.type === "all" || addon.type === this.type)
    ) {
      this.pendingUninstallAddons.add(addon);
      this.addPendingUninstallBar(addon);
      this.updateAddon(addon);
    }
  }

  onInstalled(addon) {
    if (this.querySelector(`addon-card[addon-id="${addon.id}"]`)) {
      return;
    }
    this.addAddon(addon);
  }

  onUninstalled(addon) {
    this.pendingUninstallAddons.delete(addon);
    this.removePendingUninstallBar(addon);
    this.removeAddon(addon);
  }
}
customElements.define("addon-list", AddonList);

class ListView {
  constructor({ param, root }) {
    this.type = param;
    this.root = root;
  }

  async render() {
    let frag = document.createDocumentFragment();

    let list = document.createElement("addon-list");
    list.type = this.type;
    list.setSections([
      {
        headingId: "addons-enabled-heading",
        filterFn: addon =>
          !addon.hidden && addon.isActive && !isPending(addon, "uninstall"),
      },
      {
        headingId: "addons-disabled-heading",
        filterFn: addon =>
          !addon.hidden && !addon.isActive && !isPending(addon, "uninstall"),
      },
    ]);
    frag.appendChild(list);

    await list.render();

    this.root.textContent = "";
    this.root.appendChild(frag);
  }
}

class DetailView {
  constructor({ isKeyboardNavigation, param, root }) {
    let [id, selectedTab] = param.split("/");
    this.id = id;
    this.selectedTab = selectedTab;
    this.root = root;
    this.isKeyboardNavigation = isKeyboardNavigation;
  }

  async render() {
    let addon = await AddonManager.getAddonByID(this.id);

    if (!addon) {
      replaceWithDefaultViewFn();
      return;
    }

    let card = document.createElement("addon-card");

    // Ensure the category for this add-on type is selected.
    setCategoryFn(addon.type);

    // Go back to the list view when the add-on is removed.
    card.addEventListener("remove", () => loadViewFn(`list/${addon.type}`));

    card.setAddon(addon);
    card.expand();
    card.keyboardNavigation = this.isKeyboardNavigation;
    await card.render();
    if (
      this.selectedTab === "preferences" &&
      (await isAddonOptionsUIAllowed(addon))
    ) {
      card.showPrefs();
    }

    this.root.textContent = "";
    this.root.appendChild(card);
  }
}

class UpdatesView {
  constructor({ param, root }) {
    this.root = root;
    this.param = param;
  }

  async render() {
    let list = document.createElement("addon-list");
    list.type = "all";
    if (this.param == "available") {
      list.setSections([
        {
          headingId: "available-updates-heading",
          filterFn: addon => addon.updateInstall,
        },
      ]);
    } else if (this.param == "recent") {
      list.sortByFn = (a, b) => {
        if (a.updateDate > b.updateDate) {
          return -1;
        }
        if (a.updateDate < b.updateDate) {
          return 1;
        }
        return 0;
      };
      let updateLimit = new Date() - UPDATES_RECENT_TIMESPAN;
      list.setSections([
        {
          headingId: "recent-updates-heading",
          filterFn: addon =>
            !addon.hidden && addon.updateDate && addon.updateDate > updateLimit,
        },
      ]);
    } else {
      throw new Error(`Unknown updates view ${this.param}`);
    }

    await list.render();
    this.root.textContent = "";
    this.root.appendChild(list);
  }
}

// Generic view management.
let mainEl = null;

/**
 * The name of the view for an element, used for telemetry.
 *
 * @param {Element} el The element to find the view from. A parent of the
 *                     element must define a current-view property.
 * @returns {string} The current view name.
 */
function getTelemetryViewName(el) {
  return el.closest("[current-view]").getAttribute("current-view");
}

/**
 * Called from extensions.js once, when about:addons is loading.
 */
function initialize(opts) {
  mainEl = document.getElementById("main");
  loadViewFn = opts.loadViewFn;
  replaceWithDefaultViewFn = opts.replaceWithDefaultViewFn;
  setCategoryFn = opts.setCategoryFn;
  AddonCardListenerHandler.startup();
  window.addEventListener(
    "unload",
    () => {
      // Clear out the main node so the disconnectedCallback will trigger
      // properly and all of the custom elements can cleanup.
      mainEl.textContent = "";
      AddonCardListenerHandler.shutdown();
    },
    { once: true }
  );
}

/**
 * Called from extensions.js to load a view. The view's render method should
 * resolve once the view has been updated to conform with other about:addons
 * views.
 */
async function show(type, param, { isKeyboardNavigation }) {
  let container = document.createElement("div");
  container.setAttribute("current-view", type);
  if (type == "list") {
    await new ListView({ param, root: container }).render();
  } else if (type == "detail") {
    await new DetailView({
      isKeyboardNavigation,
      param,
      root: container,
    }).render();
  } else if (type == "updates") {
    await new UpdatesView({ param, root: container }).render();
  } else {
    throw new Error(`Unknown view type: ${type}`);
  }
  mainEl.textContent = "";
  mainEl.appendChild(container);
}

function hide() {
  mainEl.textContent = "";
}
