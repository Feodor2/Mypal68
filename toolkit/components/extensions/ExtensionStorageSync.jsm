/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * Local-only implementation of browser.storage.sync for Mypal.
 *
 * The upstream Firefox 68 implementation used Kinto + Firefox Accounts to
 * upload extension settings to a Mozilla-hosted server. Mypal has no Firefox
 * Accounts (identity.fxaccounts.enabled = false, auth.uri = "", serverURL =
 * ""), so all of the FxA / keyring / crypto / HTTP-sync code was dead weight.
 *
 * This file keeps only the local half: one Kinto collection per extension,
 * stored in <profile>/storage-sync.sqlite, with exactly the same API surface
 * used by parent/ext-storage.js and by
 * services/sync/modules/engines/extension-storage.js. No network requests,
 * no account, no encryption keyring. Data stays in the profile.
 *
 * Upstream is doing the same cleanup for the leftover desktop Kinto backend:
 * https://bugzilla.mozilla.org/show_bug.cgi?id=2080817
 */
"use strict";

/* exported keyToId, idToKey, storageSyncIdSchema, storageSyncInit, openCollection */
/* exported ExtensionStorageSync */

var EXPORTED_SYMBOLS = ["ExtensionStorageSync", "extensionStorageSync"];

const global = this;

const { XPCOMUtils } = ChromeUtils.import(
  "resource://gre/modules/XPCOMUtils.jsm"
);

XPCOMUtils.defineLazyModuleGetters(this, {
  ExtensionCommon: "resource://gre/modules/ExtensionCommon.jsm",
  FirefoxAdapter: "resource://services-common/kinto-storage-adapter.js",
  Kinto: "resource://services-common/kinto-offline-client.js",
  Observers: "resource://services-common/observers.js",
});

const STORAGE_SYNC_ENABLED_PREF = "webextensions.storage.sync.enabled";

XPCOMUtils.defineLazyPreferenceGetter(
  this,
  "prefPermitsStorageSync",
  STORAGE_SYNC_ENABLED_PREF,
  true
);

// Default is 5sec, which seems a bit aggressive on the open internet.
// Kept for parity with the original Kinto options (only local ops run now).
const KINTO_REQUEST_TIMEOUT = 30000;

// Collection names opened during this session, used by clearAll().
const openedCollections = new Set();

/**
 * Open (once) the SQLite connection and the Kinto instance backed by it.
 * Memoized so concurrent storage.sync calls share one connection.
 */
async function storageSyncInit() {
  if (storageSyncInit.promise === undefined) {
    const path = "storage-sync.sqlite";
    storageSyncInit.promise = FirefoxAdapter.openConnection({ path })
      .then(connection => {
        return {
          connection,
          kinto: new Kinto({
            adapter: FirefoxAdapter,
            adapterOptions: { sqliteHandle: connection },
            timeout: KINTO_REQUEST_TIMEOUT,
            retry: 0,
          }),
        };
      })
      .catch(e => {
        // Ensure one failure doesn't break us forever.
        Cu.reportError(e);
        storageSyncInit.promise = undefined;
        throw e;
      });
  }
  return storageSyncInit.promise;
}

// Kinto record IDs must contain only ASCII alphanumerics plus - and _, and
// must start with a letter. Encode everything else as _XX_ and prefix "key-".
function keyToId(key) {
  function escapeChar(match) {
    return (
      "_" +
      match
        .codePointAt(0)
        .toString(16)
        .toUpperCase() +
      "_"
    );
  }
  return "key-" + key.replace(/[^a-zA-Z0-9]/g, escapeChar);
}

// Convert a Kinto ID back into a chrome.storage key. Returns null on failure.
function idToKey(id) {
  function unescapeNumber(match, group1) {
    return String.fromCodePoint(parseInt(group1, 16));
  }
  const ESCAPED_ID_FORMAT = /^(?:[a-zA-Z0-9]|_[0-9A-F]+_)*$/;

  if (!id.startsWith("key-")) {
    return null;
  }
  const unprefixed = id.slice(4);
  if (!ESCAPED_ID_FORMAT.test(unprefixed)) {
    return null;
  }
  return unprefixed.replace(/_([0-9A-F]+)_/g, unescapeNumber);
}

// chrome.storage is a key-value store: we always supply our own IDs.
const storageSyncIdSchema = {
  generate() {
    throw new Error("cannot generate IDs");
  },
  validate(id) {
    return idToKey(id) !== null;
  },
};

