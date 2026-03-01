/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var ThirdPartyCookies = {
  PREF_ENABLED: "network.cookie.cookieBehavior",
  PREF_ENABLED_VALUES: [
    // These values match the ones exposed under the Content Blocking section
    // of the Preferences UI.
    Ci.nsICookieService.BEHAVIOR_ACCEPT,
    Ci.nsICookieService.BEHAVIOR_LIMIT_FOREIGN,
  ],

  get categoryItem() {
    delete this.categoryItem;
    return (this.categoryItem = document.getElementById(
      "identity-popup-content-blocking-category-cookies"
    ));
  },

  get categoryLabel() {
    delete this.categoryLabel;
    return (this.categoryLabel = document.getElementById(
      "identity-popup-content-blocking-cookies-state-label"
    ));
  },

  get subViewList() {
    delete this.subViewList;
    return (this.subViewList = document.getElementById(
      "identity-popup-cookiesView-list"
    ));
  },

  strings: {
    get subViewAllowed() {
      delete this.subViewAllowed;
      return (this.subViewAllowed = gNavigatorBundle.getString(
        "contentBlocking.cookiesView.allowed.label"
      ));
    },

    get subViewBlocked() {
      delete this.subViewBlocked;
      return (this.subViewBlocked = gNavigatorBundle.getString(
        "contentBlocking.cookiesView.blocked.label"
      ));
    },
  },

  updateCategoryLabel() {
    let label;
    switch (this.behaviorPref) {
      case Ci.nsICookieService.BEHAVIOR_ACCEPT:
        label = ContentBlocking.showAllowedLabels
          ? "contentBlocking.cookies.allowed.label"
          : null;
        break;
      case Ci.nsICookieService.BEHAVIOR_REJECT_FOREIGN:
        label = ContentBlocking.showBlockedLabels
          ? "contentBlocking.cookies.blocking3rdParty.label"
          : null;
        break;
      case Ci.nsICookieService.BEHAVIOR_REJECT:
        label = ContentBlocking.showBlockedLabels
          ? "contentBlocking.cookies.blockingAll.label"
          : null;
        break;
      case Ci.nsICookieService.BEHAVIOR_LIMIT_FOREIGN:
        label = ContentBlocking.showBlockedLabels
          ? "contentBlocking.cookies.blockingUnvisited.label"
          : null;
        break;
      default:
        Cu.reportError(
          `Error: Unknown cookieBehavior pref observed: ${this.behaviorPref}`
        );
    }
    this.categoryLabel.textContent = label
      ? gNavigatorBundle.getString(label)
      : "";
  },

  init() {
    XPCOMUtils.defineLazyPreferenceGetter(
      this,
      "behaviorPref",
      this.PREF_ENABLED,
      Ci.nsICookieService.BEHAVIOR_ACCEPT,
      this.updateCategoryLabel.bind(this)
    );
    this.updateCategoryLabel();
  },

  get enabled() {
    return this.PREF_ENABLED_VALUES.includes(this.behaviorPref);
  },

  isBlocking(state) {
    return (
      (state & Ci.nsIWebProgressListener.STATE_COOKIES_BLOCKED_ALL) != 0 ||
      (state & Ci.nsIWebProgressListener.STATE_COOKIES_BLOCKED_BY_PERMISSION) !=
        0 ||
      (state & Ci.nsIWebProgressListener.STATE_COOKIES_BLOCKED_FOREIGN) != 0
    );
  },

  isDetected(state) {
    return (state & Ci.nsIWebProgressListener.STATE_COOKIES_LOADED) != 0;
  },

  async updateSubView() {
    let contentBlockingLog = await gBrowser.selectedBrowser.getContentBlockingLog();
    contentBlockingLog = JSON.parse(contentBlockingLog);

    let categories = this._processContentBlockingLog(contentBlockingLog);

    this.subViewList.textContent = "";

    for (let category of ["firstParty", "thirdParty"]) {
      let box = document.createXULElement("vbox");
      let label = document.createXULElement("label");
      label.className = "identity-popup-cookiesView-list-header";
      label.textContent = gNavigatorBundle.getString(
        `contentBlocking.cookiesView.${category}.label`
      );
      box.appendChild(label);

      for (let info of categories[category]) {
        box.appendChild(this._createListItem(info));
      }

      // If the category is empty, add a label noting that to the user.
      if (categories[category].length == 0) {
        let emptyLabel = document.createXULElement("label");
        emptyLabel.classList.add("identity-popup-content-blocking-empty-label");
        emptyLabel.textContent = gNavigatorBundle.getString(
          `contentBlocking.cookiesView.${category}.empty.label`
        );
        box.appendChild(emptyLabel);
      }

      this.subViewList.appendChild(box);
    }
  },

  _hasException(origin) {
    for (let perm of Services.perms.getAllForPrincipal(
      gBrowser.contentPrincipal
    )) {
      if (
        perm.type == "3rdPartyStorage^" + origin ||
        perm.type.startsWith("3rdPartyStorage^" + origin + "^")
      ) {
        return true;
      }
    }

    let principal = Services.scriptSecurityManager.createCodebasePrincipalFromOrigin(
      origin
    );
    // Cookie exceptions get "inherited" from parent- to sub-domain, so we need to
    // make sure to include parent domains in the permission check for "cookies".
    return (
      Services.perms.testPermissionFromPrincipal(principal, "cookie") !=
      Services.perms.UNKNOWN_ACTION
    );
  },

  _clearException(origin) {
    for (let perm of Services.perms.getAllForPrincipal(
      gBrowser.contentPrincipal
    )) {
      if (
        perm.type == "3rdPartyStorage^" + origin ||
        perm.type.startsWith("3rdPartyStorage^" + origin + "^")
      ) {
        Services.perms.removePermission(perm);
      }
    }

    // OAs don't matter here, so we can just use the hostname.
    let host = Services.io.newURI(origin).host;

    // Cookie exceptions get "inherited" from parent- to sub-domain, so we need to
    // clear any cookie permissions from parent domains as well.
    for (let perm of Services.perms.enumerator) {
      if (
        perm.type == "cookie" &&
        Services.eTLD.hasRootDomain(host, perm.principal.host)
      ) {
        Services.perms.removePermission(perm);
      }
    }
  },

  // Transforms and filters cookie entries in the content blocking log
  // so that we can categorize and display them in the UI.
  _processContentBlockingLog(log) {
    let newLog = {
      firstParty: [],
      thirdParty: [],
    };

    let firstPartyDomain = null;
    try {
      firstPartyDomain = Services.eTLD.getBaseDomain(gBrowser.currentURI);
    } catch (e) {
      // There are nasty edge cases here where someone is trying to set a cookie
      // on a public suffix or an IP address. Just categorize those as third party...
      if (
        e.result != Cr.NS_ERROR_HOST_IS_IP_ADDRESS &&
        e.result != Cr.NS_ERROR_INSUFFICIENT_DOMAIN_LEVELS
      ) {
        throw e;
      }
    }

    for (let [origin, actions] of Object.entries(log)) {
      if (!origin.startsWith("http")) {
        continue;
      }

      let info = {
        origin,
        isAllowed: true,
        hasException: this._hasException(origin),
      };
      let hasCookie = false;

      // Extract information from the states entries in the content blocking log.
      // Each state will contain a single state flag from nsIWebProgressListener.
      // Note that we are using the same helper functions that are applied to the
      // bit map passed to onSecurityChange (which contains multiple states), thus
      // not checking exact equality, just presence of bits.
      for (let [state, blocked] of actions) {
        if (this.isDetected(state)) {
          hasCookie = true;
        }
        // blocked tells us whether the resource was actually blocked
        // (which it may not be in case of an exception).
        if (this.isBlocking(state)) {
          info.isAllowed = !blocked;
        }
      }

      if (!hasCookie) {
        continue;
      }

      let isFirstParty = false;
      try {
        let uri = Services.io.newURI(origin);
        isFirstParty = Services.eTLD.getBaseDomain(uri) == firstPartyDomain;
      } catch (e) {
        if (
          e.result != Cr.NS_ERROR_HOST_IS_IP_ADDRESS &&
          e.result != Cr.NS_ERROR_INSUFFICIENT_DOMAIN_LEVELS
        ) {
          throw e;
        }
      }

      if (isFirstParty) {
        newLog.firstParty.push(info);
      } else {
        newLog.thirdParty.push(info);
      }
    }

    return newLog;
  },

  _createListItem({ origin, isAllowed, hasException }) {
    let listItem = document.createXULElement("hbox");
    listItem.className = "identity-popup-content-blocking-list-item";
    listItem.classList.toggle("allowed", isAllowed);
    // Repeat the origin in the tooltip in case it's too long
    // and overflows in our panel.
    listItem.tooltipText = origin;

    let image = document.createXULElement("image");
    image.className = "identity-popup-cookiesView-icon";
    image.classList.toggle("allowed", isAllowed);
    listItem.append(image);

    let label = document.createXULElement("label");
    label.value = origin;
    label.className = "identity-popup-content-blocking-list-host-label";
    label.setAttribute("crop", "end");
    listItem.append(label);

    let stateLabel;
    if (isAllowed && hasException) {
      stateLabel = document.createXULElement("label");
      stateLabel.value = this.strings.subViewAllowed;
      stateLabel.className = "identity-popup-content-blocking-list-state-label";
      listItem.append(stateLabel);
    } else if (!isAllowed) {
      stateLabel = document.createXULElement("label");
      stateLabel.value = this.strings.subViewBlocked;
      stateLabel.className = "identity-popup-content-blocking-list-state-label";
      listItem.append(stateLabel);
    }

    if (hasException) {
      let removeException = document.createXULElement("button");
      removeException.className = "identity-popup-permission-remove-button";
      removeException.tooltipText = gNavigatorBundle.getFormattedString(
        "contentBlocking.cookiesView.removeButton.tooltip",
        [origin]
      );
      removeException.addEventListener("click", () => {
        this._clearException(origin);
        // Just flip the display based on what state we had previously.
        stateLabel.value = isAllowed
          ? this.strings.subViewBlocked
          : this.strings.subViewAllowed;
        listItem.classList.toggle("allowed", !isAllowed);
        image.classList.toggle("allowed", !isAllowed);
        removeException.hidden = true;
      });
      listItem.append(removeException);
    }

    return listItem;
  },
};

