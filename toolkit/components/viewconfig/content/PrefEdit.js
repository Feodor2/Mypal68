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

/* params:
.prefCol  - имя параметра
.typeCol  - тип параметра
.valueCol - значение
.lockCol  - PREF_IS_*
*/
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

  if (params.prefCol) { // изменение существующего параметра
    document.title = params.prefCol;
    valueElem.value = params.valueCol;
    valueElem.focus();
  } else { // создание нового параметра
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
    // строки для ввода значения здесь нет, поэтому при отсутствии имени возврат в диалог
    if (!name) return;

    if (valueElem.selectedIndex < 0) {
      // значение из списка не выбрано, поэтому попытаться использовать его из хвоста имени и вернуться в диалог
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
//        default:  ничего не менять
        }
      }
      return;
    }

    // сохранить, только если тип совпадает (изменение) или параметра нет (создание)
    switch (prefs.getPrefType(name)) {
      case prefs.PREF_BOOL:  // параметр существует
        if (!params.prefCol) // вызывали создание нового параметра -
          break;             // вывалиться без сохранения
      case prefs.PREF_INVALID: // параметр ещё не существует
        prefs.setBoolPref(name, valueElem.selectedIndex);
    }
  } else { // PREF_INT, PREF_STRING
    value = valueElem.value.trim();
    // проверить заполненность хотя бы одного из полей
    if (!(name || value)) return; // если нет - вернуться в диалог

    // если заполнено только одно из полей, попытаться разбить содержимое другого поля по последней ";"
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
      if (!valueElem.reportValidity()) return; // значение - не целое число

      // сохранить, только если тип совпадает (изменение) или параметра нет (создание)
      switch (prefs.getPrefType(name)) {
        case prefs.PREF_INT:  // параметр существует
          if (!params.prefCol) // вызывали создание нового параметра -
            break;             // вывалиться без сохранения
        case prefs.PREF_INVALID: // параметр ещё не существует
          prefs.setIntPref(name, parseInt(value));
      }
    } else { // PREF_STRING
      // сохранить, только если тип совпадает (изменение) или параметра нет (создание)
      switch (prefs.getPrefType(name)) {
        case prefs.PREF_STRING:  // параметр существует
          if (!params.prefCol) // вызывали создание нового параметра -
            break;             // вывалиться без сохранения
        case prefs.PREF_INVALID: // параметр ещё не существует
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