/**
 * Return the local collection for an extension. No crypto collection, no
 * remote transformers: those were only needed to encrypt records for upload.
 */
const openCollection = async function(extension) {
  const { kinto } = await storageSyncInit();
  openedCollections.add(extension.id);
  return kinto.collection(extension.id, { idSchema: storageSyncIdSchema });
};

var extensionStorageSync = null;

class ExtensionStorageSync {
  constructor() {
    this.listeners = new WeakMap();
  }

  /**
   * Still honor the pref as an escape hatch. The default is now true
   * (modules/libpref/init/all.js); the old "disabled but exposed" state was
   * Mozilla bug 1331179 and broke extensions that use storage.sync.
   */
  getCollection(extension, context) {
    if (prefPermitsStorageSync !== true) {
      return Promise.reject({
        message: `Please set ${STORAGE_SYNC_ENABLED_PREF} to true in about:config`,
      });
    }
    return openCollection(extension);
  }

  async set(extension, items, context) {
    const coll = await this.getCollection(extension, context);
    const keys = Object.keys(items);
    const ids = keys.map(keyToId);
    const changes = await coll.execute(
      txn => {
        let changes = {};
        for (let [i, key] of keys.entries()) {
          const id = ids[i];
          let item = items[key];
          let { oldRecord } = txn.upsert({
            id,
            key,
            data: item,
          });
          changes[key] = { newValue: item };
          if (oldRecord && oldRecord.data) {
            changes[key].oldValue = oldRecord.data;
          }
        }
        return changes;
      },
      { preloadIds: ids }
    );
    this.notifyListeners(extension, changes);
  }

  async remove(extension, keys, context) {
    const coll = await this.getCollection(extension, context);
    keys = [].concat(keys);
    const ids = keys.map(keyToId);
    let changes = {};
    await coll.execute(
      txn => {
        for (let [i, key] of keys.entries()) {
          const id = ids[i];
          const res = txn.deleteAny(id);
          if (res.deleted) {
            changes[key] = { oldValue: res.data.data };
          }
        }
        return changes;
      },
      { preloadIds: ids }
    );
    if (Object.keys(changes).length > 0) {
      this.notifyListeners(extension, changes);
    }
  }

  async clear(extension, context) {
    // Go through remove() so the onChanged listeners still fire.
    const coll = await this.getCollection(extension, context);
    const res = await coll.list();
    const keys = res.data.map(record => record.key);
    await this.remove(extension, keys, context);
  }

  async get(extension, spec, context) {
    const coll = await this.getCollection(extension, context);
    let keys, records;
    if (spec === null) {
      records = {};
      const res = await coll.list();
      for (let record of res.data) {
        records[record.key] = record.data;
      }
      return records;
    }
    if (typeof spec === "string") {
      keys = [spec];
      records = {};
    } else if (Array.isArray(spec)) {
      keys = spec;
      records = {};
    } else {
      keys = Object.keys(spec);
      records = Cu.cloneInto(spec, global);
    }

    for (let key of keys) {
      const res = await coll.getAny(keyToId(key));
      if (res.data && res.data._status != "deleted") {
        records[res.data.key] = res.data.data;
      }
    }
    return records;
  }

  addOnChangedListener(extension, listener, context) {
    let listeners = this.listeners.get(extension) || new Set();
    listeners.add(listener);
    this.listeners.set(extension, listeners);
  }

  removeOnChangedListener(extension, listener) {
    let listeners = this.listeners.get(extension);
    if (!listeners) {
      return;
    }
    listeners.delete(listener);
    if (listeners.size == 0) {
      this.listeners.delete(extension);
    }
  }

  notifyListeners(extension, changes) {
    // Kept for compatibility with the Extension-Storage Sync tracker. With
    // no FxA account the engine never syncs anything.
    Observers.notify("ext.storage.sync-changed");
    let listeners = this.listeners.get(extension) || new Set();
    for (let listener of listeners) {
      ExtensionCommon.runSafeSyncWithoutClone(listener, changes);
    }
  }

  // There is no server in Mypal. These two are no-ops / local wipes, kept so
  // services/sync/modules/engines/extension-storage.js and its tests still
  // have a valid API to call.
  async syncAll() {}

  async clearAll() {
    const { kinto } = await storageSyncInit();
    for (const id of openedCollections) {
      await kinto.collection(id, { idSchema: storageSyncIdSchema }).clear();
    }
  }
}
this.ExtensionStorageSync = ExtensionStorageSync;
extensionStorageSync = new ExtensionStorageSync();
