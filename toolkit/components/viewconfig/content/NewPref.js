"use strict";

const { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");

var params = window.arguments[0]
  , win = window.opener
  , prefs = Services.prefs
  , nameTB, valueElem, success = false;
  ;

async function init() {
  document.getElementById('save').addEventListener("click", onSave);
  document.getElementById('cancel').addEventListener("click", () => window.close());

  nameTB = document.getElementById("name");
  nameTB.value = params.prefCol;

  switch (params.typeCol) {
    case prefs.PREF_BOOL:
      valueElem = document.getElementById("bool");
      break;
    case prefs.PREF_INT:
      valueElem = document.getElementById("number");
      break;
    case prefs.PREF_STRING:
      valueElem = document.getElementById("text");
  }
  valueElem.style.display = "";

  document.getElementById("valueLabel").textContent =
    await document.l10n.formatValue(
            "config-modify-title", { type: win.gTypeStrs[params.typeCol] }
          );

  if (params.prefCol) {
    document.title = params.prefCol;
    valueElem.value = params.valueCol;
    valueElem.focus();
  } else {
    document.title =
      await document.l10n.formatValue(
              "config-new-title", { type: win.gTypeStrs[params.typeCol] }
            );
    nameTB.parentElement.style.display = "";
    nameTB.focus();
  }

  win = window.outerWidth;
  window.sizeToContent();
  window.outerWidth = win;
}

function onSave() {
  var name = nameTB.value.trim(), value, tailPos;

  if (params.typeCol == prefs.PREF_BOOL) {
    if (!name) return;

    if (valueElem.selectedIndex < 0) {
      tailPos = name.lastIndexOf(";");
      if (tailPos >=0) {
        switch (name.substring(tailPos + 1).trim().toLowerCase()) {
          case "false":
            valueElem.selectedIndex = 0;
            nameTB.value = name.substring(0, tailPos).trim();
            break;
          case "true":
            valueElem.selectedIndex = 1;
            nameTB.value = name.substring(0, tailPos).trim();
          default:
        }
      }
      return;
    }

    switch (prefs.getPrefType(name)) {
      case prefs.PREF_BOOL:
        if (!params.prefCol)
          break;
      case prefs.PREF_INVALID:
        prefs.setBoolPref(name, valueElem.selectedIndex);
    }
  } else {
    value = valueElem.value.trim();
    if (!(name || value)) return;

    if (!name) {
      name = value;
      value = "";
    }
    if (!value) {
      tailPos = name.indexOf(";");
      if (tailPos >=0) {
        nameTB.value = name.substring(0, tailPos).trim();
        valueElem.value = name.substring(tailPos + 1).trim();
        return;
      }
    }

    if (params.typeCol == prefs.PREF_INT) {
      if (!valueElem.reportValidity()) return;

      switch (prefs.getPrefType(name)) {
        case prefs.PREF_INT:
          if (!params.prefCol)
            break;
        case prefs.PREF_INVALID:
          prefs.setIntPref(name, parseInt(value));
      }
    } else {
      switch (prefs.getPrefType(name)) {
        case prefs.PREF_STRING:
          if (!params.prefCol)
            break;
        case prefs.PREF_INVALID:
          prefs.setCharPref(name, value);
      }
    }
  }
  prefs.savePrefFile(null);
  params.prefCol = name;
  success = true;
  window.close();
}

window.addEventListener('load', init);
