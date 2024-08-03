/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

"use strict";

const { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");
const { AddonManager } = ChromeUtils.import(
  "resource://gre/modules/AddonManager.jsm"
);

async function renderPluginMetadata(id) {
  let plugin = await AddonManager.getAddonByID(id);
  if (!plugin) {
    return;
  }

  let libLabel = document.getElementById("pluginLibraries");
  libLabel.textContent = plugin.pluginLibraries.join(", ");

  let typeLabel = document.getElementById("pluginMimeTypes"),
    types = [];
  for (let type of plugin.pluginMimeTypes) {
    let extras = [type.description.trim(), type.suffixes]
      .filter(x => x)
      .join(": ");
    types.push(type.type + (extras ? " (" + extras + ")" : ""));
  }
  typeLabel.textContent = types.join(",\n");
}

function isFlashPlugin(aPlugin) {
  for (let type of aPlugin.pluginMimeTypes) {
    if (type.type == "application/x-shockwave-flash") {
      return true;
    }
  }
  return false;
}
// Protected mode is win32-only, not win64
function canDisableFlashProtectedMode(aPlugin) {
  return isFlashPlugin(aPlugin) && Services.appinfo.XPCOMABI == "x86-msvc";
}

function init() {
  let params = new URLSearchParams(location.hash.slice(1));
  renderPluginMetadata(params.get("id"));
}

window.addEventListener("load", init, { once: true });