var ContentBlocking = {
  PREF_ANIMATIONS_ENABLED: "toolkit.cosmeticAnimations.enabled",
  PREF_CB_CATEGORY: "browser.contentblocking.category",
  PREF_SHOW_ALLOWED_LABELS:
    "browser.contentblocking.control-center.ui.showAllowedLabels",
  PREF_SHOW_BLOCKED_LABELS:
    "browser.contentblocking.control-center.ui.showBlockedLabels",

  get content() {
    delete this.content;
    return (this.content = document.getElementById(
      "identity-popup-content-blocking-content"
    ));
  },

  get icon() {
    delete this.icon;
    return (this.icon = document.getElementById("tracking-protection-icon"));
  },

  get iconBox() {
    delete this.iconBox;
    return (this.iconBox = document.getElementById(
      "tracking-protection-icon-box"
    ));
  },

  get animatedIcon() {
    delete this.animatedIcon;
    return (this.animatedIcon = document.getElementById(
      "tracking-protection-icon-animatable-image"
    ));
  },

  get identityPopupMultiView() {
    delete this.identityPopupMultiView;
    return (this.identityPopupMultiView = document.getElementById(
      "identity-popup-multiView"
    ));
  },

  get identityPopup() {
    delete this.identityPopup;
    return (this.identityPopup = document.getElementById("identity-popup"));
  },

  strings: {
    get activeTooltipText() {
      delete this.activeTooltipText;
      return (this.activeTooltipText = gNavigatorBundle.getString(
        "trackingProtection.icon.activeTooltip"
      ));
    },

    get disabledTooltipText() {
      delete this.disabledTooltipText;
      return (this.disabledTooltipText = gNavigatorBundle.getString(
        "trackingProtection.icon.disabledTooltip"
      ));
    },
  },

  // A list of blockers that will be displayed in the categories list
  // when blockable content is detected. A blocker must be an object
  // with at least the following two properties:
  //  - enabled: Whether the blocker is currently turned on.
  //  - isDetected(state): Given a content blocking state, whether the blocker has
  //                       either allowed or blocked elements.
  //  - categoryItem: The DOM item that represents the entry in the category list.
  //
  // It may also contain an init() and uninit() function, which will be called
  // on ContentBlocking.init() and ContentBlocking.uninit().
  blockers: [
    ThirdPartyCookies,
  ],

  get _baseURIForChannelClassifier() {
    // Convert document URI into the format used by
    // nsChannelClassifier::ShouldEnableTrackingProtection.
    // Any scheme turned into https is correct.
    try {
      return Services.io.newURI(
        "https://" + gBrowser.selectedBrowser.currentURI.hostPort
      );
    } catch (e) {
      // Getting the hostPort for about: and file: URIs fails, but TP doesn't work with
      // these URIs anyway, so just return null here.
      return null;
    }
  },

  init() {
    this.animatedIcon.addEventListener("animationend", () =>
      this.iconBox.removeAttribute("animate")
    );

    this.updateAnimationsEnabled = () => {
      this.iconBox.toggleAttribute(
        "animationsenabled",
        Services.prefs.getBoolPref(this.PREF_ANIMATIONS_ENABLED, false)
      );
    };

    XPCOMUtils.defineLazyPreferenceGetter(
      this,
      "showBlockedLabels",
      this.PREF_SHOW_BLOCKED_LABELS,
      false,
      () => {
        for (let blocker of this.blockers) {
          blocker.updateCategoryLabel();
        }
      }
    );
    XPCOMUtils.defineLazyPreferenceGetter(
      this,
      "showAllowedLabels",
      this.PREF_SHOW_ALLOWED_LABELS,
      false,
      () => {
        for (let blocker of this.blockers) {
          blocker.updateCategoryLabel();
        }
      }
    );

    for (let blocker of this.blockers) {
      if (blocker.init) {
        blocker.init();
      }
    }

    this.updateAnimationsEnabled();

    Services.prefs.addObserver(
      this.PREF_ANIMATIONS_ENABLED,
      this.updateAnimationsEnabled
    );
  },

  uninit() {
    for (let blocker of this.blockers) {
      if (blocker.uninit) {
        blocker.uninit();
      }
    }

    Services.prefs.removeObserver(
      this.PREF_ANIMATIONS_ENABLED,
      this.updateAnimationsEnabled
    );
  },

  hideIdentityPopupAndReload() {
    this.identityPopup.hidePopup();
    BrowserReload();
  },

  openPreferences(origin) {
    openPreferences("privacy-trackingprotection", { origin });
  },

  backToMainView() {
    this.identityPopupMultiView.goBack();
  },

  async showCookiesSubview() {
    await ThirdPartyCookies.updateSubView();
    this.identityPopupMultiView.showSubView("identity-popup-cookiesView");
  },

  onContentBlockingEvent(event, webProgress, isSimulated) {
    let previousState = gBrowser.securityUI.contentBlockingEvent;
    let baseURI = this._baseURIForChannelClassifier;

    // Don't deal with about:, file: etc.
    if (!baseURI) {
      this.iconBox.removeAttribute("animate");
      this.iconBox.removeAttribute("active");
      this.iconBox.removeAttribute("hasException");
      return;
    }

    let anyDetected = false;
    let anyBlocking = false;

    for (let blocker of this.blockers) {
      // Store data on whether the blocker is activated in the current document for
      // reporting it using the "report breakage" dialog. Under normal circumstances this
      // dialog should only be able to open in the currently selected tab and onSecurityChange
      // runs on tab switch, so we can avoid associating the data with the document directly.
      blocker.activated = blocker.isBlocking(event);
      blocker.categoryItem.classList.toggle("blocked", blocker.enabled);
      let detected = blocker.isDetected(event);
      blocker.categoryItem.hidden = !detected;
      anyDetected = anyDetected || detected;
      anyBlocking = anyBlocking || blocker.activated;
    }

    let isBrowserPrivate = PrivateBrowsingUtils.isBrowserPrivate(
      gBrowser.selectedBrowser
    );

    // Check whether the user has added an exception for this site.
    let principal = Services.scriptSecurityManager.createCodebasePrincipal(
      baseURI,
      {}
    );
    let type = isBrowserPrivate
      ? "trackingprotection-pb"
      : "trackingprotection";
    let hasException =
      Services.perms.testExactPermissionFromPrincipal(principal, type) ==
      Services.perms.ALLOW_ACTION;

    // Reset the animation in case the user is switching tabs or if no blockers were detected
    // (this is most likely happening because the user navigated on to a different site). This
    // allows us to play it from the start without choppiness next time.
    if (isSimulated || !anyBlocking) {
      this.iconBox.removeAttribute("animate");
      // Only play the animation when the shield is not already shown on the page (the visibility
      // of the shield based on this onSecurityChange be determined afterwards).
    } else if (anyBlocking && !this.iconBox.hasAttribute("active")) {
      this.iconBox.setAttribute("animate", "true");
    }

    // We consider the shield state "active" when some kind of blocking activity
    // occurs on the page.  Note that merely allowing the loading of content that
    // we could have blocked does not trigger the appearance of the shield.
    // This state will be overriden later if there's an exception set for this site.
    this.content.toggleAttribute("detected", anyDetected);
    this.content.toggleAttribute("blocking", anyBlocking);
    this.content.toggleAttribute("hasException", hasException);

    this.iconBox.toggleAttribute("active", anyBlocking);
    this.iconBox.toggleAttribute("hasException", hasException);

    if (hasException) {
      this.iconBox.setAttribute(
        "tooltiptext",
        this.strings.disabledTooltipText
      );
      if (!this.hadShieldState && !isSimulated) {
        this.hadShieldState = true;
      }
    } else if (anyBlocking) {
      this.iconBox.setAttribute("tooltiptext", this.strings.activeTooltipText);
      if (!this.hadShieldState && !isSimulated) {
        this.hadShieldState = true;
      }
    } else {
      this.iconBox.removeAttribute("tooltiptext");
    }
  },

  disableForCurrentPage() {
    let baseURI = this._baseURIForChannelClassifier;

    // Add the current host in the 'trackingprotection' consumer of
    // the permission manager using a normalized URI. This effectively
    // places this host on the tracking protection allowlist.
    if (PrivateBrowsingUtils.isBrowserPrivate(gBrowser.selectedBrowser)) {
      PrivateBrowsingUtils.addToTrackingAllowlist(baseURI);
    } else {
      Services.perms.add(
        baseURI,
        "trackingprotection",
        Services.perms.ALLOW_ACTION
      );
    }

    this.hideIdentityPopupAndReload();
  },

  enableForCurrentPage() {
    // Remove the current host from the 'trackingprotection' consumer
    // of the permission manager. This effectively removes this host
    // from the tracking protection allowlist.
    let baseURI = this._baseURIForChannelClassifier;

    if (PrivateBrowsingUtils.isBrowserPrivate(gBrowser.selectedBrowser)) {
      PrivateBrowsingUtils.removeFromTrackingAllowlist(baseURI);
    } else {
      Services.perms.remove(baseURI, "trackingprotection");
    }

    this.hideIdentityPopupAndReload();
  },
};
