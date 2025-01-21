/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
"use strict";

const { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");

ChromeUtils.defineModuleGetter(
  this,
  "AppConstants",
  "resource://gre/modules/AppConstants.jsm"
);
ChromeUtils.defineModuleGetter(
  this,
  "UpdateUtils",
  "resource://gre/modules/UpdateUtils.jsm"
);

// NB: Eagerly load modules that will be loaded/constructed/initialized in the
// common case to avoid the overhead of wrapping and detecting lazy loading.
const { actionCreators: ac, actionTypes: at } = ChromeUtils.import(
  "resource://activity-stream/common/Actions.jsm"
);
const { AboutPreferences } = ChromeUtils.import(
  "resource://activity-stream/lib/AboutPreferences.jsm"
);
const { DefaultPrefs } = ChromeUtils.import(
  "resource://activity-stream/lib/ActivityStreamPrefs.jsm"
);
const { NewTabInit } = ChromeUtils.import(
  "resource://activity-stream/lib/NewTabInit.jsm"
);
const { SectionsFeed } = ChromeUtils.import(
  "resource://activity-stream/lib/SectionsManager.jsm"
);
const { PlacesFeed } = ChromeUtils.import(
  "resource://activity-stream/lib/PlacesFeed.jsm"
);
const { PrefsFeed } = ChromeUtils.import(
  "resource://activity-stream/lib/PrefsFeed.jsm"
);
const { Store } = ChromeUtils.import(
  "resource://activity-stream/lib/Store.jsm"
);
const { SystemTickFeed } = ChromeUtils.import(
  "resource://activity-stream/lib/SystemTickFeed.jsm"
);
const { FaviconFeed } = ChromeUtils.import(
  "resource://activity-stream/lib/FaviconFeed.jsm"
);
const { TopSitesFeed } = ChromeUtils.import(
  "resource://activity-stream/lib/TopSitesFeed.jsm"
);
const { HighlightsFeed } = ChromeUtils.import(
  "resource://activity-stream/lib/HighlightsFeed.jsm"
);

const DEFAULT_SITES = new Map([
  // This first item is the global list fallback for any unexpected geos
  [
    "",
    "https://www.wikipedia.org/,https://www.reddit.com/",
  ],
  [
    "US",
    "https://www.reddit.com/,https://www.wikipedia.org/",
  ],
  [
    "CA",
    "https://www.reddit.com/,https://www.wikipedia.org/",
  ],
  [
    "DE",
    "https://www.wikipedia.org/,https://www.reddit.com/",
  ],
  [
    "PL",
    "https://allegro.pl/,https://www.wikipedia.org/,https://www.olx.pl/,https://www.wykop.pl/",
  ],
  [
    "RU",
    "https://vk.com/,https://ok.ru/,https://www.avito.ru/,https://www.aliexpress.com/,https://www.wikipedia.org/",
  ],
  [
    "FR",
    "https://www.wikipedia.org/,https://www.leboncoin.fr/",
  ],
]);
const GEO_PREF = "browser.search.region";
const IS_NIGHTLY_OR_UNBRANDED_BUILD = ["nightly", "default"].includes(
  UpdateUtils.getUpdateChannel(true)
);

// Determine if spocs should be shown for a geo/locale
function showSpocs({ geo }) {
  return SPOCS_GEOS.includes(geo);
}

// Configure default Activity Stream prefs with a plain `value` or a `getValue`
// that computes a value. A `value_local_dev` is used for development defaults.
const PREFS_CONFIG = new Map([
  [
    "default.sites",
    {
      title:
        "Comma-separated list of default top sites to fill in behind visited sites",
      getValue: ({ geo }) =>
        DEFAULT_SITES.get(DEFAULT_SITES.has(geo) ? geo : ""),
    },
  ],
  [
    "filterAdult",
    {
      title: "Remove adult pages from sites, highlights, etc.",
      value: true,
    },
  ],
  [
    "showSearch",
    {
      title: "Show the Search bar",
      value: true,
    },
  ],
  [
    "topSitesRows",
    {
      title: "Number of rows of Top Sites to display",
      value: 1,
    },
  ],
  [
    "section.highlights.includeVisited",
    {
      title:
        "Boolean flag that decides whether or not to show visited pages in highlights.",
      value: true,
    },
  ],
  [
    "section.highlights.includeBookmarks",
    {
      title:
        "Boolean flag that decides whether or not to show bookmarks in highlights.",
      value: true,
    },
  ],
  [
    "section.highlights.includeDownloads",
    {
      title:
        "Boolean flag that decides whether or not to show saved recent Downloads in highlights.",
      value: true,
    },
  ],
  [
    "section.highlights.rows",
    {
      title: "Number of rows of Highlights to display",
      value: 1,
    },
  ],
  [
    "sectionOrder",
    {
      title: "The rendering order for the sections",
      value: "topsites,highlights",
    },
  ],
  [
    "improvesearch.noDefaultSearchTile",
    {
      title: "Remove tiles that are the same as the default search",
      value: true,
    },
  ],
  [
    "improvesearch.topSiteSearchShortcuts.searchEngines",
    {
      title:
        "An ordered, comma-delimited list of search shortcuts that we should try and pin",
      // This pref is dynamic as the shortcuts vary depending on the region
      getValue: ({ geo }) => {
        if (!geo) {
          return "";
        }
        const searchShortcuts = [];
        if (geo === "CN") {
          searchShortcuts.push("baidu");
        } else if (["BY", "KZ", "RU", "TR"].includes(geo)) {
          searchShortcuts.push("yandex");
        } else {
          searchShortcuts.push("google");
        }
        if (["DE", "FR", "GB", "IT", "JP", "US"].includes(geo)) {
          searchShortcuts.push("amazon");
        }
        return searchShortcuts.join(",");
      },
    },
  ],
  [
    "improvesearch.topSiteSearchShortcuts.havePinned",
    {
      title:
        "A comma-delimited list of search shortcuts that have previously been pinned",
      value: "",
    },
  ],
]);

// Array of each feed's FEEDS_CONFIG factory and values to add to PREFS_CONFIG
const FEEDS_DATA = [
  {
    name: "aboutpreferences",
    factory: () => new AboutPreferences(),
    title: "about:preferences rendering",
    value: true,
  },
  {
    name: "newtabinit",
    factory: () => new NewTabInit(),
    title: "Sends a copy of the state to each new tab that is opened",
    value: true,
  },
  {
    name: "places",
    factory: () => new PlacesFeed(),
    title: "Listens for and relays various Places-related events",
    value: true,
  },
  {
    name: "prefs",
    factory: () => new PrefsFeed(PREFS_CONFIG),
    title: "Preferences",
    value: true,
  },
  {
    name: "sections",
    factory: () => new SectionsFeed(),
    title: "Manages sections",
    value: true,
  },
  {
    name: "section.highlights",
    factory: () => new HighlightsFeed(),
    title: "Fetches content recommendations from places db",
    value: true,
  },
  {
    name: "systemtick",
    factory: () => new SystemTickFeed(),
    title: "Produces system tick events to periodically check for data expiry",
    value: true,
  },
  {
    name: "favicon",
    factory: () => new FaviconFeed(),
    title: "Fetches tippy top manifests from remote service",
    value: true,
  },
  {
    name: "topsites",
    factory: () => new TopSitesFeed(),
    title: "Queries places and gets metadata for Top Sites section",
    value: true,
  },
];

const FEEDS_CONFIG = new Map();
for (const config of FEEDS_DATA) {
  const pref = `feeds.${config.name}`;
  FEEDS_CONFIG.set(pref, config.factory);
  PREFS_CONFIG.set(pref, config);
}

this.ActivityStream = class ActivityStream {
  /**
   * constructor - Initializes an instance of ActivityStream
   */
  constructor() {
    this.initialized = false;
    this.store = new Store();
    this.feeds = FEEDS_CONFIG;
    this._defaultPrefs = new DefaultPrefs(PREFS_CONFIG);
  }

  init() {
    try {
      this._updateDynamicPrefs();
      this._defaultPrefs.init();

      // Hook up the store and let all feeds and pages initialize
      this.store.init(
        this.feeds,
        ac.BroadcastToContent({
          type: at.INIT,
          data: {},
        }),
        { type: at.UNINIT }
      );

      this.initialized = true;
    } catch (e) {
      throw e;
    }
  }

  /**
   * Check if an old pref has a custom value to migrate. Clears the pref so that
   * it's the default after migrating (to avoid future need to migrate).
   *
   * @param oldPrefName {string} Pref to check and migrate
   * @param cbIfNotDefault {function} Callback that gets the current pref value
   */
  _migratePref(oldPrefName, cbIfNotDefault) {
    // Nothing to do if the user doesn't have a custom value
    if (!Services.prefs.prefHasUserValue(oldPrefName)) {
      return;
    }

    // Figure out what kind of pref getter to use
    let prefGetter;
    switch (Services.prefs.getPrefType(oldPrefName)) {
      case Services.prefs.PREF_BOOL:
        prefGetter = "getBoolPref";
        break;
      case Services.prefs.PREF_INT:
        prefGetter = "getIntPref";
        break;
      case Services.prefs.PREF_STRING:
        prefGetter = "getStringPref";
        break;
    }

    // Give the callback the current value then clear the pref
    cbIfNotDefault(Services.prefs[prefGetter](oldPrefName));
    Services.prefs.clearUserPref(oldPrefName);
  }

  uninit() {
    if (this.geo === "") {
      Services.prefs.removeObserver(GEO_PREF, this);
    }

    this.store.uninit();
    this.initialized = false;
  }

  _updateDynamicPrefs() {
    // Save the geo pref if we have it
    if (Services.prefs.prefHasUserValue(GEO_PREF)) {
      this.geo = Services.prefs.getStringPref(GEO_PREF);
    } else if (this.geo !== "") {
      // Watch for geo changes and use a dummy value for now
      Services.prefs.addObserver(GEO_PREF, this);
      this.geo = "";
    }

    this.locale = Services.locale.appLocaleAsBCP47;

    // Update the pref config of those with dynamic values
    for (const pref of PREFS_CONFIG.keys()) {
      // Only need to process dynamic prefs
      const prefConfig = PREFS_CONFIG.get(pref);
      if (!prefConfig.getValue) {
        continue;
      }

      // Have the dynamic pref just reuse using existing default, e.g., those
      // set via Autoconfig or policy
      try {
        const existingDefault = this._defaultPrefs.get(pref);
        if (existingDefault !== undefined && prefConfig.value === undefined) {
          prefConfig.getValue = () => existingDefault;
        }
      } catch (ex) {
        // We get NS_ERROR_UNEXPECTED for prefs that have a user value (causing
        // default branch to believe there's a type) but no actual default value
      }

      // Compute the dynamic value (potentially generic based on dummy geo)
      const newValue = prefConfig.getValue({
        geo: this.geo,
        locale: this.locale,
      });

      // If there's an existing value and it has changed, that means we need to
      // overwrite the default with the new value.
      if (prefConfig.value !== undefined && prefConfig.value !== newValue) {
        this._defaultPrefs.set(pref, newValue);
      }

      prefConfig.value = newValue;
    }
  }

  observe(subject, topic, data) {
    switch (topic) {
      case "nsPref:changed":
        // We should only expect one geo change, so update and stop observing
        if (data === GEO_PREF) {
          this._updateDynamicPrefs();
          Services.prefs.removeObserver(GEO_PREF, this);
        }
        break;
    }
  }
};

const EXPORTED_SYMBOLS = ["ActivityStream", "PREFS_CONFIG"];
