/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, { enumerable: true, get: getter });
/******/ 		}
/******/ 	};
/******/
/******/ 	// define __esModule on exports
/******/ 	__webpack_require__.r = function(exports) {
/******/ 		if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 			Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 		}
/******/ 		Object.defineProperty(exports, '__esModule', { value: true });
/******/ 	};
/******/
/******/ 	// create a fake namespace object
/******/ 	// mode & 1: value is a module id, require it
/******/ 	// mode & 2: merge all properties of value into the ns
/******/ 	// mode & 4: return value when already ns object
/******/ 	// mode & 8|1: behave like require
/******/ 	__webpack_require__.t = function(value, mode) {
/******/ 		if(mode & 1) value = __webpack_require__(value);
/******/ 		if(mode & 8) return value;
/******/ 		if((mode & 4) && typeof value === 'object' && value && value.__esModule) return value;
/******/ 		var ns = Object.create(null);
/******/ 		__webpack_require__.r(ns);
/******/ 		Object.defineProperty(ns, 'default', { enumerable: true, value: value });
/******/ 		if(mode & 2 && typeof value != 'string') for(var key in value) __webpack_require__.d(ns, key, function(key) { return value[key]; }.bind(null, key));
/******/ 		return ns;
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = 0);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var content_src_components_Base_Base__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(3);
/* harmony import */ var content_src_lib_detect_user_session_start__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(48);
/* harmony import */ var content_src_lib_init_store__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(6);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(24);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_4___default = /*#__PURE__*/__webpack_require__.n(react_redux__WEBPACK_IMPORTED_MODULE_4__);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_5___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_5__);
/* harmony import */ var react_dom__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(14);
/* harmony import */ var react_dom__WEBPACK_IMPORTED_MODULE_6___default = /*#__PURE__*/__webpack_require__.n(react_dom__WEBPACK_IMPORTED_MODULE_6__);
/* harmony import */ var common_Reducers_jsm__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(53);








const store = Object(content_src_lib_init_store__WEBPACK_IMPORTED_MODULE_3__["initStore"])(common_Reducers_jsm__WEBPACK_IMPORTED_MODULE_7__["reducers"]);
new content_src_lib_detect_user_session_start__WEBPACK_IMPORTED_MODULE_2__["DetectUserSessionStart"](store).sendEventOrAddListener();
store.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].AlsoToMain({
  type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].NEW_TAB_STATE_REQUEST
}));
react_dom__WEBPACK_IMPORTED_MODULE_6___default.a.hydrate(react__WEBPACK_IMPORTED_MODULE_5___default.a.createElement(react_redux__WEBPACK_IMPORTED_MODULE_4__["Provider"], {
  store: store
}, react__WEBPACK_IMPORTED_MODULE_5___default.a.createElement(content_src_components_Base_Base__WEBPACK_IMPORTED_MODULE_1__["Base"], {
  isFirstrun: global.document.location.href === "about:welcome",
  locale: global.document.documentElement.lang,
  strings: global.gActivityStreamStrings
})), document.getElementById("root"));
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 1 */
/***/ (function(module, exports) {

var g;

// This works in non-strict mode
g = (function() {
	return this;
})();

try {
	// This works if eval is allowed (see CSP)
	g = g || new Function("return this")();
} catch (e) {
	// This works if the window reference is available
	if (typeof window === "object") g = window;
}

// g can still be undefined, but nothing to do about it...
// We return undefined, instead of nothing here, so it's
// easier to handle this case. if(!global) { ...}

module.exports = g;


/***/ }),
/* 2 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "MAIN_MESSAGE_TYPE", function() { return MAIN_MESSAGE_TYPE; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "CONTENT_MESSAGE_TYPE", function() { return CONTENT_MESSAGE_TYPE; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "PRELOAD_MESSAGE_TYPE", function() { return PRELOAD_MESSAGE_TYPE; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "UI_CODE", function() { return UI_CODE; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "BACKGROUND_PROCESS", function() { return BACKGROUND_PROCESS; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "globalImportContext", function() { return globalImportContext; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "actionTypes", function() { return actionTypes; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "actionCreators", function() { return actionCreators; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "actionUtils", function() { return actionUtils; });
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


var MAIN_MESSAGE_TYPE = "ActivityStream:Main";
var CONTENT_MESSAGE_TYPE = "ActivityStream:Content";
var PRELOAD_MESSAGE_TYPE = "ActivityStream:PreloadedBrowser";
var UI_CODE = 1;
var BACKGROUND_PROCESS = 2;
/**
 * globalImportContext - Are we in UI code (i.e. react, a dom) or some kind of background process?
 *                       Use this in action creators if you need different logic
 *                       for ui/background processes.
 */

const globalImportContext = typeof Window === "undefined" ? BACKGROUND_PROCESS : UI_CODE; // Export for tests

// Create an object that avoids accidental differing key/value pairs:
// {
//   INIT: "INIT",
//   UNINIT: "UNINIT"
// }
const actionTypes = {};

for (const type of ["ADDONS_INFO_REQUEST", "ADDONS_INFO_RESPONSE", "AS_ROUTER_INITIALIZED", "AS_ROUTER_PREF_CHANGED", "AS_ROUTER_TARGETING_UPDATE", "BLOCK_URL", "BOOKMARK_URL", "COPY_DOWNLOAD_LINK", "DELETE_BOOKMARK_BY_ID", "DELETE_HISTORY_URL", "DIALOG_CANCEL", "DIALOG_OPEN", "DOWNLOAD_CHANGED", "FAKE_FOCUS_SEARCH", "FILL_SEARCH_TERM", "HANDOFF_SEARCH_TO_AWESOMEBAR", "HIDE_SEARCH", "INIT", "NEW_TAB_INIT", "NEW_TAB_INITIAL_STATE", "NEW_TAB_LOAD", "NEW_TAB_REHYDRATED", "NEW_TAB_STATE_REQUEST", "NEW_TAB_UNLOAD", "OPEN_DOWNLOAD_FILE", "OPEN_LINK", "OPEN_NEW_WINDOW", "OPEN_PRIVATE_WINDOW", "OPEN_WEBEXT_SETTINGS", "PLACES_BOOKMARK_ADDED", "PLACES_BOOKMARK_REMOVED", "PLACES_HISTORY_CLEARED", "PLACES_LINKS_CHANGED", "PLACES_LINK_BLOCKED", "PLACES_LINK_DELETED", "PREFS_INITIAL_VALUES", "PREF_CHANGED", "PREVIEW_REQUEST", "PREVIEW_REQUEST_CANCEL", "PREVIEW_RESPONSE", "REMOVE_DOWNLOAD_FILE", "RICH_ICON_MISSING", "SAVE_SESSION_PERF_DATA", "SCREENSHOT_UPDATED", "SECTION_DEREGISTER", "SECTION_DISABLE", "SECTION_ENABLE", "SECTION_MOVE", "SECTION_OPTIONS_CHANGED", "SECTION_REGISTER", "SECTION_UPDATE", "SECTION_UPDATE_CARD", "SETTINGS_CLOSE", "SETTINGS_OPEN", "SET_PREF", "SHOW_DOWNLOAD_FILE", "SHOW_FIREFOX_ACCOUNTS", "SHOW_SEARCH", "SKIPPED_SIGNIN", "SUBMIT_EMAIL", "SYSTEM_TICK", "TOP_SITES_CANCEL_EDIT", "TOP_SITES_CLOSE_SEARCH_SHORTCUTS_MODAL", "TOP_SITES_EDIT", "TOP_SITES_INSERT", "TOP_SITES_OPEN_SEARCH_SHORTCUTS_MODAL", "TOP_SITES_PIN", "TOP_SITES_PREFS_UPDATED", "TOP_SITES_UNPIN", "TOP_SITES_UPDATED", "TOTAL_BOOKMARKS_REQUEST", "TOTAL_BOOKMARKS_RESPONSE", "UNINIT", "UPDATE_PINNED_SEARCH_SHORTCUTS", "UPDATE_SEARCH_SHORTCUTS", "UPDATE_SECTION_PREFS", "WEBEXT_CLICK", "WEBEXT_DISMISS"]) {
  actionTypes[type] = type;
} // These are acceptable actions for AS Router messages to have. They can show up
// as call-to-action buttons in snippets, onboarding tour, etc.

function _RouteMessage(action, options) {
  const meta = action.meta ? { ...action.meta
  } : {};

  if (!options || !options.from || !options.to) {
    throw new Error("Routed Messages must have options as the second parameter, and must at least include a .from and .to property.");
  } // For each of these fields, if they are passed as an option,
  // add them to the action. If they are not defined, remove them.


  ["from", "to", "toTarget", "fromTarget", "skipMain", "skipLocal"].forEach(o => {
    if (typeof options[o] !== "undefined") {
      meta[o] = options[o];
    } else if (meta[o]) {
      delete meta[o];
    }
  });
  return { ...action,
    meta
  };
}
/**
 * AlsoToMain - Creates a message that will be dispatched locally and also sent to the Main process.
 *
 * @param  {object} action Any redux action (required)
 * @param  {object} options
 * @param  {bool}   skipLocal Used by OnlyToMain to skip the main reducer
 * @param  {string} fromTarget The id of the content port from which the action originated. (optional)
 * @return {object} An action with added .meta properties
 */


function AlsoToMain(action, fromTarget, skipLocal) {
  return _RouteMessage(action, {
    from: CONTENT_MESSAGE_TYPE,
    to: MAIN_MESSAGE_TYPE,
    fromTarget,
    skipLocal
  });
}
/**
 * OnlyToMain - Creates a message that will be sent to the Main process and skip the local reducer.
 *
 * @param  {object} action Any redux action (required)
 * @param  {object} options
 * @param  {string} fromTarget The id of the content port from which the action originated. (optional)
 * @return {object} An action with added .meta properties
 */


function OnlyToMain(action, fromTarget) {
  return AlsoToMain(action, fromTarget, true);
}
/**
 * BroadcastToContent - Creates a message that will be dispatched to main and sent to ALL content processes.
 *
 * @param  {object} action Any redux action (required)
 * @return {object} An action with added .meta properties
 */


function BroadcastToContent(action) {
  return _RouteMessage(action, {
    from: MAIN_MESSAGE_TYPE,
    to: CONTENT_MESSAGE_TYPE
  });
}
/**
 * AlsoToOneContent - Creates a message that will be will be dispatched to the main store
 *                    and also sent to a particular Content process.
 *
 * @param  {object} action Any redux action (required)
 * @param  {string} target The id of a content port
 * @param  {bool} skipMain Used by OnlyToOneContent to skip the main process
 * @return {object} An action with added .meta properties
 */


function AlsoToOneContent(action, target, skipMain) {
  if (!target) {
    throw new Error("You must provide a target ID as the second parameter of AlsoToOneContent. If you want to send to all content processes, use BroadcastToContent");
  }

  return _RouteMessage(action, {
    from: MAIN_MESSAGE_TYPE,
    to: CONTENT_MESSAGE_TYPE,
    toTarget: target,
    skipMain
  });
}
/**
 * OnlyToOneContent - Creates a message that will be sent to a particular Content process
 *                    and skip the main reducer.
 *
 * @param  {object} action Any redux action (required)
 * @param  {string} target The id of a content port
 * @return {object} An action with added .meta properties
 */


function OnlyToOneContent(action, target) {
  return AlsoToOneContent(action, target, true);
}
/**
 * AlsoToPreloaded - Creates a message that dispatched to the main reducer and also sent to the preloaded tab.
 *
 * @param  {object} action Any redux action (required)
 * @return {object} An action with added .meta properties
 */


function AlsoToPreloaded(action) {
  return _RouteMessage(action, {
    from: MAIN_MESSAGE_TYPE,
    to: PRELOAD_MESSAGE_TYPE
  });
}

function SetPref(name, value, importContext = globalImportContext) {
  const action = {
    type: actionTypes.SET_PREF,
    data: {
      name,
      value
    }
  };
  return importContext === UI_CODE ? AlsoToMain(action) : action;
}

function WebExtEvent(type, data, importContext = globalImportContext) {
  if (!data || !data.source) {
    throw new Error("WebExtEvent actions should include a property \"source\", the id of the webextension that should receive the event.");
  }

  const action = {
    type,
    data
  };
  return importContext === UI_CODE ? AlsoToMain(action) : action;
}

var actionCreators = {
  BroadcastToContent,
  AlsoToOneContent,
  OnlyToOneContent,
  AlsoToMain,
  OnlyToMain,
  AlsoToPreloaded,
  SetPref,
  WebExtEvent
}; // These are helpers to test for certain kinds of actions

var actionUtils = {
  isSendToMain(action) {
    if (!action.meta) {
      return false;
    }

    return action.meta.to === MAIN_MESSAGE_TYPE && action.meta.from === CONTENT_MESSAGE_TYPE;
  },

  isBroadcastToContent(action) {
    if (!action.meta) {
      return false;
    }

    if (action.meta.to === CONTENT_MESSAGE_TYPE && !action.meta.toTarget) {
      return true;
    }

    return false;
  },

  isSendToOneContent(action) {
    if (!action.meta) {
      return false;
    }

    if (action.meta.to === CONTENT_MESSAGE_TYPE && action.meta.toTarget) {
      return true;
    }

    return false;
  },

  isSendToPreloaded(action) {
    if (!action.meta) {
      return false;
    }

    return action.meta.to === PRELOAD_MESSAGE_TYPE && action.meta.from === MAIN_MESSAGE_TYPE;
  },

  isFromMain(action) {
    if (!action.meta) {
      return false;
    }

    return action.meta.from === MAIN_MESSAGE_TYPE && action.meta.to === CONTENT_MESSAGE_TYPE;
  },

  getPortIdOfSender(action) {
    return action.meta && action.meta.fromTarget || null;
  },

  _RouteMessage
};

/***/ }),
/* 3 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_Base", function() { return _Base; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "BaseContent", function() { return BaseContent; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Base", function() { return Base; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var _asrouter_asrouter_content__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(5);
/* harmony import */ var content_src_components_ConfirmDialog_ConfirmDialog__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(27);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(24);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_4___default = /*#__PURE__*/__webpack_require__.n(react_redux__WEBPACK_IMPORTED_MODULE_4__);
/* harmony import */ var content_src_components_ErrorBoundary_ErrorBoundary__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(31);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_7___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_7__);
/* harmony import */ var content_src_components_Search_Search__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(47);
/* harmony import */ var content_src_components_Sections_Sections__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(36);
function _extends() { _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; }; return _extends.apply(this, arguments); }












const PrefsButton = props => react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement("div", {
  className: "prefs-button"
}, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement("button", {
  className: "icon icon-settings",
  onClick: props.onClick,
  "data-l10n-id": "newtab-settings-button"
})); // Returns a function will not be continuously triggered when called. The
// function will be triggered if called again after `wait` milliseconds.


function debounce(func, wait) {
  let timer;
  return (...args) => {
    if (timer) {
      return;
    }

    let wakeUp = () => {
      timer = null;
    };

    timer = setTimeout(wakeUp, wait);
    func.apply(this, args);
  };
}

class _Base extends react__WEBPACK_IMPORTED_MODULE_7___default.a.PureComponent {
  componentWillMount() {
    if (this.props.isFirstrun) {
      global.document.body.classList.add("welcome", "hide-main");
    }
  }

  componentWillUnmount() {
    this.updateTheme();
  }

  componentWillUpdate() {
    this.updateTheme();
  }

  updateTheme() {
    const bodyClassName = ["activity-stream", // If we skipped the about:welcome overlay and removed the CSS classes
    // we don't want to add them back to the Activity Stream view
    document.body.classList.contains("welcome") ? "welcome" : "", document.body.classList.contains("hide-main") ? "hide-main" : "", document.body.classList.contains("inline-onboarding") ? "inline-onboarding" : ""].filter(v => v).join(" ");
    global.document.body.className = bodyClassName;
  }

  render() {
    const {
      props
    } = this;
    const {
      App
    } = props;

    if (!App.initialized) {
      return null;
    }

    return react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(content_src_components_ErrorBoundary_ErrorBoundary__WEBPACK_IMPORTED_MODULE_6__["ErrorBoundary"], {
      className: "base-content-fallback"
    }, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(react__WEBPACK_IMPORTED_MODULE_7___default.a.Fragment, null, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(BaseContent, this.props), null));
  }

}
class BaseContent extends react__WEBPACK_IMPORTED_MODULE_7___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.openPreferences = this.openPreferences.bind(this);
    this.onWindowScroll = debounce(this.onWindowScroll.bind(this), 5);
    this.state = {
      fixedSearch: false
    };
  }

  componentDidMount() {
    global.addEventListener("scroll", this.onWindowScroll);
  }

  componentWillUnmount() {
    global.removeEventListener("scroll", this.onWindowScroll);
  }

  onWindowScroll() {
    const SCROLL_THRESHOLD = 34;

    if (global.scrollY > SCROLL_THRESHOLD && !this.state.fixedSearch) {
      this.setState({
        fixedSearch: true
      });
    } else if (global.scrollY <= SCROLL_THRESHOLD && this.state.fixedSearch) {
      this.setState({
        fixedSearch: false
      });
    }
  }

  openPreferences() {
    this.props.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].SETTINGS_OPEN
    }));
  }

  render() {
    const {
      props
    } = this;
    const {
      App
    } = props;
    const {
      initialized
    } = App;
    const prefs = props.Prefs.values;
    let filteredSections = props.Sections; // Filter out highlights for DS

    const noSectionsEnabled = !prefs["feeds.topsites"] && filteredSections.filter(section => section.enabled).length === 0;
    const searchHandoffEnabled = prefs["improvesearch.handoffToAwesomebar"];
    const outerClassName = ["outer-wrapper", prefs.showSearch && this.state.fixedSearch && !noSectionsEnabled && "fixed-search", prefs.showSearch && noSectionsEnabled && "only-search"].filter(v => v).join(" ");
    return react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement("div", null, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement("div", {
      className: outerClassName
    }, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement("main", null, prefs.showSearch && react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement("div", {
      className: "non-collapsible-section"
    }, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(content_src_components_ErrorBoundary_ErrorBoundary__WEBPACK_IMPORTED_MODULE_6__["ErrorBoundary"], null, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(content_src_components_Search_Search__WEBPACK_IMPORTED_MODULE_8__["Search"], _extends({
      showLogo: noSectionsEnabled,
      handoffEnabled: searchHandoffEnabled
    }, props.Search)))), react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement("div", {
      className: `body-wrapper${initialized ? " on" : ""}`
    }, react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(content_src_components_Sections_Sections__WEBPACK_IMPORTED_MODULE_9__["Sections"], null), react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(PrefsButton, {
      onClick: this.openPreferences
    })), react__WEBPACK_IMPORTED_MODULE_7___default.a.createElement(content_src_components_ConfirmDialog_ConfirmDialog__WEBPACK_IMPORTED_MODULE_3__["ConfirmDialog"], null))));
  }

}
const Base = Object(react_redux__WEBPACK_IMPORTED_MODULE_4__["connect"])(state => ({
  App: state.App,
  Prefs: state.Prefs,
  Sections: state.Sections,
  Search: state.Search
}))(_Base);
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 4 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 5 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 6 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "MERGE_STORE_ACTION", function() { return MERGE_STORE_ACTION; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "OUTGOING_MESSAGE_NAME", function() { return OUTGOING_MESSAGE_NAME; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "INCOMING_MESSAGE_NAME", function() { return INCOMING_MESSAGE_NAME; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "EARLY_QUEUED_ACTIONS", function() { return EARLY_QUEUED_ACTIONS; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "rehydrationMiddleware", function() { return rehydrationMiddleware; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "queueEarlyMessageMiddleware", function() { return queueEarlyMessageMiddleware; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "initStore", function() { return initStore; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var redux__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(7);
/* harmony import */ var redux__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(redux__WEBPACK_IMPORTED_MODULE_1__);
/* eslint-env mozilla/frame-script */


const MERGE_STORE_ACTION = "NEW_TAB_INITIAL_STATE";
const OUTGOING_MESSAGE_NAME = "ActivityStream:ContentToMain";
const INCOMING_MESSAGE_NAME = "ActivityStream:MainToContent";
const EARLY_QUEUED_ACTIONS = [common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].SAVE_SESSION_PERF_DATA];
/**
 * A higher-order function which returns a reducer that, on MERGE_STORE action,
 * will return the action.data object merged into the previous state.
 *
 * For all other actions, it merely calls mainReducer.
 *
 * Because we want this to merge the entire state object, it's written as a
 * higher order function which takes the main reducer (itself often a call to
 * combineReducers) as a parameter.
 *
 * @param  {function} mainReducer reducer to call if action != MERGE_STORE_ACTION
 * @return {function}             a reducer that, on MERGE_STORE_ACTION action,
 *                                will return the action.data object merged
 *                                into the previous state, and the result
 *                                of calling mainReducer otherwise.
 */

function mergeStateReducer(mainReducer) {
  return (prevState, action) => {
    if (action.type === MERGE_STORE_ACTION) {
      return { ...prevState,
        ...action.data
      };
    }

    return mainReducer(prevState, action);
  };
}
/**
 * messageMiddleware - Middleware that looks for SentToMain type actions, and sends them if necessary
 */


const messageMiddleware = store => next => action => {
  const skipLocal = action.meta && action.meta.skipLocal;

  if (common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionUtils"].isSendToMain(action)) {
    RPMSendAsyncMessage(OUTGOING_MESSAGE_NAME, action);
  }

  if (!skipLocal) {
    next(action);
  }
};

const rehydrationMiddleware = store => next => action => {
  if (store._didRehydrate) {
    return next(action);
  }

  const isMergeStoreAction = action.type === MERGE_STORE_ACTION;
  const isRehydrationRequest = action.type === common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].NEW_TAB_STATE_REQUEST;

  if (isRehydrationRequest) {
    store._didRequestInitialState = true;
    return next(action);
  }

  if (isMergeStoreAction) {
    store._didRehydrate = true;
    return next(action);
  } // If init happened after our request was made, we need to re-request


  if (store._didRequestInitialState && action.type === common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].INIT) {
    return next(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].AlsoToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].NEW_TAB_STATE_REQUEST
    }));
  }

  if (common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionUtils"].isBroadcastToContent(action) || common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionUtils"].isSendToOneContent(action) || common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionUtils"].isSendToPreloaded(action)) {
    // Note that actions received before didRehydrate will not be dispatched
    // because this could negatively affect preloading and the the state
    // will be replaced by rehydration anyway.
    return null;
  }

  return next(action);
};
/**
 * This middleware queues up all the EARLY_QUEUED_ACTIONS until it receives
 * the first action from main. This is useful for those actions for main which
 * require higher reliability, i.e. the action will not be lost in the case
 * that it gets sent before the main is ready to receive it. Conversely, any
 * actions allowed early are accepted to be ignorable or re-sendable.
 */

const queueEarlyMessageMiddleware = store => next => action => {
  if (store._receivedFromMain) {
    next(action);
  } else if (common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionUtils"].isFromMain(action)) {
    next(action);
    store._receivedFromMain = true; // Sending out all the early actions as main is ready now

    if (store._earlyActionQueue) {
      store._earlyActionQueue.forEach(next);

      store._earlyActionQueue = [];
    }
  } else if (EARLY_QUEUED_ACTIONS.includes(action.type)) {
    store._earlyActionQueue = store._earlyActionQueue || [];

    store._earlyActionQueue.push(action);
  } else {
    // Let any other type of action go through
    next(action);
  }
};
/**
 * initStore - Create a store and listen for incoming actions
 *
 * @param  {object} reducers An object containing Redux reducers
 * @param  {object} intialState (optional) The initial state of the store, if desired
 * @return {object}          A redux store
 */

function initStore(reducers) {
  const store = Object(redux__WEBPACK_IMPORTED_MODULE_1__["createStore"])(mergeStateReducer(Object(redux__WEBPACK_IMPORTED_MODULE_1__["combineReducers"])(reducers)), global.RPMAddMessageListener && Object(redux__WEBPACK_IMPORTED_MODULE_1__["applyMiddleware"])(rehydrationMiddleware, queueEarlyMessageMiddleware, messageMiddleware));
  store._didRehydrate = false;
  store._didRequestInitialState = false;

  if (global.RPMAddMessageListener) {
    global.RPMAddMessageListener(INCOMING_MESSAGE_NAME, msg => {
      try {
        store.dispatch(msg.data);
      } catch (ex) {
        console.error("Content msg:", msg, "Dispatch error: ", ex); // eslint-disable-line no-console

        dump(`Content msg: ${JSON.stringify(msg)}\nDispatch error: ${ex}\n${ex.stack}`);
      }
    });
  }

  return store;
}
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 7 */
/***/ (function(module, exports) {

module.exports = Redux;

/***/ }),
/* 8 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 9 */
/***/ (function(module, exports) {

module.exports = React;

/***/ }),
/* 10 */
/***/ (function(module, exports) {

module.exports = PropTypes;

/***/ }),
/* 11 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "IS_NEWTAB", function() { return IS_NEWTAB; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "NEWTAB_DARK_THEME", function() { return NEWTAB_DARK_THEME; });
const IS_NEWTAB = global.document && global.document.documentURI === "about:newtab";
const NEWTAB_DARK_THEME = {
  "ntp_background": {
    "r": 42,
    "g": 42,
    "b": 46,
    "a": 1
  },
  "ntp_text": {
    "r": 249,
    "g": 249,
    "b": 250,
    "a": 1
  },
  "sidebar": {
    "r": 56,
    "g": 56,
    "b": 61,
    "a": 1
  },
  "sidebar_text": {
    "r": 249,
    "g": 249,
    "b": 250,
    "a": 1
  }
};
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 12 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 13 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ModalOverlayWrapper", function() { return ModalOverlayWrapper; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ModalOverlay", function() { return ModalOverlay; });
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);

class ModalOverlayWrapper extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.onKeyDown = this.onKeyDown.bind(this);
  }

  onKeyDown(event) {
    if (event.key === "Escape") {
      this.props.onClose(event);
    }
  }

  componentWillMount() {
    this.props.document.addEventListener("keydown", this.onKeyDown);
    this.props.document.body.classList.add("modal-open");
  }

  componentWillUnmount() {
    this.props.document.removeEventListener("keydown", this.onKeyDown);
    this.props.document.body.classList.remove("modal-open");
  }

  render() {
    const {
      props
    } = this;
    let className = props.unstyled ? "" : "modalOverlayInner active";

    if (props.innerClassName) {
      className += ` ${props.innerClassName}`;
    }

    return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(react__WEBPACK_IMPORTED_MODULE_0___default.a.Fragment, null, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
      className: "modalOverlayOuter active",
      onClick: props.onClose,
      onKeyDown: this.onKeyDown,
      role: "presentation"
    }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
      className: className,
      "aria-labelledby": props.headerId,
      id: props.id,
      role: "dialog"
    }, props.children));
  }

}
ModalOverlayWrapper.defaultProps = {
  document: global.document
};
class ModalOverlay extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  render() {
    const {
      title,
      button_label
    } = this.props;
    return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(ModalOverlayWrapper, {
      onClose: this.props.onDismissBundle
    }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("h2", null, " ", title, " "), this.props.children, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("div", {
      className: "footer"
    }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("button", {
      className: "button primary modalButton",
      onClick: this.props.onDismissBundle
    }, " ", button_label, " ")));
  }

}
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 14 */
/***/ (function(module, exports) {

module.exports = ReactDOM;

/***/ }),
/* 15 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 16 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "convertLinks", function() { return convertLinks; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "RichText", function() { return RichText; });
/* harmony import */ var fluent_react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(50);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _rich_text_strings__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(52);
/* harmony import */ var _template_utils__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(17);
function _extends() { _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; }; return _extends.apply(this, arguments); }




 // Elements allowed in snippet content

const ALLOWED_TAGS = {
  b: react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("b", null),
  i: react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("i", null),
  u: react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("u", null),
  strong: react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("strong", null),
  em: react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("em", null),
  br: react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("br", null)
};
/**
 * Transform an object (tag name: {url}) into (tag name: anchor) where the url
 * is used as href, in order to render links inside a Fluent.Localized component.
 */

function convertLinks(links, sendClick, doNotAutoBlock, openNewWindow = false) {
  if (links) {
    return Object.keys(links).reduce((acc, linkTag) => {
      const {
        action
      } = links[linkTag]; // Setting the value to false will not include the attribute in the anchor

      const url = action ? false : Object(_template_utils__WEBPACK_IMPORTED_MODULE_3__["safeURI"])(links[linkTag].url);
      acc[linkTag] = // eslint was getting a false positive caused by the dynamic injection
      // of content.
      // eslint-disable-next-line jsx-a11y/anchor-has-content
      react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("a", {
        href: url,
        target: openNewWindow ? "_blank" : "",
        "data-metric": links[linkTag].metric,
        "data-action": action,
        "data-args": links[linkTag].args,
        "data-do_not_autoblock": doNotAutoBlock,
        onClick: sendClick
      });
      return acc;
    }, {});
  }

  return null;
}
/**
 * Message wrapper used to sanitize markup and render HTML.
 */

function RichText(props) {
  if (!_rich_text_strings__WEBPACK_IMPORTED_MODULE_2__["RICH_TEXT_KEYS"].includes(props.localization_id)) {
    throw new Error(`ASRouter: ${props.localization_id} is not a valid rich text property. If you want it to be processed, you need to add it to asrouter/rich-text-strings.js`);
  }

  return react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement(fluent_react__WEBPACK_IMPORTED_MODULE_0__["Localized"], _extends({
    id: props.localization_id
  }, ALLOWED_TAGS, props.customElements, convertLinks(props.links, props.sendClick, props.doNotAutoBlock, props.openNewWindow)), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("span", null, props.text));
}

/***/ }),
/* 17 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "safeURI", function() { return safeURI; });
function safeURI(url) {
  if (!url) {
    return "";
  }

  const {
    protocol
  } = new URL(url);
  const isAllowed = ["http:", "https:", "data:", "resource:", "chrome:"].includes(protocol);

  if (!isAllowed) {
    console.warn(`The protocol ${protocol} is not allowed for template URLs.`); // eslint-disable-line no-console
  }

  return isAllowed ? url : "";
}

/***/ }),
/* 18 */
/***/ (function(module) {

/***/ }),
/* 19 */
/***/ (function(module) {

/***/ }),
/* 20 */
/***/ (function(module) {

/***/ }),
/* 21 */
/***/ (function(module) {

/***/ }),
/* 22 */
/***/ (function(module) {

/***/ }),
/* 23 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 24 */
/***/ (function(module, exports) {

module.exports = ReactRedux;

/***/ }),
/* 25 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 26 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "SimpleHashRouter", function() { return SimpleHashRouter; });
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);

class SimpleHashRouter extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.onHashChange = this.onHashChange.bind(this);
    this.state = {
      hash: global.location.hash
    };
  }

  onHashChange() {
    this.setState({
      hash: global.location.hash
    });
  }

  componentWillMount() {
    global.addEventListener("hashchange", this.onHashChange);
  }

  componentWillUnmount() {
    global.removeEventListener("hashchange", this.onHashChange);
  }

  render() {
    const [, ...routes] = this.state.hash.split("-");
    return react__WEBPACK_IMPORTED_MODULE_0___default.a.cloneElement(this.props.children, {
      location: {
        hash: this.state.hash,
        routes
      }
    });
  }

}
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 27 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_ConfirmDialog", function() { return _ConfirmDialog; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ConfirmDialog", function() { return ConfirmDialog; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(24);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(react_redux__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_2__);



/**
 * ConfirmDialog component.
 * One primary action button, one cancel button.
 *
 * Content displayed is controlled by `data` prop the component receives.
 * Example:
 * data: {
 *   // Any sort of data needed to be passed around by actions.
 *   payload: site.url,
 *   // Primary button AlsoToMain action.
 *   action: "DELETE_HISTORY_URL",
 *   // Primary button USerEvent action.
 *   userEvent: "DELETE",
 *   // Array of locale ids to display.
 *   message_body: ["confirm_history_delete_p1", "confirm_history_delete_notice_p2"],
 *   // Text for primary button.
 *   confirm_button_string_id: "menu_action_delete"
 * },
 */

class _ConfirmDialog extends react__WEBPACK_IMPORTED_MODULE_2___default.a.PureComponent {
  constructor(props) {
    super(props);
    this._handleCancelBtn = this._handleCancelBtn.bind(this);
    this._handleConfirmBtn = this._handleConfirmBtn.bind(this);
  }

  _handleCancelBtn() {
    this.props.dispatch({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].DIALOG_CANCEL
    });
  }

  _handleConfirmBtn() {
    this.props.data.onConfirm.forEach(this.props.dispatch);
  }

  _renderModalMessage() {
    const message_body = this.props.data.body_string_id;

    if (!message_body) {
      return null;
    }

    return react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("span", null, message_body.map(msg => react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("p", {
      key: msg,
      "data-l10n-id": msg
    })));
  }

  render() {
    if (!this.props.visible) {
      return null;
    }

    return react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("div", {
      className: "confirmation-dialog"
    }, react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("div", {
      className: "modal-overlay",
      onClick: this._handleCancelBtn,
      role: "presentation"
    }), react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("div", {
      className: "modal"
    }, react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("section", {
      className: "modal-message"
    }, this.props.data.icon && react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("span", {
      className: `icon icon-spacer icon-${this.props.data.icon}`
    }), this._renderModalMessage()), react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("section", {
      className: "actions"
    }, react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("button", {
      onClick: this._handleCancelBtn,
      "data-l10n-id": this.props.data.cancel_button_string_id
    }), react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement("button", {
      className: "done",
      onClick: this._handleConfirmBtn,
      "data-l10n-id": this.props.data.confirm_button_string_id
    }))));
  }

}
const ConfirmDialog = Object(react_redux__WEBPACK_IMPORTED_MODULE_1__["connect"])(state => state.Dialog)(_ConfirmDialog);

/***/ }),
/* 28 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ContextMenu", function() { return ContextMenu; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ContextMenuItem", function() { return ContextMenuItem; });
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);

class ContextMenu extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.hideContext = this.hideContext.bind(this);
    this.onShow = this.onShow.bind(this);
    this.onClick = this.onClick.bind(this);
  }

  hideContext() {
    this.props.onUpdate(false);
  }

  onShow() {
    if (this.props.onShow) {
      this.props.onShow();
    }
  }

  componentDidMount() {
    this.onShow();
    setTimeout(() => {
      global.addEventListener("click", this.hideContext);
    }, 0);
  }

  componentWillUnmount() {
    global.removeEventListener("click", this.hideContext);
  }

  onClick(event) {
    // Eat all clicks on the context menu so they don't bubble up to window.
    // This prevents the context menu from closing when clicking disabled items
    // or the separators.
    event.stopPropagation();
  }

  render() {
    // Disabling focus on the menu span allows the first tab to focus on the first menu item instead of the wrapper.
    return (// eslint-disable-next-line jsx-a11y/interactive-supports-focus
      react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("span", {
        role: "menu",
        className: "context-menu",
        onClick: this.onClick,
        onKeyDown: this.onClick
      }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("ul", {
        className: "context-menu-list"
      }, this.props.options.map((option, i) => option.type === "separator" ? react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("li", {
        key: i,
        className: "separator"
      }) : option.type !== "empty" && react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(ContextMenuItem, {
        key: i,
        option: option,
        hideContext: this.hideContext,
        tabIndex: "0"
      }))))
    );
  }

}
class ContextMenuItem extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.onClick = this.onClick.bind(this);
    this.onKeyDown = this.onKeyDown.bind(this);
    this.focusFirst = this.focusFirst.bind(this);
  }

  onClick() {
    this.props.hideContext();
    this.props.option.onClick();
  }

  focusFirst(button) {
    if (button) {
      button.focus();
    }
  } // This selects the correct node based on the key pressed


  focusSibling(target, key) {
    const parent = target.parentNode;
    const closestSiblingSelector = key === "ArrowUp" ? "previousSibling" : "nextSibling";

    if (!parent[closestSiblingSelector]) {
      return;
    }

    if (parent[closestSiblingSelector].firstElementChild) {
      parent[closestSiblingSelector].firstElementChild.focus();
    } else {
      parent[closestSiblingSelector][closestSiblingSelector].firstElementChild.focus();
    }
  }

  onKeyDown(event) {
    const {
      option
    } = this.props;

    switch (event.key) {
      case "Tab":
        // tab goes down in context menu, shift + tab goes up in context menu
        // if we're on the last item, one more tab will close the context menu
        // similarly, if we're on the first item, one more shift + tab will close it
        if (event.shiftKey && option.first || !event.shiftKey && option.last) {
          this.props.hideContext();
        }

        break;

      case "ArrowUp":
      case "ArrowDown":
        event.preventDefault();
        this.focusSibling(event.target, event.key);
        break;

      case "Enter":
        this.props.hideContext();
        option.onClick();
        break;

      case "Escape":
        this.props.hideContext();
        break;
    }
  }

  render() {
    const {
      option
    } = this.props;
    return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("li", {
      role: "menuitem",
      className: "context-menu-item"
    }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("button", {
      className: option.disabled ? "disabled" : "",
      tabIndex: "0",
      onClick: this.onClick,
      onKeyDown: this.onKeyDown,
      ref: option.first ? this.focusFirst : null
    }, option.icon && react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("span", {
      className: `icon icon-spacer icon-${option.icon}`
    }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("span", {
      "data-l10n-id": option.string_id || option.id
    })));
  }

}
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 29 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 30 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "CollapsibleSection", function() { return CollapsibleSection; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var content_src_components_ErrorBoundary_ErrorBoundary__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(31);
/* harmony import */ var content_src_components_FluentOrText_FluentOrText__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(33);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_3__);
/* harmony import */ var content_src_components_SectionMenu_SectionMenu__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(34);
/* harmony import */ var content_src_lib_section_menu_options__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(35);






const VISIBLE = "visible";
const VISIBILITY_CHANGE_EVENT = "visibilitychange";
class CollapsibleSection extends react__WEBPACK_IMPORTED_MODULE_3___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.onBodyMount = this.onBodyMount.bind(this);
    this.onHeaderClick = this.onHeaderClick.bind(this);
    this.onKeyPress = this.onKeyPress.bind(this);
    this.onTransitionEnd = this.onTransitionEnd.bind(this);
    this.enableOrDisableAnimation = this.enableOrDisableAnimation.bind(this);
    this.onMenuButtonClick = this.onMenuButtonClick.bind(this);
    this.onMenuButtonMouseEnter = this.onMenuButtonMouseEnter.bind(this);
    this.onMenuButtonMouseLeave = this.onMenuButtonMouseLeave.bind(this);
    this.onMenuUpdate = this.onMenuUpdate.bind(this);
    this.state = {
      enableAnimation: true,
      isAnimating: false,
      menuButtonHover: false,
      showContextMenu: false
    };
    this.setContextMenuButtonRef = this.setContextMenuButtonRef.bind(this);
  }

  componentWillMount() {
    this.props.document.addEventListener(VISIBILITY_CHANGE_EVENT, this.enableOrDisableAnimation);
  }

  componentWillUpdate(nextProps) {
    // Check if we're about to go from expanded to collapsed
    if (!this.props.collapsed && nextProps.collapsed) {
      // This next line forces a layout flush of the section body, which has a
      // max-height style set, so that the upcoming collapse animation can
      // animate from that height to the collapsed height. Without this, the
      // update is coalesced and there's no animation from no-max-height to 0.
      this.sectionBody.scrollHeight; // eslint-disable-line no-unused-expressions
    }
  }

  setContextMenuButtonRef(element) {
    this.contextMenuButtonRef = element;
  }

  componentDidMount() {
    this.contextMenuButtonRef.addEventListener("mouseenter", this.onMenuButtonMouseEnter);
    this.contextMenuButtonRef.addEventListener("mouseleave", this.onMenuButtonMouseLeave);
  }

  componentWillUnmount() {
    this.props.document.removeEventListener(VISIBILITY_CHANGE_EVENT, this.enableOrDisableAnimation);
    this.contextMenuButtonRef.removeEventListener("mouseenter", this.onMenuButtonMouseEnter);
    this.contextMenuButtonRef.removeEventListener("mouseleave", this.onMenuButtonMouseLeave);
  }

  enableOrDisableAnimation() {
    // Only animate the collapse/expand for visible tabs.
    const visible = this.props.document.visibilityState === VISIBLE;

    if (this.state.enableAnimation !== visible) {
      this.setState({
        enableAnimation: visible
      });
    }
  }

  onBodyMount(node) {
    this.sectionBody = node;
  }

  onHeaderClick() {
    // If this.sectionBody is unset, it means that we're in some sort of error
    // state, probably displaying the error fallback, so we won't be able to
    // compute the height, and we don't want to persist the preference.
    // If props.collapsed is undefined handler shouldn't do anything.
    if (!this.sectionBody || this.props.collapsed === undefined) {
      return;
    } // Get the current height of the body so max-height transitions can work


    this.setState({
      isAnimating: true,
      maxHeight: `${this._getSectionBodyHeight()}px`
    });
    const {
      action
    } = content_src_lib_section_menu_options__WEBPACK_IMPORTED_MODULE_5__["SectionMenuOptions"].CheckCollapsed(this.props);
    this.props.dispatch(action);
  }

  onKeyPress(event) {
    if (event.key === "Enter" || event.key === " ") {
      this.onHeaderClick();
    }
  }

  _getSectionBodyHeight() {
    const div = this.sectionBody;

    if (div.style.display === "none") {
      // If the div isn't displayed, we can't get it's height. So we display it
      // to get the height (it doesn't show up because max-height is set to 0px
      // in CSS). We don't undo this because we are about to expand the section.
      div.style.display = "block";
    }

    return div.scrollHeight;
  }

  onTransitionEnd(event) {
    // Only update the animating state for our own transition (not a child's)
    if (event.target === event.currentTarget) {
      this.setState({
        isAnimating: false
      });
    }
  }

  renderIcon() {
    const {
      icon
    } = this.props;

    if (icon && icon.startsWith("moz-extension://")) {
      return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
        className: "icon icon-small-spacer",
        style: {
          backgroundImage: `url('${icon}')`
        }
      });
    }

    return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      className: `icon icon-small-spacer icon-${icon || "webextension"}`
    });
  }

  onMenuButtonClick(event) {
    event.preventDefault();
    this.setState({
      showContextMenu: true
    });
  }

  onMenuButtonMouseEnter() {
    this.setState({
      menuButtonHover: true
    });
  }

  onMenuButtonMouseLeave() {
    this.setState({
      menuButtonHover: false
    });
  }

  onMenuUpdate(showContextMenu) {
    this.setState({
      showContextMenu
    });
  }

  render() {
    const isCollapsible = this.props.collapsed !== undefined;
    const {
      enableAnimation,
      isAnimating,
      maxHeight,
      menuButtonHover,
      showContextMenu
    } = this.state;
    const {
      id,
      eventSource,
      collapsed,
      learnMore,
      title,
      extraMenuOptions,
      showPrefName,
      privacyNoticeURL,
      dispatch,
      isFixed,
      isFirst,
      isLast,
      isWebExtension
    } = this.props;
    const active = menuButtonHover || showContextMenu;
    let bodyStyle;

    if (isAnimating && !collapsed) {
      bodyStyle = {
        maxHeight
      };
    } else if (!isAnimating && collapsed) {
      bodyStyle = {
        display: "none"
      };
    }

    return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("section", {
      className: `collapsible-section ${this.props.className}${enableAnimation ? " animation-enabled" : ""}${collapsed ? " collapsed" : ""}${active ? " active" : ""}`,
      "aria-expanded": !collapsed // Note: data-section-id is used for web extension api tests in mozilla central
      ,
      "data-section-id": id
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "section-top-bar"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("h3", {
      className: "section-title"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      className: "click-target-container"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      className: "click-target",
      role: "button",
      tabIndex: "0",
      onKeyPress: this.onKeyPress,
      onClick: this.onHeaderClick
    }, this.renderIcon(), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(content_src_components_FluentOrText_FluentOrText__WEBPACK_IMPORTED_MODULE_2__["FluentOrText"], {
      message: title
    })), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      className: "click-target",
      role: "button",
      tabIndex: "0",
      onKeyPress: this.onKeyPress,
      onClick: this.onHeaderClick
    }, isCollapsible && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      className: `collapsible-arrow icon ${collapsed ? "icon-arrowhead-forward-small" : "icon-arrowhead-down-small"}`
    })), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      className: "learn-more-link-wrapper"
    }, learnMore && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      className: "learn-more-link"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(content_src_components_FluentOrText_FluentOrText__WEBPACK_IMPORTED_MODULE_2__["FluentOrText"], {
      message: learnMore.link.message
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("a", {
      href: learnMore.link.href
    })))))), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", null, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("button", {
      "aria-haspopup": "true",
      className: "context-menu-button icon",
      "data-l10n-id": "newtab-menu-section-tooltip",
      onClick: this.onMenuButtonClick,
      ref: this.setContextMenuButtonRef
    }), showContextMenu && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(content_src_components_SectionMenu_SectionMenu__WEBPACK_IMPORTED_MODULE_4__["SectionMenu"], {
      id: id,
      extraOptions: extraMenuOptions,
      eventSource: eventSource,
      showPrefName: showPrefName,
      privacyNoticeURL: privacyNoticeURL,
      collapsed: collapsed,
      onUpdate: this.onMenuUpdate,
      isFixed: isFixed,
      isFirst: isFirst,
      isLast: isLast,
      dispatch: dispatch,
      isWebExtension: isWebExtension
    }))), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(content_src_components_ErrorBoundary_ErrorBoundary__WEBPACK_IMPORTED_MODULE_1__["ErrorBoundary"], {
      className: "section-body-fallback"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: `section-body${isAnimating ? " animating" : ""}`,
      onTransitionEnd: this.onTransitionEnd,
      ref: this.onBodyMount,
      style: bodyStyle
    }, this.props.children)));
  }

}
CollapsibleSection.defaultProps = {
  document: global.document || {
    addEventListener: () => {},
    removeEventListener: () => {},
    visibilityState: "hidden"
  },
  Prefs: {
    values: {}
  }
};
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 31 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ErrorBoundaryFallback", function() { return ErrorBoundaryFallback; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ErrorBoundary", function() { return ErrorBoundary; });
/* harmony import */ var content_src_components_A11yLinkButton_A11yLinkButton__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(32);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_1__);


class ErrorBoundaryFallback extends react__WEBPACK_IMPORTED_MODULE_1___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.windowObj = this.props.windowObj || window;
    this.onClick = this.onClick.bind(this);
  }
  /**
   * Since we only get here if part of the page has crashed, do a
   * forced reload to give us the best chance at recovering.
   */


  onClick() {
    this.windowObj.location.reload(true);
  }

  render() {
    const defaultClass = "as-error-fallback";
    let className;

    if ("className" in this.props) {
      className = `${this.props.className} ${defaultClass}`;
    } else {
      className = defaultClass;
    } // "A11yLinkButton" to force normal link styling stuff (eg cursor on hover)


    return react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: className
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      "data-l10n-id": "newtab-error-fallback-info"
    }), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("span", null, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement(content_src_components_A11yLinkButton_A11yLinkButton__WEBPACK_IMPORTED_MODULE_0__["A11yLinkButton"], {
      className: "reload-button",
      onClick: this.onClick,
      "data-l10n-id": "newtab-error-fallback-refresh-link"
    })));
  }

}
ErrorBoundaryFallback.defaultProps = {
  className: "as-error-fallback"
};
class ErrorBoundary extends react__WEBPACK_IMPORTED_MODULE_1___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.state = {
      hasError: false
    };
  }

  componentDidCatch(error, info) {
    this.setState({
      hasError: true
    });
  }

  render() {
    if (!this.state.hasError) {
      return this.props.children;
    }

    return react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement(this.props.FallbackComponent, {
      className: this.props.className
    });
  }

}
ErrorBoundary.defaultProps = {
  FallbackComponent: ErrorBoundaryFallback
};

/***/ }),
/* 32 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "A11yLinkButton", function() { return A11yLinkButton; });
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);
function _extends() { _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; }; return _extends.apply(this, arguments); }


function A11yLinkButton(props) {
  // function for merging classes, if necessary
  let className = "a11y-link-button";

  if (props.className) {
    className += ` ${props.className}`;
  }

  return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("button", _extends({
    type: "button"
  }, props, {
    className: className
  }), props.children);
}

/***/ }),
/* 33 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "FluentOrText", function() { return FluentOrText; });
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);

/**
 * Set text on a child element/component depending on if the message is already
 * translated plain text or a fluent id with optional args.
 */

class FluentOrText extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  render() {
    // Ensure we have a single child to attach attributes
    const {
      children,
      message
    } = this.props;
    const child = children ? react__WEBPACK_IMPORTED_MODULE_0___default.a.Children.only(children) : react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("span", null); // For a string message, just use it as the child's text

    let grandChildren = message;
    let extraProps; // Convert a message object to set desired fluent-dom attributes

    if (typeof message === "object") {
      const args = message.args || message.values;
      extraProps = {
        "data-l10n-args": args && JSON.stringify(args),
        "data-l10n-id": message.id || message.string_id
      }; // Use original children potentially with data-l10n-name attributes

      grandChildren = child.props.children;
    } // Add the message to the child via fluent attributes or text node


    return react__WEBPACK_IMPORTED_MODULE_0___default.a.cloneElement(child, extraProps, grandChildren);
  }

}

/***/ }),
/* 34 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_SectionMenu", function() { return _SectionMenu; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "SectionMenu", function() { return SectionMenu; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var content_src_components_ContextMenu_ContextMenu__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(28);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_2__);
/* harmony import */ var content_src_lib_section_menu_options__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(35);




const DEFAULT_SECTION_MENU_OPTIONS = ["MoveUp", "MoveDown", "Separator", "RemoveSection", "CheckCollapsed", "Separator", "ManageSection"];
const WEBEXT_SECTION_MENU_OPTIONS = ["MoveUp", "MoveDown", "Separator", "CheckCollapsed", "Separator", "ManageWebExtension"];
class _SectionMenu extends react__WEBPACK_IMPORTED_MODULE_2___default.a.PureComponent {
  getOptions() {
    const {
      props
    } = this;
    const propOptions = props.isWebExtension ? [...WEBEXT_SECTION_MENU_OPTIONS] : [...DEFAULT_SECTION_MENU_OPTIONS]; // Remove the move related options if the section is fixed

    if (props.isFixed) {
      propOptions.splice(propOptions.indexOf("MoveUp"), 3);
    } // Prepend custom options and a separator


    if (props.extraOptions) {
      propOptions.splice(0, 0, ...props.extraOptions, "Separator");
    } // Insert privacy notice before the last option ("ManageSection")


    if (props.privacyNoticeURL) {
      propOptions.splice(-1, 0, "PrivacyNotice");
    }

    const options = propOptions.map(o => content_src_lib_section_menu_options__WEBPACK_IMPORTED_MODULE_3__["SectionMenuOptions"][o](props)).map(option => {
      const {
        action,
        id,
        type
      } = option;

      if (!type && id) {
        option.onClick = () => {
          props.dispatch(action);
        };
      }

      return option;
    }); // This is for accessibility to support making each item tabbable.
    // We want to know which item is the first and which item
    // is the last, so we can close the context menu accordingly.

    options[0].first = true;
    options[options.length - 1].last = true;
    return options;
  }

  render() {
    return react__WEBPACK_IMPORTED_MODULE_2___default.a.createElement(content_src_components_ContextMenu_ContextMenu__WEBPACK_IMPORTED_MODULE_1__["ContextMenu"], {
      onUpdate: this.props.onUpdate,
      options: this.getOptions()
    });
  }

}
const SectionMenu = _SectionMenu;

/***/ }),
/* 35 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "SectionMenuOptions", function() { return SectionMenuOptions; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);

/**
 * List of functions that return items that can be included as menu options in a
 * SectionMenu. All functions take the section as the only parameter.
 */

const SectionMenuOptions = {
  Separator: () => ({
    type: "separator"
  }),
  MoveUp: section => ({
    id: "newtab-section-menu-move-up",
    icon: "arrowhead-up",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].SECTION_MOVE,
      data: {
        id: section.id,
        direction: -1
      }
    }),
    disabled: !!section.isFirst
  }),
  MoveDown: section => ({
    id: "newtab-section-menu-move-down",
    icon: "arrowhead-down",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].SECTION_MOVE,
      data: {
        id: section.id,
        direction: +1
      }
    }),
    disabled: !!section.isLast
  }),
  RemoveSection: section => ({
    id: "newtab-section-menu-remove-section",
    icon: "dismiss",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].SetPref(section.showPrefName, false),
  }),
  CollapseSection: section => ({
    id: "newtab-section-menu-collapse-section",
    icon: "minimize",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].UPDATE_SECTION_PREFS,
      data: {
        id: section.id,
        value: {
          collapsed: true
        }
      }
    }),
  }),
  ExpandSection: section => ({
    id: "newtab-section-menu-expand-section",
    icon: "maximize",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].UPDATE_SECTION_PREFS,
      data: {
        id: section.id,
        value: {
          collapsed: false
        }
      }
    }),
  }),
  ManageSection: section => ({
    id: "newtab-section-menu-manage-section",
    icon: "settings",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].SETTINGS_OPEN
    }),
  }),
  ManageWebExtension: section => ({
    id: "newtab-section-menu-manage-webext",
    icon: "settings",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].OPEN_WEBEXT_SETTINGS,
      data: section.id
    })
  }),
  AddTopSite: section => ({
    id: "newtab-section-menu-add-topsite",
    icon: "add",
    action: {
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].TOP_SITES_EDIT,
      data: {
        index: -1
      }
    },
  }),
  AddSearchShortcut: section => ({
    id: "newtab-section-menu-add-search-engine",
    icon: "search",
    action: {
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].TOP_SITES_OPEN_SEARCH_SHORTCUTS_MODAL
    },
  }),
  PrivacyNotice: section => ({
    id: "newtab-section-menu-privacy-notice",
    icon: "info",
    action: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].OPEN_LINK,
      data: {
        url: section.privacyNoticeURL
      }
    }),
  }),
  CheckCollapsed: section => section.collapsed ? SectionMenuOptions.ExpandSection(section) : SectionMenuOptions.CollapseSection(section)
};

/***/ }),
/* 36 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Section", function() { return Section; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "SectionIntl", function() { return SectionIntl; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_Sections", function() { return _Sections; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Sections", function() { return Sections; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var content_src_components_Card_Card__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(54);
/* harmony import */ var content_src_components_CollapsibleSection_CollapsibleSection__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(30);
/* harmony import */ var content_src_components_ComponentPerfTimer_ComponentPerfTimer__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(38);
/* harmony import */ var content_src_components_FluentOrText_FluentOrText__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(33);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(24);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_5___default = /*#__PURE__*/__webpack_require__.n(react_redux__WEBPACK_IMPORTED_MODULE_5__);
/* harmony import */ var content_src_components_MoreRecommendations_MoreRecommendations__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(40);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_8___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_8__);
/* harmony import */ var content_src_components_Topics_Topics__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(42);
/* harmony import */ var content_src_components_TopSites_TopSites__WEBPACK_IMPORTED_MODULE_10__ = __webpack_require__(43);
function _extends() { _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; }; return _extends.apply(this, arguments); }












const VISIBLE = "visible";
const VISIBILITY_CHANGE_EVENT = "visibilitychange";
const CARDS_PER_ROW_DEFAULT = 3;
const CARDS_PER_ROW_COMPACT_WIDE = 4;
class Section extends react__WEBPACK_IMPORTED_MODULE_8___default.a.PureComponent {
  get numRows() {
    const {
      rowsPref,
      maxRows,
      Prefs
    } = this.props;
    return rowsPref ? Prefs.values[rowsPref] : maxRows;
  }

  componentWillMount() {
    this.sendNewTabRehydrated(this.props.initialized);
  }

  componentWillUpdate(nextProps) {
    this.sendNewTabRehydrated(nextProps.initialized);
  }

  componentWillUnmount() {
    if (this._onVisibilityChange) {
      this.props.document.removeEventListener(VISIBILITY_CHANGE_EVENT, this._onVisibilityChange);
    }
  }

  sendNewTabRehydrated(initialized) {
    if (initialized && !this.renderNotified) {
      this.props.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].AlsoToMain({
        type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].NEW_TAB_REHYDRATED,
        data: {}
      }));
      this.renderNotified = true;
    }
  }

  render() {
    const {
      id,
      eventSource,
      title,
      icon,
      rows,
      topics,
      emptyState,
      dispatch,
      compactCards,
      read_more_endpoint,
      contextMenuOptions,
      initialized,
      learnMore,
      pref,
      privacyNoticeURL,
      isFirst,
      isLast
    } = this.props;
    const maxCardsPerRow = compactCards ? CARDS_PER_ROW_COMPACT_WIDE : CARDS_PER_ROW_DEFAULT;
    const {
      numRows
    } = this;
    const maxCards = maxCardsPerRow * numRows;
    const maxCardsOnNarrow = CARDS_PER_ROW_DEFAULT * numRows;

    const hasTopics = topics && topics.length > 0;

    const shouldShowReadMore = read_more_endpoint && hasTopics;
    const realRows = rows.slice(0, maxCards); // The empty state should only be shown after we have initialized and there is no content.
    // Otherwise, we should show placeholders.

    const shouldShowEmptyState = initialized && !rows.length;
    const cards = [];

    if (!shouldShowEmptyState) {
      for (let i = 0; i < maxCards; i++) {
        const link = realRows[i]; // On narrow viewports, we only show 3 cards per row. We'll mark the rest as
        // .hide-for-narrow to hide in CSS via @media query.

        const className = i >= maxCardsOnNarrow ? "hide-for-narrow" : "";
        let usePlaceholder = !link; // If we are in the third card and waiting for spoc,
        // use the placeholder.

        cards.push(!usePlaceholder ? react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(content_src_components_Card_Card__WEBPACK_IMPORTED_MODULE_1__["Card"], {
          key: i,
          index: i,
          className: className,
          dispatch: dispatch,
          link: link,
          contextMenuOptions: contextMenuOptions,
          eventSource: eventSource,
          isWebExtension: this.props.isWebExtension
        }) : react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(content_src_components_Card_Card__WEBPACK_IMPORTED_MODULE_1__["PlaceholderCard"], {
          key: i,
          className: className
        }));
      }
    }

    const sectionClassName = ["section", compactCards ? "compact-cards" : "normal-cards"].join(" "); // <Section> <-- React component
    // <section> <-- HTML5 element

    return react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(content_src_components_ComponentPerfTimer_ComponentPerfTimer__WEBPACK_IMPORTED_MODULE_3__["ComponentPerfTimer"], this.props, react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(content_src_components_CollapsibleSection_CollapsibleSection__WEBPACK_IMPORTED_MODULE_2__["CollapsibleSection"], {
      className: sectionClassName,
      icon: icon,
      title: title,
      id: id,
      eventSource: eventSource,
      collapsed: this.props.pref.collapsed,
      showPrefName: pref && pref.feed || id,
      privacyNoticeURL: privacyNoticeURL,
      Prefs: this.props.Prefs,
      isFixed: this.props.isFixed,
      isFirst: isFirst,
      isLast: isLast,
      learnMore: learnMore,
      dispatch: this.props.dispatch,
      isWebExtension: this.props.isWebExtension
    }, !shouldShowEmptyState && react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("ul", {
      className: "section-list",
      style: {
        padding: 0
      }
    }, cards), shouldShowEmptyState && react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("div", {
      className: "section-empty-state"
    }, react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("div", {
      className: "empty-state"
    }, emptyState.icon && emptyState.icon.startsWith("moz-extension://") ? react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("span", {
      className: "empty-state-icon icon",
      style: {
        "background-image": `url('${emptyState.icon}')`
      }
    }) : react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("span", {
      className: `empty-state-icon icon icon-${emptyState.icon}`
    }), react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(content_src_components_FluentOrText_FluentOrText__WEBPACK_IMPORTED_MODULE_4__["FluentOrText"], {
      message: emptyState.message
    }, react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("p", {
      className: "empty-state-message"
    })))), id === "topstories" && react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("div", {
      className: "top-stories-bottom-container"
    }, shouldShowTopics && react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("div", {
      className: "wrapper-topics"
    }, react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(content_src_components_Topics_Topics__WEBPACK_IMPORTED_MODULE_9__["Topics"], {
      topics: this.props.topics
    })))));
  }

}
Section.defaultProps = {
  document: global.document,
  rows: [],
  emptyState: {},
  pref: {},
  title: ""
};
const SectionIntl = Object(react_redux__WEBPACK_IMPORTED_MODULE_5__["connect"])(state => ({
  Prefs: state.Prefs
}))(Section);
class _Sections extends react__WEBPACK_IMPORTED_MODULE_8___default.a.PureComponent {
  renderSections() {
    const sections = [];
    const enabledSections = this.props.Sections.filter(section => section.enabled);
    const {
      sectionOrder,
      "feeds.topsites": showTopSites
    } = this.props.Prefs.values; // Enabled sections doesn't include Top Sites, so we add it if enabled.

    const expectedCount = enabledSections.length + ~~showTopSites;

    for (const sectionId of sectionOrder.split(",")) {
      const commonProps = {
        key: sectionId,
        isFirst: sections.length === 0,
        isLast: sections.length === expectedCount - 1
      };

      if (sectionId === "topsites" && showTopSites) {
        sections.push(react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(content_src_components_TopSites_TopSites__WEBPACK_IMPORTED_MODULE_10__["TopSites"], commonProps));
      } else {
        const section = enabledSections.find(s => s.id === sectionId);

        if (section) {
          sections.push(react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement(SectionIntl, _extends({}, section, commonProps)));
        }
      }
    }

    return sections;
  }

  render() {
    return react__WEBPACK_IMPORTED_MODULE_8___default.a.createElement("div", {
      className: "sections-list"
    }, this.renderSections());
  }

}
const Sections = Object(react_redux__WEBPACK_IMPORTED_MODULE_5__["connect"])(state => ({
  Sections: state.Sections,
  Prefs: state.Prefs
}))(_Sections);
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 37 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ScreenshotUtils", function() { return ScreenshotUtils; });
/**
 * List of helper functions for screenshot-based images.
 *
 * There are two kinds of images:
 * 1. Remote Image: This is the image from the main process and it refers to
 *    the image in the React props. This can either be an object with the `data`
 *    and `path` properties, if it is a blob, or a string, if it is a normal image.
 * 2. Local Image: This is the image object in the content process and it refers
 *    to the image *object* in the React component's state. All local image
 *    objects have the `url` property, and an additional property `path`, if they
 *    are blobs.
 */
const ScreenshotUtils = {
  isBlob(isLocal, image) {
    return !!(image && image.path && (!isLocal && image.data || isLocal && image.url));
  },

  // This should always be called with a remote image and not a local image.
  createLocalImageObject(remoteImage) {
    if (!remoteImage) {
      return null;
    }

    if (this.isBlob(false, remoteImage)) {
      return {
        url: global.URL.createObjectURL(remoteImage.data),
        path: remoteImage.path
      };
    }

    return {
      url: remoteImage
    };
  },

  // Revokes the object URL of the image if the local image is a blob.
  // This should always be called with a local image and not a remote image.
  maybeRevokeBlobObjectURL(localImage) {
    if (this.isBlob(true, localImage)) {
      global.URL.revokeObjectURL(localImage.url);
    }
  },

  // Checks if remoteImage and localImage are the same.
  isRemoteImageLocal(localImage, remoteImage) {
    // Both remoteImage and localImage are present.
    if (remoteImage && localImage) {
      return this.isBlob(false, remoteImage) ? localImage.path === remoteImage.path : localImage.url === remoteImage;
    } // This will only handle the remaining three possible outcomes.
    // (i.e. everything except when both image and localImage are present)


    return !remoteImage && !localImage;
  }

};
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 38 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "ComponentPerfTimer", function() { return ComponentPerfTimer; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var common_PerfService_jsm__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(39);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_2__);


 // Currently record only a fixed set of sections. This will prevent data
// from custom sections from showing up or from topstories.

const RECORDED_SECTIONS = ["highlights", "topsites"];
class ComponentPerfTimer extends react__WEBPACK_IMPORTED_MODULE_2___default.a.Component {
  constructor(props) {
    super(props); // Just for test dependency injection:

    this.perfSvc = this.props.perfSvc || common_PerfService_jsm__WEBPACK_IMPORTED_MODULE_1__["perfService"];
    this._sendPaintedEvent = this._sendPaintedEvent.bind(this);
    this._reportMissingData = false;
    this._timestampHandled = false;
    this._recordedFirstRender = false;
  }

  componentDidMount() {
    if (!RECORDED_SECTIONS.includes(this.props.id)) {
      return;
    }

    this._maybeSendPaintedEvent();
  }

  componentDidUpdate() {
    if (!RECORDED_SECTIONS.includes(this.props.id)) {
      return;
    }

    this._maybeSendPaintedEvent();
  }
  /**
   * Call the given callback after the upcoming frame paints.
   *
   * @note Both setTimeout and requestAnimationFrame are throttled when the page
   * is hidden, so this callback may get called up to a second or so after the
   * requestAnimationFrame "paint" for hidden tabs.
   *
   * Newtabs hidden while loading will presumably be fairly rare (other than
   * preloaded tabs, which we will be filtering out on the server side), so such
   * cases should get lost in the noise.
   *
   * If we decide that it's important to find out when something that's hidden
   * has "painted", however, another option is to post a message to this window.
   * That should happen even faster than setTimeout, and, at least as of this
   * writing, it's not throttled in hidden windows in Firefox.
   *
   * @param {Function} callback
   *
   * @returns void
   */


  _afterFramePaint(callback) {
    requestAnimationFrame(() => setTimeout(callback, 0));
  }

  _maybeSendPaintedEvent() {
    // If we've already handled a timestamp, don't do it again.
    if (this._timestampHandled || !this.props.initialized) {
      return;
    } // And if we haven't, we're doing so now, so remember that. Even if
    // something goes wrong in the callback, we can't try again, as we'd be
    // sending back the wrong data, and we have to do it here, so that other
    // calls to this method while waiting for the next frame won't also try to
    // handle it.


    this._timestampHandled = true;

    this._afterFramePaint(this._sendPaintedEvent);
  }
  /**
   * Triggered by call to render. Only first call goes through due to
   * `_recordedFirstRender`.
   */


  _ensureFirstRenderTsRecorded() {
    // Used as t0 for recording how long component took to initialize.
    if (!this._recordedFirstRender) {
      this._recordedFirstRender = true; // topsites_first_render_ts, highlights_first_render_ts.

      const key = `${this.props.id}_first_render_ts`;
      this.perfSvc.mark(key);
    }
  }

  _sendPaintedEvent() {
    // Record first_painted event but only send if topsites.
    if (this.props.id !== "topsites") {
      return;
    } // topsites_first_painted_ts.


    const key = `${this.props.id}_first_painted_ts`;
    this.perfSvc.mark(key);

    try {
      const data = {};
      data[key] = this.perfSvc.getMostRecentAbsMarkStartByName(key);
      this.props.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
        type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].SAVE_SESSION_PERF_DATA,
        data
      }));
    } catch (ex) {// If this failed, it's likely because the `privacy.resistFingerprinting`
      // pref is true.  We should at least not blow up, and should continue
      // to set this._timestampHandled to avoid going through this again.
    }
  }

  render() {
    if (RECORDED_SECTIONS.includes(this.props.id)) {
      this._ensureFirstRenderTsRecorded();
    }

    return this.props.children;
  }

}

/***/ }),
/* 39 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_PerfService", function() { return _PerfService; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "perfService", function() { return perfService; });


if (typeof ChromeUtils !== "undefined") {
  // Use a var here instead of let outside to avoid creating a locally scoped
  // variable that hides the global, which we modify for testing.
  // eslint-disable-next-line no-var, vars-on-top
  var {
    Services
  } = ChromeUtils.import("resource://gre/modules/Services.jsm");
}

let usablePerfObj;
/* istanbul ignore else */
// eslint-disable-next-line block-scoped-var

if (typeof Services !== "undefined") {
  // Borrow the high-resolution timer from the hidden window....
  // eslint-disable-next-line block-scoped-var
  usablePerfObj = Services.appShell.hiddenDOMWindow.performance;
} else {
  // we must be running in content space
  // eslint-disable-next-line no-undef
  usablePerfObj = performance;
}

function _PerfService(options) {
  // For testing, so that we can use a fake Window.performance object with
  // known state.
  if (options && options.performanceObj) {
    this._perf = options.performanceObj;
  } else {
    this._perf = usablePerfObj;
  }
}
_PerfService.prototype = {
  /**
   * Calls the underlying mark() method on the appropriate Window.performance
   * object to add a mark with the given name to the appropriate performance
   * timeline.
   *
   * @param  {String} name  the name to give the current mark
   * @return {void}
   */
  mark: function mark(str) {
    this._perf.mark(str);
  },

  /**
   * Calls the underlying getEntriesByName on the appropriate Window.performance
   * object.
   *
   * @param  {String} name
   * @param  {String} type eg "mark"
   * @return {Array}       Performance* objects
   */
  getEntriesByName: function getEntriesByName(name, type) {
    return this._perf.getEntriesByName(name, type);
  },

  /**
   * The timeOrigin property from the appropriate performance object.
   * Used to ensure that timestamps from the add-on code and the content code
   * are comparable.
   *
   * @note If this is called from a context without a window
   * (eg a JSM in chrome), it will return the timeOrigin of the XUL hidden
   * window, which appears to be the first created window (and thus
   * timeOrigin) in the browser.  Note also, however, there is also a private
   * hidden window, presumably for private browsing, which appears to be
   * created dynamically later.  Exactly how/when that shows up needs to be
   * investigated.
   *
   * @return {Number} A double of milliseconds with a precision of 0.5us.
   */
  get timeOrigin() {
    return this._perf.timeOrigin;
  },

  /**
   * Returns the "absolute" version of performance.now(), i.e. one that
   * should ([bug 1401406](https://bugzilla.mozilla.org/show_bug.cgi?id=1401406)
   * be comparable across both chrome and content.
   *
   * @return {Number}
   */
  absNow: function absNow() {
    return this.timeOrigin + this._perf.now();
  },

  /**
   * This returns the absolute startTime from the most recent performance.mark()
   * with the given name.
   *
   * @param  {String} name  the name to lookup the start time for
   *
   * @return {Number}       the returned start time, as a DOMHighResTimeStamp
   *
   * @throws {Error}        "No Marks with the name ..." if none are available
   *
   * @note Always surround calls to this by try/catch.  Otherwise your code
   * may fail when the `privacy.resistFingerprinting` pref is true.  When
   * this pref is set, all attempts to get marks will likely fail, which will
   * cause this method to throw.
   *
   * See [bug 1369303](https://bugzilla.mozilla.org/show_bug.cgi?id=1369303)
   * for more info.
   */
  getMostRecentAbsMarkStartByName(name) {
    let entries = this.getEntriesByName(name, "mark");

    if (!entries.length) {
      throw new Error(`No marks with the name ${name}`);
    }

    let mostRecentEntry = entries[entries.length - 1];
    return this._perf.timeOrigin + mostRecentEntry.startTime;
  }

};
var perfService = new _PerfService();

/***/ }),
/* 40 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 41 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 42 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Topic", function() { return Topic; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Topics", function() { return Topics; });
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_0__);

class Topic extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  render() {
    const {
      url,
      name
    } = this.props;
    return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("li", null, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("a", {
      key: name,
      href: url
    }, name));
  }

}
class Topics extends react__WEBPACK_IMPORTED_MODULE_0___default.a.PureComponent {
  render() {
    const {
      topics
    } = this.props;
    return react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("span", {
      className: "topics"
    }, react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("span", {
      "data-l10n-id": "newtab-pocket-read-more"
    }), react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement("ul", null, topics && topics.map(t => react__WEBPACK_IMPORTED_MODULE_0___default.a.createElement(Topic, {
      key: t.name,
      url: t.url,
      name: t.name
    }))));
  }

}

/***/ }),
/* 43 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_TopSites", function() { return _TopSites; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TopSites", function() { return TopSites; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(44);
/* harmony import */ var content_src_components_CollapsibleSection_CollapsibleSection__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(30);
/* harmony import */ var content_src_components_ComponentPerfTimer_ComponentPerfTimer__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(38);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(24);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_4___default = /*#__PURE__*/__webpack_require__.n(react_redux__WEBPACK_IMPORTED_MODULE_4__);
/* harmony import */ var _asrouter_components_ModalOverlay_ModalOverlay__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(13);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_6___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_6__);
/* harmony import */ var _SearchShortcutsForm__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(45);
/* harmony import */ var common_Reducers_jsm__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(53);
/* harmony import */ var _TopSiteForm__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(55);
/* harmony import */ var _TopSite__WEBPACK_IMPORTED_MODULE_10__ = __webpack_require__(46);
function _extends() { _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; }; return _extends.apply(this, arguments); }













function topSiteIconType(link) {
  if (link.customScreenshotURL) {
    return "custom_screenshot";
  }

  if (link.tippyTopIcon || link.faviconRef === "tippytop") {
    return "tippytop";
  }

  if (link.faviconSize >= _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["MIN_RICH_FAVICON_SIZE"]) {
    return "rich_icon";
  }

  if (link.screenshot && link.faviconSize >= _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["MIN_CORNER_FAVICON_SIZE"]) {
    return "screenshot_with_icon";
  }

  if (link.screenshot) {
    return "screenshot";
  }

  return "no_image";
}
/**
 * Iterates through TopSites and counts types of images.
 * @param acc Accumulator for reducer.
 * @param topsite Entry in TopSites.
 */


function countTopSitesIconsTypes(topSites) {
  const countTopSitesTypes = (acc, link) => {
    acc[topSiteIconType(link)]++;
    return acc;
  };

  return topSites.reduce(countTopSitesTypes, {
    "custom_screenshot": 0,
    "screenshot_with_icon": 0,
    "screenshot": 0,
    "tippytop": 0,
    "rich_icon": 0,
    "no_image": 0
  });
}

class _TopSites extends react__WEBPACK_IMPORTED_MODULE_6___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.onEditFormClose = this.onEditFormClose.bind(this);
    this.onSearchShortcutsFormClose = this.onSearchShortcutsFormClose.bind(this);
  }

  /**
   * Return the TopSites that are visible based on prefs and window width.
   */


  _getVisibleTopSites() {
    // We hide 2 sites per row when not in the wide layout.
    let sitesPerRow = common_Reducers_jsm__WEBPACK_IMPORTED_MODULE_8__["TOP_SITES_MAX_SITES_PER_ROW"]; // $break-point-widest = 1072px (from _variables.scss)

    if (!global.matchMedia(`(min-width: 1072px)`).matches) {
      sitesPerRow -= 2;
    }

    return this.props.TopSites.rows.slice(0, this.props.TopSitesRows * sitesPerRow);
  }

  onEditFormClose() {
    this.props.dispatch({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].TOP_SITES_CANCEL_EDIT
    });
  }

  onSearchShortcutsFormClose() {
    this.props.dispatch({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].TOP_SITES_CLOSE_SEARCH_SHORTCUTS_MODAL
    });
  }

  render() {
    const {
      props
    } = this;
    const {
      editForm,
      showSearchShortcutsForm
    } = props.TopSites;
    const extraMenuOptions = ["AddTopSite"];

    if (props.Prefs.values["improvesearch.topSiteSearchShortcuts"]) {
      extraMenuOptions.push("AddSearchShortcut");
    }

    return react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement(content_src_components_ComponentPerfTimer_ComponentPerfTimer__WEBPACK_IMPORTED_MODULE_3__["ComponentPerfTimer"], {
      id: "topsites",
      initialized: props.TopSites.initialized,
      dispatch: props.dispatch
    }, react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement(content_src_components_CollapsibleSection_CollapsibleSection__WEBPACK_IMPORTED_MODULE_2__["CollapsibleSection"], {
      className: "top-sites",
      icon: "topsites",
      id: "topsites",
      title: this.props.title || {
        id: "newtab-section-header-topsites"
      },
      extraMenuOptions: extraMenuOptions,
      showPrefName: "feeds.topsites",
      eventSource: _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["TOP_SITES_SOURCE"],
      collapsed: props.TopSites.pref ? props.TopSites.pref.collapsed : undefined,
      isFixed: props.isFixed,
      isFirst: props.isFirst,
      isLast: props.isLast,
      dispatch: props.dispatch
    }, react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement(_TopSite__WEBPACK_IMPORTED_MODULE_10__["TopSiteList"], {
      TopSites: props.TopSites,
      TopSitesRows: props.TopSitesRows,
      dispatch: props.dispatch,
      topSiteIconType: topSiteIconType
    }), react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement("div", {
      className: "edit-topsites-wrapper"
    }, editForm && react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement("div", {
      className: "edit-topsites"
    }, react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement(_asrouter_components_ModalOverlay_ModalOverlay__WEBPACK_IMPORTED_MODULE_5__["ModalOverlayWrapper"], {
      unstyled: true,
      onClose: this.onEditFormClose,
      innerClassName: "modal"
    }, react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement(_TopSiteForm__WEBPACK_IMPORTED_MODULE_9__["TopSiteForm"], _extends({
      site: props.TopSites.rows[editForm.index],
      onClose: this.onEditFormClose,
      dispatch: this.props.dispatch
    }, editForm)))), showSearchShortcutsForm && react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement("div", {
      className: "edit-search-shortcuts"
    }, react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement(_asrouter_components_ModalOverlay_ModalOverlay__WEBPACK_IMPORTED_MODULE_5__["ModalOverlayWrapper"], {
      unstyled: true,
      onClose: this.onSearchShortcutsFormClose,
      innerClassName: "modal"
    }, react__WEBPACK_IMPORTED_MODULE_6___default.a.createElement(_SearchShortcutsForm__WEBPACK_IMPORTED_MODULE_7__["SearchShortcutsForm"], {
      TopSites: props.TopSites,
      onClose: this.onSearchShortcutsFormClose,
      dispatch: this.props.dispatch
    }))))));
  }

}
const TopSites = Object(react_redux__WEBPACK_IMPORTED_MODULE_4__["connect"])(state => ({
  TopSites: state.TopSites,
  Prefs: state.Prefs,
  TopSitesRows: state.Prefs.values.topSitesRows
}))(_TopSites);
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 44 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TOP_SITES_SOURCE", function() { return TOP_SITES_SOURCE; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TOP_SITES_CONTEXT_MENU_OPTIONS", function() { return TOP_SITES_CONTEXT_MENU_OPTIONS; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TOP_SITES_SEARCH_SHORTCUTS_CONTEXT_MENU_OPTIONS", function() { return TOP_SITES_SEARCH_SHORTCUTS_CONTEXT_MENU_OPTIONS; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "MIN_RICH_FAVICON_SIZE", function() { return MIN_RICH_FAVICON_SIZE; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "MIN_CORNER_FAVICON_SIZE", function() { return MIN_CORNER_FAVICON_SIZE; });
const TOP_SITES_SOURCE = "TOP_SITES";
const TOP_SITES_CONTEXT_MENU_OPTIONS = ["CheckPinTopSite", "EditTopSite", "Separator", "OpenInNewWindow", "OpenInPrivateWindow", "Separator", "BlockUrl", "DeleteUrl"]; // the special top site for search shortcut experiment can only have the option to unpin (which removes) the topsite

const TOP_SITES_SEARCH_SHORTCUTS_CONTEXT_MENU_OPTIONS = ["CheckPinTopSite", "Separator", "BlockUrl"]; // minimum size necessary to show a rich icon instead of a screenshot

const MIN_RICH_FAVICON_SIZE = 96; // minimum size necessary to show any icon in the top left corner with a screenshot

const MIN_CORNER_FAVICON_SIZE = 16;

/***/ }),
/* 45 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "SelectableSearchShortcut", function() { return SelectableSearchShortcut; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "SearchShortcutsForm", function() { return SearchShortcutsForm; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _TopSitesConstants__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(44);



class SelectableSearchShortcut extends react__WEBPACK_IMPORTED_MODULE_1___default.a.PureComponent {
  render() {
    const {
      shortcut,
      selected
    } = this.props;
    const imageStyle = {
      backgroundImage: `url("${shortcut.tippyTopIcon}")`
    };
    return react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: "top-site-outer search-shortcut"
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("input", {
      type: "checkbox",
      id: shortcut.keyword,
      name: shortcut.keyword,
      checked: selected,
      onChange: this.props.onChange
    }), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("label", {
      htmlFor: shortcut.keyword
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: "top-site-inner"
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("span", null, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: "tile"
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: "top-site-icon rich-icon",
      style: imageStyle,
      "data-fallback": "@"
    }), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: "top-site-icon search-topsite"
    })), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: "title"
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("span", {
      dir: "auto"
    }, shortcut.keyword))))));
  }

}
class SearchShortcutsForm extends react__WEBPACK_IMPORTED_MODULE_1___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.onCancelButtonClick = this.onCancelButtonClick.bind(this);
    this.onSaveButtonClick = this.onSaveButtonClick.bind(this); // clone the shortcuts and add them to the state so we can add isSelected property

    const shortcuts = [];
    const {
      rows,
      searchShortcuts
    } = props.TopSites;
    searchShortcuts.forEach(shortcut => {
      shortcuts.push({ ...shortcut,
        isSelected: !!rows.find(row => row && row.isPinned && row.searchTopSite && row.label === shortcut.keyword)
      });
    });
    this.state = {
      shortcuts
    };
  }

  handleChange(event) {
    const {
      target
    } = event;
    const {
      name,
      checked
    } = target;
    this.setState(prevState => {
      const shortcuts = prevState.shortcuts.slice();
      let shortcut = shortcuts.find(({
        keyword
      }) => keyword === name);
      shortcut.isSelected = checked;
      return {
        shortcuts
      };
    });
  }

  onCancelButtonClick(ev) {
    ev.preventDefault();
    this.props.onClose();
  }

  onSaveButtonClick(ev) {
    ev.preventDefault(); // Check if there were any changes and act accordingly

    const {
      rows
    } = this.props.TopSites;
    const pinQueue = [];
    const unpinQueue = [];
    this.state.shortcuts.forEach(shortcut => {
      const alreadyPinned = rows.find(row => row && row.isPinned && row.searchTopSite && row.label === shortcut.keyword);

      if (shortcut.isSelected && !alreadyPinned) {
        pinQueue.push(this._searchTopSite(shortcut));
      } else if (!shortcut.isSelected && alreadyPinned) {
        unpinQueue.push({
          url: alreadyPinned.url,
          searchVendor: shortcut.shortURL
        });
      }
    }); // Tell the feed to do the work.

    this.props.onClose();
  }

  _searchTopSite(shortcut) {
    return {
      url: shortcut.url,
      searchTopSite: true,
      label: shortcut.keyword,
      searchVendor: shortcut.shortURL
    };
  }

  render() {
    return react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("form", {
      className: "topsite-form"
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", {
      className: "search-shortcuts-container"
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("h3", {
      className: "section-title grey-title",
      "data-l10n-id": "newtab-topsites-add-search-engine-header"
    }), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("div", null, this.state.shortcuts.map(shortcut => react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement(SelectableSearchShortcut, {
      key: shortcut.keyword,
      shortcut: shortcut,
      selected: shortcut.isSelected,
      onChange: this.handleChange
    })))), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("section", {
      className: "actions"
    }, react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("button", {
      className: "cancel",
      type: "button",
      onClick: this.onCancelButtonClick,
      "data-l10n-id": "newtab-topsites-cancel-button"
    }), react__WEBPACK_IMPORTED_MODULE_1___default.a.createElement("button", {
      className: "done",
      type: "submit",
      onClick: this.onSaveButtonClick,
      "data-l10n-id": "newtab-topsites-save-button"
    })));
  }

}

/***/ }),
/* 46 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TopSiteLink", function() { return TopSiteLink; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TopSite", function() { return TopSite; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TopSitePlaceholder", function() { return TopSitePlaceholder; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TopSiteList", function() { return TopSiteList; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(44);
/* harmony import */ var content_src_components_LinkMenu_LinkMenu__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(56);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_3__);
/* harmony import */ var content_src_lib_screenshot_utils__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(37);
/* harmony import */ var common_Reducers_jsm__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(53);
function _extends() { _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; }; return _extends.apply(this, arguments); }







class TopSiteLink extends react__WEBPACK_IMPORTED_MODULE_3___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.state = {
      screenshotImage: null
    };
    this.onDragEvent = this.onDragEvent.bind(this);
    this.onKeyPress = this.onKeyPress.bind(this);
  }
  /*
   * Helper to determine whether the drop zone should allow a drop. We only allow
   * dropping top sites for now.
   */


  _allowDrop(e) {
    return e.dataTransfer.types.includes("text/topsite-index");
  }

  onDragEvent(event) {
    switch (event.type) {
      case "click":
        // Stop any link clicks if we started any dragging
        if (this.dragged) {
          event.preventDefault();
        }

        break;

      case "dragstart":
        this.dragged = true;
        event.dataTransfer.effectAllowed = "move";
        event.dataTransfer.setData("text/topsite-index", this.props.index);
        event.target.blur();
        this.props.onDragEvent(event, this.props.index, this.props.link, this.props.title);
        break;

      case "dragend":
        this.props.onDragEvent(event);
        break;

      case "dragenter":
      case "dragover":
      case "drop":
        if (this._allowDrop(event)) {
          event.preventDefault();
          this.props.onDragEvent(event, this.props.index);
        }

        break;

      case "mousedown":
        // Block the scroll wheel from appearing for middle clicks on search top sites
        if (event.button === 1 && this.props.link.searchTopSite) {
          event.preventDefault();
        } // Reset at the first mouse event of a potential drag


        this.dragged = false;
        break;
    }
  }
  /**
   * Helper to obtain the next state based on nextProps and prevState.
   *
   * NOTE: Rename this method to getDerivedStateFromProps when we update React
   *       to >= 16.3. We will need to update tests as well. We cannot rename this
   *       method to getDerivedStateFromProps now because there is a mismatch in
   *       the React version that we are using for both testing and production.
   *       (i.e. react-test-render => "16.3.2", react => "16.2.0").
   *
   * See https://github.com/airbnb/enzyme/blob/master/packages/enzyme-adapter-react-16/package.json#L43.
   */


  static getNextStateFromProps(nextProps, prevState) {
    const {
      screenshot
    } = nextProps.link;
    const imageInState = content_src_lib_screenshot_utils__WEBPACK_IMPORTED_MODULE_4__["ScreenshotUtils"].isRemoteImageLocal(prevState.screenshotImage, screenshot);

    if (imageInState) {
      return null;
    } // Since image was updated, attempt to revoke old image blob URL, if it exists.


    content_src_lib_screenshot_utils__WEBPACK_IMPORTED_MODULE_4__["ScreenshotUtils"].maybeRevokeBlobObjectURL(prevState.screenshotImage);
    return {
      screenshotImage: content_src_lib_screenshot_utils__WEBPACK_IMPORTED_MODULE_4__["ScreenshotUtils"].createLocalImageObject(screenshot)
    };
  } // NOTE: Remove this function when we update React to >= 16.3 since React will
  //       call getDerivedStateFromProps automatically. We will also need to
  //       rename getNextStateFromProps to getDerivedStateFromProps.


  componentWillMount() {
    const nextState = TopSiteLink.getNextStateFromProps(this.props, this.state);

    if (nextState) {
      this.setState(nextState);
    }
  } // NOTE: Remove this function when we update React to >= 16.3 since React will
  //       call getDerivedStateFromProps automatically. We will also need to
  //       rename getNextStateFromProps to getDerivedStateFromProps.


  componentWillReceiveProps(nextProps) {
    const nextState = TopSiteLink.getNextStateFromProps(nextProps, this.state);

    if (nextState) {
      this.setState(nextState);
    }
  }

  componentWillUnmount() {
    content_src_lib_screenshot_utils__WEBPACK_IMPORTED_MODULE_4__["ScreenshotUtils"].maybeRevokeBlobObjectURL(this.state.screenshotImage);
  }

  onKeyPress(event) {
    // If we have tabbed to a search shortcut top site, and we click 'enter',
    // we should execute the onClick function. This needs to be added because
    // search top sites are anchor tags without an href. See bug 1483135
    if (this.props.link.searchTopSite && event.key === "Enter") {
      this.props.onClick(event);
    }
  }

  render() {
    const {
      children,
      className,
      defaultStyle,
      isDraggable,
      link,
      onClick,
      title
    } = this.props;
    const topSiteOuterClassName = `top-site-outer${className ? ` ${className}` : ""}${link.isDragged ? " dragged" : ""}${link.searchTopSite ? " search-shortcut" : ""}`;
    const {
      tippyTopIcon,
      faviconSize
    } = link;
    const [letterFallback] = title;
    let imageClassName;
    let imageStyle;
    let showSmallFavicon = false;
    let smallFaviconStyle;
    let smallFaviconFallback;
    let hasScreenshotImage = this.state.screenshotImage && this.state.screenshotImage.url;

    if (defaultStyle) {
      // force no styles (letter fallback) even if the link has imagery
      smallFaviconFallback = false;
    } else if (link.searchTopSite) {
      imageClassName = "top-site-icon rich-icon";
      imageStyle = {
        backgroundColor: link.backgroundColor,
        backgroundImage: `url(${tippyTopIcon})`
      };
      smallFaviconStyle = {
        backgroundImage: `url(${tippyTopIcon})`
      };
    } else if (link.customScreenshotURL) {
      // assume high quality custom screenshot and use rich icon styles and class names
      imageClassName = "top-site-icon rich-icon";
      imageStyle = {
        backgroundColor: link.backgroundColor,
        backgroundImage: hasScreenshotImage ? `url(${this.state.screenshotImage.url})` : "none"
      };
    } else if (tippyTopIcon || faviconSize >= _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["MIN_RICH_FAVICON_SIZE"]) {
      // styles and class names for top sites with rich icons
      imageClassName = "top-site-icon rich-icon";
      imageStyle = {
        backgroundColor: link.backgroundColor,
        backgroundImage: `url(${tippyTopIcon || link.favicon})`
      };
    } else {
      // styles and class names for top sites with screenshot + small icon in top left corner
      imageClassName = `screenshot${hasScreenshotImage ? " active" : ""}`;
      imageStyle = {
        backgroundImage: hasScreenshotImage ? `url(${this.state.screenshotImage.url})` : "none"
      }; // only show a favicon in top left if it's greater than 16x16

      if (faviconSize >= _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["MIN_CORNER_FAVICON_SIZE"]) {
        showSmallFavicon = true;
        smallFaviconStyle = {
          backgroundImage: `url(${link.favicon})`
        };
      } else if (hasScreenshotImage) {
        // Don't show a small favicon if there is no screenshot, because that
        // would result in two fallback icons
        showSmallFavicon = true;
        smallFaviconFallback = true;
      }
    }

    let draggableProps = {};

    if (isDraggable) {
      draggableProps = {
        onClick: this.onDragEvent,
        onDragEnd: this.onDragEvent,
        onDragStart: this.onDragEvent,
        onMouseDown: this.onDragEvent
      };
    }

    return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("li", _extends({
      className: topSiteOuterClassName,
      onDrop: this.onDragEvent,
      onDragOver: this.onDragEvent,
      onDragEnter: this.onDragEvent,
      onDragLeave: this.onDragEvent
    }, draggableProps), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "top-site-inner"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("a", {
      className: "top-site-button",
      href: link.searchTopSite ? undefined : link.url,
      tabIndex: "0",
      onKeyPress: this.onKeyPress,
      onClick: onClick,
      draggable: true
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "tile",
      "aria-hidden": true,
      "data-fallback": letterFallback
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: imageClassName,
      style: imageStyle
    }), link.searchTopSite && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "top-site-icon search-topsite"
    }), showSmallFavicon && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "top-site-icon default-icon",
      "data-fallback": smallFaviconFallback && letterFallback,
      style: smallFaviconStyle
    })), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: `title ${link.isPinned ? "pinned" : ""}`
    }, link.isPinned && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "icon icon-pin-small"
    }), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("span", {
      dir: "auto"
    }, title))), children));
  }

}
TopSiteLink.defaultProps = {
  title: "",
  link: {},
  isDraggable: true
};
class TopSite extends react__WEBPACK_IMPORTED_MODULE_3___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.state = {
      showContextMenu: false
    };
    this.onLinkClick = this.onLinkClick.bind(this);
    this.onMenuButtonClick = this.onMenuButtonClick.bind(this);
    this.onMenuUpdate = this.onMenuUpdate.bind(this);
  }

  onLinkClick(event) {

    event.preventDefault();
    const {
      altKey,
      button,
      ctrlKey,
      metaKey,
      shiftKey
    } = event;

    if (!this.props.link.searchTopSite) {
      this.props.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
        type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].OPEN_LINK,
        data: Object.assign(this.props.link, {
          event: {
            altKey,
            button,
            ctrlKey,
            metaKey,
            shiftKey
          }
        })
      }));
    } else {
      this.props.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
        type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].FILL_SEARCH_TERM,
        data: {
          label: this.props.link.label
        }
      }));
    }
  }

  onMenuButtonClick(event) {
    event.preventDefault();
    this.props.onActivate(this.props.index);
    this.setState({
      showContextMenu: true
    });
  }

  onMenuUpdate(showContextMenu) {
    this.setState({
      showContextMenu
    });
  }

  render() {
    const {
      props
    } = this;
    const {
      link
    } = props;
    const isContextMenuOpen = this.state.showContextMenu && props.activeIndex === props.index;
    const title = link.label || link.hostname;
    return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(TopSiteLink, _extends({}, props, {
      onClick: this.onLinkClick,
      onDragEvent: this.props.onDragEvent,
      className: `${props.className || ""}${isContextMenuOpen ? " active" : ""}`,
      title: title
    }), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", null, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("button", {
      "aria-haspopup": "true",
      className: "context-menu-button icon",
      "data-l10n-id": "newtab-menu-content-tooltip",
      "data-l10n-args": JSON.stringify({
        title
      }),
      onClick: this.onMenuButtonClick
    }), isContextMenuOpen && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(content_src_components_LinkMenu_LinkMenu__WEBPACK_IMPORTED_MODULE_2__["LinkMenu"], {
      dispatch: props.dispatch,
      index: props.index,
      onUpdate: this.onMenuUpdate,
      options: link.searchTopSite ? _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["TOP_SITES_SEARCH_SHORTCUTS_CONTEXT_MENU_OPTIONS"] : _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["TOP_SITES_CONTEXT_MENU_OPTIONS"],
      site: link,
      source: _TopSitesConstants__WEBPACK_IMPORTED_MODULE_1__["TOP_SITES_SOURCE"]
    })));
  }

}
TopSite.defaultProps = {
  link: {},

  onActivate() {}

};
class TopSitePlaceholder extends react__WEBPACK_IMPORTED_MODULE_3___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.onEditButtonClick = this.onEditButtonClick.bind(this);
  }

  onEditButtonClick() {
    this.props.dispatch({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].TOP_SITES_EDIT,
      data: {
        index: this.props.index
      }
    });
  }

  render() {
    return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(TopSiteLink, _extends({}, this.props, {
      className: `placeholder ${this.props.className || ""}`,
      isDraggable: false
    }), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("button", {
      "aria-haspopup": "true",
      className: "context-menu-button edit-button icon",
      "data-l10n-id": "newtab-menu-topsites-placeholder-tooltip",
      onClick: this.onEditButtonClick
    }));
  }

}
class TopSiteList extends react__WEBPACK_IMPORTED_MODULE_3___default.a.PureComponent {
  static get DEFAULT_STATE() {
    return {
      activeIndex: null,
      draggedIndex: null,
      draggedSite: null,
      draggedTitle: null,
      topSitesPreview: null
    };
  }

  constructor(props) {
    super(props);
    this.state = TopSiteList.DEFAULT_STATE;
    this.onDragEvent = this.onDragEvent.bind(this);
    this.onActivate = this.onActivate.bind(this);
  }

  componentWillReceiveProps(nextProps) {
    if (this.state.draggedSite) {
      const prevTopSites = this.props.TopSites && this.props.TopSites.rows;
      const newTopSites = nextProps.TopSites && nextProps.TopSites.rows;

      if (prevTopSites && prevTopSites[this.state.draggedIndex] && prevTopSites[this.state.draggedIndex].url === this.state.draggedSite.url && (!newTopSites[this.state.draggedIndex] || newTopSites[this.state.draggedIndex].url !== this.state.draggedSite.url)) {
        // We got the new order from the redux store via props. We can clear state now.
        this.setState(TopSiteList.DEFAULT_STATE);
      }
    }
  }

  onDragEvent(event, index, link, title) {
    switch (event.type) {
      case "dragstart":
        this.dropped = false;
        this.setState({
          draggedIndex: index,
          draggedSite: link,
          draggedTitle: title,
          activeIndex: null
        });
        break;

      case "dragend":
        if (!this.dropped) {
          // If there was no drop event, reset the state to the default.
          this.setState(TopSiteList.DEFAULT_STATE);
        }

        break;

      case "dragenter":
        if (index === this.state.draggedIndex) {
          this.setState({
            topSitesPreview: null
          });
        } else {
          this.setState({
            topSitesPreview: this._makeTopSitesPreview(index)
          });
        }

        break;

      case "drop":
        if (index !== this.state.draggedIndex) {
          this.dropped = true;
          this.props.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].AlsoToMain({
            type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].TOP_SITES_INSERT,
            data: {
              site: {
                url: this.state.draggedSite.url,
                label: this.state.draggedTitle,
                customScreenshotURL: this.state.draggedSite.customScreenshotURL,
                // Only if the search topsites experiment is enabled
                ...(this.state.draggedSite.searchTopSite && {
                  searchTopSite: true
                })
              },
              index,
              draggedFromIndex: this.state.draggedIndex
            }
          }));
        }

        break;
    }
  }

  _getTopSites() {
    // Make a copy of the sites to truncate or extend to desired length
    let topSites = this.props.TopSites.rows.slice();
    topSites.length = this.props.TopSitesRows * common_Reducers_jsm__WEBPACK_IMPORTED_MODULE_5__["TOP_SITES_MAX_SITES_PER_ROW"];
    return topSites;
  }
  /**
   * Make a preview of the topsites that will be the result of dropping the currently
   * dragged site at the specified index.
   */


  _makeTopSitesPreview(index) {
    const topSites = this._getTopSites();

    topSites[this.state.draggedIndex] = null;
    const pinnedOnly = topSites.map(site => site && site.isPinned ? site : null);
    const unpinned = topSites.filter(site => site && !site.isPinned);
    const siteToInsert = Object.assign({}, this.state.draggedSite, {
      isPinned: true,
      isDragged: true
    });

    if (!pinnedOnly[index]) {
      pinnedOnly[index] = siteToInsert;
    } else {
      // Find the hole to shift the pinned site(s) towards. We shift towards the
      // hole left by the site being dragged.
      let holeIndex = index;
      const indexStep = index > this.state.draggedIndex ? -1 : 1;

      while (pinnedOnly[holeIndex]) {
        holeIndex += indexStep;
      } // Shift towards the hole.


      const shiftingStep = index > this.state.draggedIndex ? 1 : -1;

      while (holeIndex !== index) {
        const nextIndex = holeIndex + shiftingStep;
        pinnedOnly[holeIndex] = pinnedOnly[nextIndex];
        holeIndex = nextIndex;
      }

      pinnedOnly[index] = siteToInsert;
    } // Fill in the remaining holes with unpinned sites.


    const preview = pinnedOnly;

    for (let i = 0; i < preview.length; i++) {
      if (!preview[i]) {
        preview[i] = unpinned.shift() || null;
      }
    }

    return preview;
  }

  onActivate(index) {
    this.setState({
      activeIndex: index
    });
  }

  render() {
    const {
      props
    } = this;

    const topSites = this.state.topSitesPreview || this._getTopSites();

    const topSitesUI = [];
    const commonProps = {
      onDragEvent: this.onDragEvent,
      dispatch: props.dispatch
    }; // We assign a key to each placeholder slot. We need it to be independent
    // of the slot index (i below) so that the keys used stay the same during
    // drag and drop reordering and the underlying DOM nodes are reused.
    // This mostly (only?) affects linux so be sure to test on linux before changing.

    let holeIndex = 0; // On narrow viewports, we only show 6 sites per row. We'll mark the rest as
    // .hide-for-narrow to hide in CSS via @media query.

    const maxNarrowVisibleIndex = props.TopSitesRows * 6;

    for (let i = 0, l = topSites.length; i < l; i++) {
      const link = topSites[i] && Object.assign({}, topSites[i], {
        iconType: this.props.topSiteIconType(topSites[i])
      });
      const slotProps = {
        key: link ? link.url : holeIndex++,
        index: i
      };

      if (i >= maxNarrowVisibleIndex) {
        slotProps.className = "hide-for-narrow";
      }

      topSitesUI.push(!link ? react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(TopSitePlaceholder, _extends({}, slotProps, commonProps)) : react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement(TopSite, _extends({
        link: link,
        activeIndex: this.state.activeIndex,
        onActivate: this.onActivate
      }, slotProps, commonProps)));
    }

    return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("ul", {
      className: `top-sites-list${this.state.draggedSite ? " dnd-active" : ""}`
    }, topSitesUI);
  }

}

/***/ }),
/* 47 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_Search", function() { return _Search; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Search", function() { return Search; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(24);
/* harmony import */ var react_redux__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(react_redux__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var content_src_lib_constants__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(11);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(9);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_3__);
/* globals ContentSearchUIController */






class _Search extends react__WEBPACK_IMPORTED_MODULE_3___default.a.PureComponent {
  constructor(props) {
    super(props);
    this.onSearchClick = this.onSearchClick.bind(this);
    this.onSearchHandoffClick = this.onSearchHandoffClick.bind(this);
    this.onSearchHandoffPaste = this.onSearchHandoffPaste.bind(this);
    this.onSearchHandoffDrop = this.onSearchHandoffDrop.bind(this);
    this.onInputMount = this.onInputMount.bind(this);
    this.onSearchHandoffButtonMount = this.onSearchHandoffButtonMount.bind(this);
  }

  onSearchClick(event) {
    window.gContentSearchController.search(event);
  }

  doSearchHandoff(text) {
    this.props.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].OnlyToMain({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].HANDOFF_SEARCH_TO_AWESOMEBAR,
      data: {
        text
      }
    }));
    this.props.dispatch({
      type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].FAKE_FOCUS_SEARCH
    });

    if (text) {
      this.props.dispatch({
        type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].HIDE_SEARCH
      });
    }
  }

  onSearchHandoffClick(event) {
    // When search hand-off is enabled, we render a big button that is styled to
    // look like a search textbox. If the button is clicked, we style
    // the button as if it was a focused search box and show a fake cursor but
    // really focus the awesomebar without the focus styles ("hidden focus").
    event.preventDefault();
    this.doSearchHandoff();
  }

  onSearchHandoffPaste(event) {
    event.preventDefault();
    this.doSearchHandoff(event.clipboardData.getData("Text"));
  }

  onSearchHandoffDrop(event) {
    event.preventDefault();
    let text = event.dataTransfer.getData("text");

    if (text) {
      this.doSearchHandoff(text);
    }
  }

  componentWillUnmount() {
    delete window.gContentSearchController;
  }

  onInputMount(input) {
    if (input) {
      // the search data and acts as context for the search request (See
      // nsISearchEngine.getSubmission). It is necessary so that search engine
      // plugins can correctly atribute referrals. (See github ticket #3321 for
      // more details)

      const searchSource = content_src_lib_constants__WEBPACK_IMPORTED_MODULE_2__["IS_NEWTAB"] ? "newtab" : "homepage"; // gContentSearchController needs to exist as a global so that tests for
      // the existing about:home can find it; and so it allows these tests to pass.
      // In the future, when activity stream is default about:home, this can be renamed

      window.gContentSearchController = new ContentSearchUIController(input, input.parentNode, null, searchSource);
      addEventListener("ContentSearchClient", this);
    } else {
      window.gContentSearchController = null;
      removeEventListener("ContentSearchClient", this);
    }
  }

  onSearchHandoffButtonMount(button) {
    // Keep a reference to the button for use during "paste" event handling.
    this._searchHandoffButton = button;
  }
  /*
   * Do not change the ID on the input field, as legacy newtab code
   * specifically looks for the id 'newtab-search-text' on input fields
   * in order to execute searches in various tests
   */


  render() {
    const wrapperClassName = ["search-wrapper", this.props.hide && "search-hidden", this.props.fakeFocus && "fake-focus"].filter(v => v).join(" ");
    return react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: wrapperClassName
    }, this.props.showLogo && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "logo-and-wordmark"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "logo"
    }), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "wordmark"
    })), !this.props.handoffEnabled && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "search-inner-wrapper"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("input", {
      id: "newtab-search-text",
      "data-l10n-id": "newtab-search-box-search-the-web-input",
      maxLength: "256",
      ref: this.onInputMount,
      type: "search"
    }), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("button", {
      id: "searchSubmit",
      className: "search-button",
      "data-l10n-id": "newtab-search-box-search-button",
      onClick: this.onSearchClick
    })), this.props.handoffEnabled && react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "search-inner-wrapper"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("button", {
      className: "search-handoff-button",
      "data-l10n-id": "newtab-search-box-search-the-web-input",
      ref: this.onSearchHandoffButtonMount,
      onClick: this.onSearchHandoffClick,
      tabIndex: "-1"
    }, react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "fake-textbox",
      "data-l10n-id": "newtab-search-box-search-the-web-text"
    }), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("input", {
      type: "search",
      className: "fake-editable",
      tabIndex: "-1",
      "aria-hidden": "true",
      onDrop: this.onSearchHandoffDrop,
      onPaste: this.onSearchHandoffPaste
    }), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("div", {
      className: "fake-caret"
    })), react__WEBPACK_IMPORTED_MODULE_3___default.a.createElement("input", {
      type: "search",
      style: {
        display: "none"
      },
      ref: this.onInputMount
    })));
  }

}
const Search = Object(react_redux__WEBPACK_IMPORTED_MODULE_1__["connect"])()(_Search);

/***/ }),
/* 48 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* WEBPACK VAR INJECTION */(function(global) {/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "DetectUserSessionStart", function() { return DetectUserSessionStart; });
/* harmony import */ var common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(2);
/* harmony import */ var common_PerfService_jsm__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(39);


const VISIBLE = "visible";
const VISIBILITY_CHANGE_EVENT = "visibilitychange";
class DetectUserSessionStart {
  constructor(store, options = {}) {
    this._store = store; // Overrides for testing

    this.document = options.document || global.document;
    this._perfService = options.perfService || common_PerfService_jsm__WEBPACK_IMPORTED_MODULE_1__["perfService"];
    this._onVisibilityChange = this._onVisibilityChange.bind(this);
  }
  /**
   * sendEventOrAddListener - Notify immediately if the page is already visible,
   *                    or else set up a listener for when visibility changes.
   *                    This is needed for accurate session tracking for telemetry,
   *                    because tabs are pre-loaded.
   */


  sendEventOrAddListener() {
    if (this.document.visibilityState === VISIBLE) {
      // If the document is already visible, to the user, send a notification
      // immediately that a session has started.
      this._sendEvent();
    } else {
      // If the document is not visible, listen for when it does become visible.
      this.document.addEventListener(VISIBILITY_CHANGE_EVENT, this._onVisibilityChange);
    }
  }
  /**
   * _sendEvent - Sends a message to the main process to indicate the current
   *              tab is now visible to the user, includes the
   *              visibility_event_rcvd_ts time in ms from the UNIX epoch.
   */


  _sendEvent() {
    this._perfService.mark("visibility_event_rcvd_ts");

    try {
      let visibility_event_rcvd_ts = this._perfService.getMostRecentAbsMarkStartByName("visibility_event_rcvd_ts");

      this._store.dispatch(common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionCreators"].AlsoToMain({
        type: common_Actions_jsm__WEBPACK_IMPORTED_MODULE_0__["actionTypes"].SAVE_SESSION_PERF_DATA,
        data: {
          visibility_event_rcvd_ts
        }
      }));
    } catch (ex) {// If this failed, it's likely because the `privacy.resistFingerprinting`
      // pref is true.  We should at least not blow up.
    }
  }
  /**
   * _onVisibilityChange - If the visibility has changed to visible, sends a notification
   *                      and removes the event listener. This should only be called once per tab.
   */


  _onVisibilityChange() {
    if (this.document.visibilityState === VISIBLE) {
      this._sendEvent();

      this.document.removeEventListener(VISIBILITY_CHANGE_EVENT, this._onVisibilityChange);
    }
  }

}
/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(1)))

/***/ }),
/* 49 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 50 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);

// EXTERNAL MODULE: external "React"
var external_React_ = __webpack_require__(9);

// EXTERNAL MODULE: external "PropTypes"
var external_PropTypes_ = __webpack_require__(10);
var external_PropTypes_default = /*#__PURE__*/__webpack_require__.n(external_PropTypes_);

// CONCATENATED MODULE: ./node_modules/fluent-sequence/src/map_sync.js
/*
 * Synchronously map an identifier or an array of identifiers to the best
 * `FluentBundle` instance(s).
 *
 * @param {Iterable} iterable
 * @param {string|Array<string>} ids
 * @returns {FluentBundle|Array<FluentBundle>}
 */
function mapBundleSync(iterable, ids) {
  if (!Array.isArray(ids)) {
    return getBundleForId(iterable, ids);
  }

  return ids.map(
    id => getBundleForId(iterable, id)
  );
}

/*
 * Find the best `FluentBundle` with the translation for `id`.
 */
function getBundleForId(iterable, id) {
  for (const bundle of iterable) {
    if (bundle.hasMessage(id)) {
      return bundle;
    }
  }

  return null;
}

// CONCATENATED MODULE: ./node_modules/fluent-sequence/src/map_async.js
/*
 * Asynchronously map an identifier or an array of identifiers to the best
 * `FluentBundle` instance(s).
 *
 * @param {AsyncIterable} iterable
 * @param {string|Array<string>} ids
 * @returns {Promise<FluentBundle|Array<FluentBundle>>}
 */
async function mapBundleAsync(iterable, ids) {
  if (!Array.isArray(ids)) {
    for await (const bundle of iterable) {
      if (bundle.hasMessage(ids)) {
        return bundle;
      }
    }
  }

  let remainingCount = ids.length;
  const foundBundles = new Array(remainingCount).fill(null);

  for await (const bundle of iterable) {
    for (const [index, id] of ids.entries()) {
      if (!foundBundles[index] && bundle.hasMessage(id)) {
        foundBundles[index] = bundle;
        remainingCount--;
      }

      // Return early when all ids have been mapped to contexts.
      if (remainingCount === 0) {
        return foundBundles;
      }
    }
  }

  return foundBundles;
}

// CONCATENATED MODULE: ./node_modules/fluent-sequence/src/index.js
/*
 * @module fluent-sequence
 * @overview Manage ordered sequences of FluentBundles.
 */




// CONCATENATED MODULE: ./node_modules/cached-iterable/src/cached_iterable.mjs
/*
 * Base CachedIterable class.
 */
class CachedIterable extends Array {
    /**
     * Create a `CachedIterable` instance from an iterable or, if another
     * instance of `CachedIterable` is passed, return it without any
     * modifications.
     *
     * @param {Iterable} iterable
     * @returns {CachedIterable}
     */
    static from(iterable) {
        if (iterable instanceof this) {
            return iterable;
        }

        return new this(iterable);
    }
}

// CONCATENATED MODULE: ./node_modules/cached-iterable/src/cached_sync_iterable.mjs


/*
 * CachedSyncIterable caches the elements yielded by an iterable.
 *
 * It can be used to iterate over an iterable many times without depleting the
 * iterable.
 */
class cached_sync_iterable_CachedSyncIterable extends CachedIterable {
    /**
     * Create an `CachedSyncIterable` instance.
     *
     * @param {Iterable} iterable
     * @returns {CachedSyncIterable}
     */
    constructor(iterable) {
        super();

        if (Symbol.iterator in Object(iterable)) {
            this.iterator = iterable[Symbol.iterator]();
        } else {
            throw new TypeError("Argument must implement the iteration protocol.");
        }
    }

    [Symbol.iterator]() {
        const cached = this;
        let cur = 0;

        return {
            next() {
                if (cached.length <= cur) {
                    cached.push(cached.iterator.next());
                }
                return cached[cur++];
            }
        };
    }

    /**
     * This method allows user to consume the next element from the iterator
     * into the cache.
     *
     * @param {number} count - number of elements to consume
     */
    touchNext(count = 1) {
        let idx = 0;
        while (idx++ < count) {
            const last = this[this.length - 1];
            if (last && last.done) {
                break;
            }
            this.push(this.iterator.next());
        }
        // Return the last cached {value, done} object to allow the calling
        // code to decide if it needs to call touchNext again.
        return this[this.length - 1];
    }
}

// CONCATENATED MODULE: ./node_modules/cached-iterable/src/cached_async_iterable.mjs


/*
 * CachedAsyncIterable caches the elements yielded by an async iterable.
 *
 * It can be used to iterate over an iterable many times without depleting the
 * iterable.
 */
class cached_async_iterable_CachedAsyncIterable extends CachedIterable {
    /**
     * Create an `CachedAsyncIterable` instance.
     *
     * @param {Iterable} iterable
     * @returns {CachedAsyncIterable}
     */
    constructor(iterable) {
        super();

        if (Symbol.asyncIterator in Object(iterable)) {
            this.iterator = iterable[Symbol.asyncIterator]();
        } else if (Symbol.iterator in Object(iterable)) {
            this.iterator = iterable[Symbol.iterator]();
        } else {
            throw new TypeError("Argument must implement the iteration protocol.");
        }
    }

    /**
     * Synchronous iterator over the cached elements.
     *
     * Return a generator object implementing the iterator protocol over the
     * cached elements of the original (async or sync) iterable.
     */
    [Symbol.iterator]() {
        const cached = this;
        let cur = 0;

        return {
            next() {
                if (cached.length === cur) {
                    return {value: undefined, done: true};
                }
                return cached[cur++];
            }
        };
    }

    /**
     * Asynchronous iterator caching the yielded elements.
     *
     * Elements yielded by the original iterable will be cached and available
     * synchronously. Returns an async generator object implementing the
     * iterator protocol over the elements of the original (async or sync)
     * iterable.
     */
    [Symbol.asyncIterator]() {
        const cached = this;
        let cur = 0;

        return {
            async next() {
                if (cached.length <= cur) {
                    cached.push(await cached.iterator.next());
                }
                return cached[cur++];
            }
        };
    }

    /**
     * This method allows user to consume the next element from the iterator
     * into the cache.
     *
     * @param {number} count - number of elements to consume
     */
    async touchNext(count = 1) {
        let idx = 0;
        while (idx++ < count) {
            const last = this[this.length - 1];
            if (last && last.done) {
                break;
            }
            this.push(await this.iterator.next());
        }
        // Return the last cached {value, done} object to allow the calling
        // code to decide if it needs to call touchNext again.
        return this[this.length - 1];
    }
}

// CONCATENATED MODULE: ./node_modules/cached-iterable/src/index.mjs



// CONCATENATED MODULE: ./node_modules/fluent-react/src/localization.js


/*
 * `ReactLocalization` handles translation formatting and fallback.
 *
 * The current negotiated fallback chain of languages is stored in the
 * `ReactLocalization` instance in form of an iterable of `FluentBundle`
 * instances.  This iterable is used to find the best existing translation for
 * a given identifier.
 *
 * `Localized` components must subscribe to the changes of the
 * `ReactLocalization`'s fallback chain.  When the fallback chain changes (the
 * `bundles` iterable is set anew), all subscribed compontent must relocalize.
 *
 * The `ReactLocalization` class instances are exposed to `Localized` elements
 * via the `LocalizationProvider` component.
 */

class localization_ReactLocalization {
  constructor(bundles) {
    this.bundles = cached_sync_iterable_CachedSyncIterable.from(bundles);
    this.subs = new Set();
  }
  /*
   * Subscribe a `Localized` component to changes of `bundles`.
   */


  subscribe(comp) {
    this.subs.add(comp);
  }
  /*
   * Unsubscribe a `Localized` component from `bundles` changes.
   */


  unsubscribe(comp) {
    this.subs.delete(comp);
  }
  /*
   * Set a new `bundles` iterable and trigger the retranslation.
   */


  setBundles(bundles) {
    this.bundles = cached_sync_iterable_CachedSyncIterable.from(bundles); // Update all subscribed Localized components.

    this.subs.forEach(comp => comp.relocalize());
  }

  getBundle(id) {
    return mapBundleSync(this.bundles, id);
  }
  /*
   * Find a translation by `id` and format it to a string using `args`.
   */


  getString(id, args, fallback) {
    const bundle = this.getBundle(id);

    if (bundle === null) {
      return fallback || id;
    }

    const msg = bundle.getMessage(id);
    return bundle.format(msg, args);
  }

}
function isReactLocalization(props, propName) {
  const prop = props[propName];

  if (prop instanceof localization_ReactLocalization) {
    return null;
  }

  return new Error(`The ${propName} context field must be an instance of ReactLocalization.`);
}
// CONCATENATED MODULE: ./node_modules/fluent-react/src/markup.js
/* eslint-env browser */
let cachedParseMarkup; // We use a function creator to make the reference to `document` lazy. At the
// same time, it's eager enough to throw in <LocalizationProvider> as soon as
// it's first mounted which reduces the risk of this error making it to the
// runtime without developers noticing it in development.

function createParseMarkup() {
  if (typeof document === "undefined") {
    // We can't use <template> to sanitize translations.
    throw new Error("`document` is undefined. Without it, translations cannot " + "be safely sanitized. Consult the documentation at " + "https://github.com/projectfluent/fluent.js/wiki/React-Overlays.");
  }

  if (!cachedParseMarkup) {
    const template = document.createElement("template");

    cachedParseMarkup = function parseMarkup(str) {
      template.innerHTML = str;
      return Array.from(template.content.childNodes);
    };
  }

  return cachedParseMarkup;
}
// CONCATENATED MODULE: ./node_modules/fluent-react/src/provider.js




/*
 * The Provider component for the `ReactLocalization` class.
 *
 * Exposes a `ReactLocalization` instance to all descendants via React's
 * context feature.  It makes translations available to all localizable
 * elements in the descendant's render tree without the need to pass them
 * explicitly.
 *
 *     <LocalizationProvider bundles={…}>
 *         …
 *     </LocalizationProvider>
 *
 * The `LocalizationProvider` component takes one prop: `bundles`.  It should
 * be an iterable of `FluentBundle` instances in order of the user's
 * preferred languages.  The `FluentBundle` instances will be used by
 * `ReactLocalization` to format translations.  If a translation is missing in
 * one instance, `ReactLocalization` will fall back to the next one.
 */

class provider_LocalizationProvider extends external_React_["Component"] {
  constructor(props) {
    super(props);
    const {
      bundles,
      parseMarkup
    } = props;

    if (bundles === undefined) {
      throw new Error("LocalizationProvider must receive the bundles prop.");
    }

    if (!bundles[Symbol.iterator]) {
      throw new Error("The bundles prop must be an iterable.");
    }

    this.l10n = new localization_ReactLocalization(bundles);
    this.parseMarkup = parseMarkup || createParseMarkup();
  }

  getChildContext() {
    return {
      l10n: this.l10n,
      parseMarkup: this.parseMarkup
    };
  }

  componentWillReceiveProps(next) {
    const {
      bundles
    } = next;

    if (bundles !== this.props.bundles) {
      this.l10n.setBundles(bundles);
    }
  }

  render() {
    return external_React_["Children"].only(this.props.children);
  }

}
provider_LocalizationProvider.childContextTypes = {
  l10n: isReactLocalization,
  parseMarkup: external_PropTypes_default.a.func
};
provider_LocalizationProvider.propTypes = {
  children: external_PropTypes_default.a.element.isRequired,
  bundles: isIterable,
  parseMarkup: external_PropTypes_default.a.func
};

function isIterable(props, propName, componentName) {
  const prop = props[propName];

  if (Symbol.iterator in Object(prop)) {
    return null;
  }

  return new Error(`The ${propName} prop supplied to ${componentName} must be an iterable.`);
}
// CONCATENATED MODULE: ./node_modules/fluent-react/src/with_localization.js


function withLocalization(Inner) {
  class WithLocalization extends external_React_["Component"] {
    componentDidMount() {
      const {
        l10n
      } = this.context;

      if (l10n) {
        l10n.subscribe(this);
      }
    }

    componentWillUnmount() {
      const {
        l10n
      } = this.context;

      if (l10n) {
        l10n.unsubscribe(this);
      }
    }
    /*
     * Rerender this component in a new language.
     */


    relocalize() {
      // When the `ReactLocalization`'s fallback chain changes, update the
      // component.
      this.forceUpdate();
    }
    /*
     * Find a translation by `id` and format it to a string using `args`.
     */


    getString(id, args, fallback) {
      const {
        l10n
      } = this.context;

      if (!l10n) {
        return fallback || id;
      }

      return l10n.getString(id, args, fallback);
    }

    render() {
      return Object(external_React_["createElement"])(Inner, Object.assign( // getString needs to be re-bound on updates to trigger a re-render
      {
        getString: (...args) => this.getString(...args)
      }, this.props));
    }

  }

  WithLocalization.displayName = `WithLocalization(${displayName(Inner)})`;
  WithLocalization.contextTypes = {
    l10n: isReactLocalization
  };
  return WithLocalization;
}

function displayName(component) {
  return component.displayName || component.name || "Component";
}
// CONCATENATED MODULE: ./node_modules/fluent-react/vendor/omittedCloseTags.js
/**
 * Copyright (c) 2013-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in this directory.
 */
// For HTML, certain tags should omit their close tag. We keep a whitelist for
// those special-case tags.
var omittedCloseTags = {
  area: true,
  base: true,
  br: true,
  col: true,
  embed: true,
  hr: true,
  img: true,
  input: true,
  keygen: true,
  link: true,
  meta: true,
  param: true,
  source: true,
  track: true,
  wbr: true // NOTE: menuitem's close tag should be omitted, but that causes problems.

};
/* harmony default export */ var vendor_omittedCloseTags = (omittedCloseTags);
// CONCATENATED MODULE: ./node_modules/fluent-react/vendor/voidElementTags.js
/**
 * Copyright (c) 2013-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in this directory.
 */
 // For HTML, certain tags cannot have children. This has the same purpose as
// `omittedCloseTags` except that `menuitem` should still have its closing tag.

var voidElementTags = {
  menuitem: true,
  ...vendor_omittedCloseTags
};
/* harmony default export */ var vendor_voidElementTags = (voidElementTags);
// CONCATENATED MODULE: ./node_modules/fluent-react/src/localized.js



 // Match the opening angle bracket (<) in HTML tags, and HTML entities like
// &amp;, &#0038;, &#x0026;.

const reMarkup = /<|&#?\w+;/;
/*
 * Prepare props passed to `Localized` for formatting.
 */

function toArguments(props) {
  const args = {};
  const elems = {};

  for (const [propname, propval] of Object.entries(props)) {
    if (propname.startsWith("$")) {
      const name = propname.substr(1);
      args[name] = propval;
    } else if (Object(external_React_["isValidElement"])(propval)) {
      // We'll try to match localNames of elements found in the translation with
      // names of elements passed as props. localNames are always lowercase.
      const name = propname.toLowerCase();
      elems[name] = propval;
    }
  }

  return [args, elems];
}
/*
 * The `Localized` class renders its child with translated props and children.
 *
 *     <Localized id="hello-world">
 *         <p>{'Hello, world!'}</p>
 *     </Localized>
 *
 * The `id` prop should be the unique identifier of the translation.  Any
 * attributes found in the translation will be applied to the wrapped element.
 *
 * Arguments to the translation can be passed as `$`-prefixed props on
 * `Localized`.
 *
 *     <Localized id="hello-world" $username={name}>
 *         <p>{'Hello, { $username }!'}</p>
 *     </Localized>
 *
 *  It's recommended that the contents of the wrapped component be a string
 *  expression.  The string will be used as the ultimate fallback if no
 *  translation is available.  It also makes it easy to grep for strings in the
 *  source code.
 */


class localized_Localized extends external_React_["Component"] {
  componentDidMount() {
    const {
      l10n
    } = this.context;

    if (l10n) {
      l10n.subscribe(this);
    }
  }

  componentWillUnmount() {
    const {
      l10n
    } = this.context;

    if (l10n) {
      l10n.unsubscribe(this);
    }
  }
  /*
   * Rerender this component in a new language.
   */


  relocalize() {
    // When the `ReactLocalization`'s fallback chain changes, update the
    // component.
    this.forceUpdate();
  }

  render() {
    const {
      l10n,
      parseMarkup
    } = this.context;
    const {
      id,
      attrs,
      children: elem
    } = this.props; // Validate that the child element isn't an array

    if (Array.isArray(elem)) {
      throw new Error("<Localized/> expected to receive a single " + "React node child");
    }

    if (!l10n) {
      // Use the wrapped component as fallback.
      return elem;
    }

    const bundle = l10n.getBundle(id);

    if (bundle === null) {
      // Use the wrapped component as fallback.
      return elem;
    }

    const msg = bundle.getMessage(id);
    const [args, elems] = toArguments(this.props);
    const messageValue = bundle.format(msg, args); // Check if the fallback is a valid element -- if not then it's not
    // markup (e.g. nothing or a fallback string) so just use the
    // formatted message value

    if (!Object(external_React_["isValidElement"])(elem)) {
      return messageValue;
    } // The default is to forbid all message attributes. If the attrs prop exists
    // on the Localized instance, only set message attributes which have been
    // explicitly allowed by the developer.


    if (attrs && msg.attrs) {
      var localizedProps = {};

      for (const [name, allowed] of Object.entries(attrs)) {
        if (allowed && msg.attrs.hasOwnProperty(name)) {
          localizedProps[name] = bundle.format(msg.attrs[name], args);
        }
      }
    } // If the wrapped component is a known void element, explicitly dismiss the
    // message value and do not pass it to cloneElement in order to avoid the
    // "void element tags must neither have `children` nor use
    // `dangerouslySetInnerHTML`" error.


    if (elem.type in vendor_voidElementTags) {
      return Object(external_React_["cloneElement"])(elem, localizedProps);
    } // If the message has a null value, we're only interested in its attributes.
    // Do not pass the null value to cloneElement as it would nuke all children
    // of the wrapped component.


    if (messageValue === null) {
      return Object(external_React_["cloneElement"])(elem, localizedProps);
    } // If the message value doesn't contain any markup nor any HTML entities,
    // insert it as the only child of the wrapped component.


    if (!reMarkup.test(messageValue)) {
      return Object(external_React_["cloneElement"])(elem, localizedProps, messageValue);
    } // If the message contains markup, parse it and try to match the children
    // found in the translation with the props passed to this Localized.


    const translationNodes = parseMarkup(messageValue);
    const translatedChildren = translationNodes.map(childNode => {
      if (childNode.nodeType === childNode.TEXT_NODE) {
        return childNode.textContent;
      } // If the child is not expected just take its textContent.


      if (!elems.hasOwnProperty(childNode.localName)) {
        return childNode.textContent;
      }

      const sourceChild = elems[childNode.localName]; // If the element passed as a prop to <Localized> is a known void element,
      // explicitly dismiss any textContent which might have accidentally been
      // defined in the translation to prevent the "void element tags must not
      // have children" error.

      if (sourceChild.type in vendor_voidElementTags) {
        return sourceChild;
      } // TODO Protect contents of elements wrapped in <Localized>
      // https://github.com/projectfluent/fluent.js/issues/184
      // TODO  Control localizable attributes on elements passed as props
      // https://github.com/projectfluent/fluent.js/issues/185


      return Object(external_React_["cloneElement"])(sourceChild, null, childNode.textContent);
    });
    return Object(external_React_["cloneElement"])(elem, localizedProps, ...translatedChildren);
  }

}
localized_Localized.contextTypes = {
  l10n: isReactLocalization,
  parseMarkup: external_PropTypes_default.a.func
};
localized_Localized.propTypes = {
  children: external_PropTypes_default.a.node
};
// CONCATENATED MODULE: ./node_modules/fluent-react/src/index.js
/* concated harmony reexport LocalizationProvider */__webpack_require__.d(__webpack_exports__, "LocalizationProvider", function() { return provider_LocalizationProvider; });
/* concated harmony reexport withLocalization */__webpack_require__.d(__webpack_exports__, "withLocalization", function() { return withLocalization; });
/* concated harmony reexport Localized */__webpack_require__.d(__webpack_exports__, "Localized", function() { return localized_Localized; });
/* concated harmony reexport ReactLocalization */__webpack_require__.d(__webpack_exports__, "ReactLocalization", function() { return localization_ReactLocalization; });
/* concated harmony reexport isReactLocalization */__webpack_require__.d(__webpack_exports__, "isReactLocalization", function() { return isReactLocalization; });
/*
 * @module fluent-react
 * @overview
 *

 * `fluent-react` provides React bindings for Fluent.  It takes advantage of
 * React's Components system and the virtual DOM.  Translations are exposed to
 * components via the provider pattern.
 *
 *     <LocalizationProvider bundles={…}>
 *         <Localized id="hello-world">
 *             <p>{'Hello, world!'}</p>
 *         </Localized>
 *     </LocalizationProvider>
 *
 * Consult the documentation of the `LocalizationProvider` and the `Localized`
 * components for more information.
 */





/***/ }),
/* 51 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

/***/ }),
/* 52 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);

// CONCATENATED MODULE: ./node_modules/fluent/src/types.js
/* global Intl */

/**
 * The `FluentType` class is the base of Fluent's type system.
 *
 * Fluent types wrap JavaScript values and store additional configuration for
 * them, which can then be used in the `toString` method together with a proper
 * `Intl` formatter.
 */
class FluentType {
  /**
   * Create an `FluentType` instance.
   *
   * @param   {Any}    value - JavaScript value to wrap.
   * @param   {Object} opts  - Configuration.
   * @returns {FluentType}
   */
  constructor(value, opts) {
    this.value = value;
    this.opts = opts;
  }
  /**
   * Unwrap the raw value stored by this `FluentType`.
   *
   * @returns {Any}
   */


  valueOf() {
    return this.value;
  }
  /**
   * Format this instance of `FluentType` to a string.
   *
   * Formatted values are suitable for use outside of the `FluentBundle`.
   * This method can use `Intl` formatters memoized by the `FluentBundle`
   * instance passed as an argument.
   *
   * @param   {FluentBundle} [bundle]
   * @returns {string}
   */


  toString() {
    throw new Error("Subclasses of FluentType must implement toString.");
  }

}
class FluentNone extends FluentType {
  toString() {
    return this.value || "???";
  }

}
class FluentNumber extends FluentType {
  constructor(value, opts) {
    super(parseFloat(value), opts);
  }

  toString(bundle) {
    try {
      const nf = bundle._memoizeIntlObject(Intl.NumberFormat, this.opts);

      return nf.format(this.value);
    } catch (e) {
      // XXX Report the error.
      return this.value;
    }
  }

}
class FluentDateTime extends FluentType {
  constructor(value, opts) {
    super(new Date(value), opts);
  }

  toString(bundle) {
    try {
      const dtf = bundle._memoizeIntlObject(Intl.DateTimeFormat, this.opts);

      return dtf.format(this.value);
    } catch (e) {
      // XXX Report the error.
      return this.value;
    }
  }

}
// CONCATENATED MODULE: ./node_modules/fluent/src/builtins.js
/**
 * @overview
 *
 * The FTL resolver ships with a number of functions built-in.
 *
 * Each function take two arguments:
 *   - args - an array of positional args
 *   - opts - an object of key-value args
 *
 * Arguments to functions are guaranteed to already be instances of
 * `FluentType`.  Functions must return `FluentType` objects as well.
 */

/* harmony default export */ var builtins = ({
  "NUMBER": ([arg], opts) => new FluentNumber(arg.valueOf(), merge(arg.opts, opts)),
  "DATETIME": ([arg], opts) => new FluentDateTime(arg.valueOf(), merge(arg.opts, opts))
});

function merge(argopts, opts) {
  return Object.assign({}, argopts, values(opts));
}

function values(opts) {
  const unwrapped = {};

  for (const [name, opt] of Object.entries(opts)) {
    unwrapped[name] = opt.valueOf();
  }

  return unwrapped;
}
// CONCATENATED MODULE: ./node_modules/fluent/src/resolver.js
/* global Intl */

/**
 * @overview
 *
 * The role of the Fluent resolver is to format a translation object to an
 * instance of `FluentType` or an array of instances.
 *
 * Translations can contain references to other messages or variables,
 * conditional logic in form of select expressions, traits which describe their
 * grammatical features, and can use Fluent builtins which make use of the
 * `Intl` formatters to format numbers, dates, lists and more into the
 * bundle's language. See the documentation of the Fluent syntax for more
 * information.
 *
 * In case of errors the resolver will try to salvage as much of the
 * translation as possible.  In rare situations where the resolver didn't know
 * how to recover from an error it will return an instance of `FluentNone`.
 *
 * All expressions resolve to an instance of `FluentType`. The caller should
 * use the `toString` method to convert the instance to a native value.
 *
 * All functions in this file pass around a special object called `scope`.
 * This object stores a set of elements used by all resolve functions:
 *
 *  * {FluentBundle} bundle
 *      bundle for which the given resolution is happening
 *  * {Object} args
 *      list of developer provided arguments that can be used
 *  * {Array} errors
 *      list of errors collected while resolving
 *  * {WeakSet} dirty
 *      Set of patterns already encountered during this resolution.
 *      This is used to prevent cyclic resolutions.
 */

 // Prevent expansion of too long placeables.

const MAX_PLACEABLE_LENGTH = 2500; // Unicode bidi isolation characters.

const FSI = "\u2068";
const PDI = "\u2069"; // Helper: match a variant key to the given selector.

function resolver_match(bundle, selector, key) {
  if (key === selector) {
    // Both are strings.
    return true;
  } // XXX Consider comparing options too, e.g. minimumFractionDigits.


  if (key instanceof FluentNumber && selector instanceof FluentNumber && key.value === selector.value) {
    return true;
  }

  if (selector instanceof FluentNumber && typeof key === "string") {
    let category = bundle._memoizeIntlObject(Intl.PluralRules, selector.opts).select(selector.value);

    if (key === category) {
      return true;
    }
  }

  return false;
} // Helper: resolve the default variant from a list of variants.


function getDefault(scope, variants, star) {
  if (variants[star]) {
    return Type(scope, variants[star]);
  }

  scope.errors.push(new RangeError("No default"));
  return new FluentNone();
} // Helper: resolve arguments to a call expression.


function getArguments(scope, args) {
  const positional = [];
  const named = {};

  for (const arg of args) {
    if (arg.type === "narg") {
      named[arg.name] = Type(scope, arg.value);
    } else {
      positional.push(Type(scope, arg));
    }
  }

  return [positional, named];
} // Resolve an expression to a Fluent type.


function Type(scope, expr) {
  // A fast-path for strings which are the most common case. Since they
  // natively have the `toString` method they can be used as if they were
  // a FluentType instance without incurring the cost of creating one.
  if (typeof expr === "string") {
    return scope.bundle._transform(expr);
  } // A fast-path for `FluentNone` which doesn't require any additional logic.


  if (expr instanceof FluentNone) {
    return expr;
  } // The Runtime AST (Entries) encodes patterns (complex strings with
  // placeables) as Arrays.


  if (Array.isArray(expr)) {
    return Pattern(scope, expr);
  }

  switch (expr.type) {
    case "str":
      return expr.value;

    case "num":
      return new FluentNumber(expr.value, {
        minimumFractionDigits: expr.precision
      });

    case "var":
      return VariableReference(scope, expr);

    case "mesg":
      return MessageReference(scope, expr);

    case "term":
      return TermReference(scope, expr);

    case "func":
      return FunctionReference(scope, expr);

    case "select":
      return SelectExpression(scope, expr);

    case undefined:
      {
        // If it's a node with a value, resolve the value.
        if (expr.value !== null && expr.value !== undefined) {
          return Type(scope, expr.value);
        }

        scope.errors.push(new RangeError("No value"));
        return new FluentNone();
      }

    default:
      return new FluentNone();
  }
} // Resolve a reference to a variable.


function VariableReference(scope, {
  name
}) {
  if (!scope.args || !scope.args.hasOwnProperty(name)) {
    if (scope.insideTermReference === false) {
      scope.errors.push(new ReferenceError(`Unknown variable: ${name}`));
    }

    return new FluentNone(`$${name}`);
  }

  const arg = scope.args[name]; // Return early if the argument already is an instance of FluentType.

  if (arg instanceof FluentType) {
    return arg;
  } // Convert the argument to a Fluent type.


  switch (typeof arg) {
    case "string":
      return arg;

    case "number":
      return new FluentNumber(arg);

    case "object":
      if (arg instanceof Date) {
        return new FluentDateTime(arg);
      }

    default:
      scope.errors.push(new TypeError(`Unsupported variable type: ${name}, ${typeof arg}`));
      return new FluentNone(`$${name}`);
  }
} // Resolve a reference to another message.


function MessageReference(scope, {
  name,
  attr
}) {
  const message = scope.bundle._messages.get(name);

  if (!message) {
    const err = new ReferenceError(`Unknown message: ${name}`);
    scope.errors.push(err);
    return new FluentNone(name);
  }

  if (attr) {
    const attribute = message.attrs && message.attrs[attr];

    if (attribute) {
      return Type(scope, attribute);
    }

    scope.errors.push(new ReferenceError(`Unknown attribute: ${attr}`));
    return Type(scope, message);
  }

  return Type(scope, message);
} // Resolve a call to a Term with key-value arguments.


function TermReference(scope, {
  name,
  attr,
  args
}) {
  const id = `-${name}`;

  const term = scope.bundle._terms.get(id);

  if (!term) {
    const err = new ReferenceError(`Unknown term: ${id}`);
    scope.errors.push(err);
    return new FluentNone(id);
  } // Every TermReference has its own args.


  const [, keyargs] = getArguments(scope, args);
  const local = { ...scope,
    args: keyargs,
    insideTermReference: true
  };

  if (attr) {
    const attribute = term.attrs && term.attrs[attr];

    if (attribute) {
      return Type(local, attribute);
    }

    scope.errors.push(new ReferenceError(`Unknown attribute: ${attr}`));
    return Type(local, term);
  }

  return Type(local, term);
} // Resolve a call to a Function with positional and key-value arguments.


function FunctionReference(scope, {
  name,
  args
}) {
  // Some functions are built-in. Others may be provided by the runtime via
  // the `FluentBundle` constructor.
  const func = scope.bundle._functions[name] || builtins[name];

  if (!func) {
    scope.errors.push(new ReferenceError(`Unknown function: ${name}()`));
    return new FluentNone(`${name}()`);
  }

  if (typeof func !== "function") {
    scope.errors.push(new TypeError(`Function ${name}() is not callable`));
    return new FluentNone(`${name}()`);
  }

  try {
    return func(...getArguments(scope, args));
  } catch (e) {
    // XXX Report errors.
    return new FluentNone();
  }
} // Resolve a select expression to the member object.


function SelectExpression(scope, {
  selector,
  variants,
  star
}) {
  let sel = Type(scope, selector);

  if (sel instanceof FluentNone) {
    const variant = getDefault(scope, variants, star);
    return Type(scope, variant);
  } // Match the selector against keys of each variant, in order.


  for (const variant of variants) {
    const key = Type(scope, variant.key);

    if (resolver_match(scope.bundle, sel, key)) {
      return Type(scope, variant);
    }
  }

  const variant = getDefault(scope, variants, star);
  return Type(scope, variant);
} // Resolve a pattern (a complex string with placeables).


function Pattern(scope, ptn) {
  if (scope.dirty.has(ptn)) {
    scope.errors.push(new RangeError("Cyclic reference"));
    return new FluentNone();
  } // Tag the pattern as dirty for the purpose of the current resolution.


  scope.dirty.add(ptn);
  const result = []; // Wrap interpolations with Directional Isolate Formatting characters
  // only when the pattern has more than one element.

  const useIsolating = scope.bundle._useIsolating && ptn.length > 1;

  for (const elem of ptn) {
    if (typeof elem === "string") {
      result.push(scope.bundle._transform(elem));
      continue;
    }

    const part = Type(scope, elem).toString(scope.bundle);

    if (useIsolating) {
      result.push(FSI);
    }

    if (part.length > MAX_PLACEABLE_LENGTH) {
      scope.errors.push(new RangeError("Too many characters in placeable " + `(${part.length}, max allowed is ${MAX_PLACEABLE_LENGTH})`));
      result.push(part.slice(MAX_PLACEABLE_LENGTH));
    } else {
      result.push(part);
    }

    if (useIsolating) {
      result.push(PDI);
    }
  }

  scope.dirty.delete(ptn);
  return result.join("");
}
/**
 * Format a translation into a string.
 *
 * @param   {FluentBundle} bundle
 *    A FluentBundle instance which will be used to resolve the
 *    contextual information of the message.
 * @param   {Object}         args
 *    List of arguments provided by the developer which can be accessed
 *    from the message.
 * @param   {Object}         message
 *    An object with the Message to be resolved.
 * @param   {Array}          errors
 *    An error array that any encountered errors will be appended to.
 * @returns {FluentType}
 */


function resolve(bundle, args, message, errors = []) {
  const scope = {
    bundle,
    args,
    errors,
    dirty: new WeakSet(),
    // TermReferences are resolved in a new scope.
    insideTermReference: false
  };
  return Type(scope, message).toString(bundle);
}
// CONCATENATED MODULE: ./node_modules/fluent/src/error.js
class FluentError extends Error {}
// CONCATENATED MODULE: ./node_modules/fluent/src/resource.js
 // This regex is used to iterate through the beginnings of messages and terms.
// With the /m flag, the ^ matches at the beginning of every line.

const RE_MESSAGE_START = /^(-?[a-zA-Z][\w-]*) *= */mg; // Both Attributes and Variants are parsed in while loops. These regexes are
// used to break out of them.

const RE_ATTRIBUTE_START = /\.([a-zA-Z][\w-]*) *= */y;
const RE_VARIANT_START = /\*?\[/y;
const RE_NUMBER_LITERAL = /(-?[0-9]+(?:\.([0-9]+))?)/y;
const RE_IDENTIFIER = /([a-zA-Z][\w-]*)/y;
const RE_REFERENCE = /([$-])?([a-zA-Z][\w-]*)(?:\.([a-zA-Z][\w-]*))?/y;
const RE_FUNCTION_NAME = /^[A-Z][A-Z0-9_-]*$/; // A "run" is a sequence of text or string literal characters which don't
// require any special handling. For TextElements such special characters are: {
// (starts a placeable), and line breaks which require additional logic to check
// if the next line is indented. For StringLiterals they are: \ (starts an
// escape sequence), " (ends the literal), and line breaks which are not allowed
// in StringLiterals. Note that string runs may be empty; text runs may not.

const RE_TEXT_RUN = /([^{}\n\r]+)/y;
const RE_STRING_RUN = /([^\\"\n\r]*)/y; // Escape sequences.

const RE_STRING_ESCAPE = /\\([\\"])/y;
const RE_UNICODE_ESCAPE = /\\u([a-fA-F0-9]{4})|\\U([a-fA-F0-9]{6})/y; // Used for trimming TextElements and indents.

const RE_LEADING_NEWLINES = /^\n+/;
const RE_TRAILING_SPACES = / +$/; // Used in makeIndent to strip spaces from blank lines and normalize CRLF to LF.

const RE_BLANK_LINES = / *\r?\n/g; // Used in makeIndent to measure the indentation.

const RE_INDENT = /( *)$/; // Common tokens.

const TOKEN_BRACE_OPEN = /{\s*/y;
const TOKEN_BRACE_CLOSE = /\s*}/y;
const TOKEN_BRACKET_OPEN = /\[\s*/y;
const TOKEN_BRACKET_CLOSE = /\s*] */y;
const TOKEN_PAREN_OPEN = /\s*\(\s*/y;
const TOKEN_ARROW = /\s*->\s*/y;
const TOKEN_COLON = /\s*:\s*/y; // Note the optional comma. As a deviation from the Fluent EBNF, the parser
// doesn't enforce commas between call arguments.

const TOKEN_COMMA = /\s*,?\s*/y;
const TOKEN_BLANK = /\s+/y; // Maximum number of placeables in a single Pattern to protect against Quadratic
// Blowup attacks. See https://msdn.microsoft.com/en-us/magazine/ee335713.aspx.

const MAX_PLACEABLES = 100;
/**
 * Fluent Resource is a structure storing a map of parsed localization entries.
 */

class resource_FluentResource extends Map {
  /**
   * Create a new FluentResource from Fluent code.
   */
  static fromString(source) {
    RE_MESSAGE_START.lastIndex = 0;
    let resource = new this();
    let cursor = 0; // Iterate over the beginnings of messages and terms to efficiently skip
    // comments and recover from errors.

    while (true) {
      let next = RE_MESSAGE_START.exec(source);

      if (next === null) {
        break;
      }

      cursor = RE_MESSAGE_START.lastIndex;

      try {
        resource.set(next[1], parseMessage());
      } catch (err) {
        if (err instanceof FluentError) {
          // Don't report any Fluent syntax errors. Skip directly to the
          // beginning of the next message or term.
          continue;
        }

        throw err;
      }
    }

    return resource; // The parser implementation is inlined below for performance reasons.
    // The parser focuses on minimizing the number of false negatives at the
    // expense of increasing the risk of false positives. In other words, it
    // aims at parsing valid Fluent messages with a success rate of 100%, but it
    // may also parse a few invalid messages which the reference parser would
    // reject. The parser doesn't perform any validation and may produce entries
    // which wouldn't make sense in the real world. For best results users are
    // advised to validate translations with the fluent-syntax parser
    // pre-runtime.
    // The parser makes an extensive use of sticky regexes which can be anchored
    // to any offset of the source string without slicing it. Errors are thrown
    // to bail out of parsing of ill-formed messages.

    function test(re) {
      re.lastIndex = cursor;
      return re.test(source);
    } // Advance the cursor by the char if it matches. May be used as a predicate
    // (was the match found?) or, if errorClass is passed, as an assertion.


    function consumeChar(char, errorClass) {
      if (source[cursor] === char) {
        cursor++;
        return true;
      }

      if (errorClass) {
        throw new errorClass(`Expected ${char}`);
      }

      return false;
    } // Advance the cursor by the token if it matches. May be used as a predicate
    // (was the match found?) or, if errorClass is passed, as an assertion.


    function consumeToken(re, errorClass) {
      if (test(re)) {
        cursor = re.lastIndex;
        return true;
      }

      if (errorClass) {
        throw new errorClass(`Expected ${re.toString()}`);
      }

      return false;
    } // Execute a regex, advance the cursor, and return all capture groups.


    function match(re) {
      re.lastIndex = cursor;
      let result = re.exec(source);

      if (result === null) {
        throw new FluentError(`Expected ${re.toString()}`);
      }

      cursor = re.lastIndex;
      return result;
    } // Execute a regex, advance the cursor, and return the capture group.


    function match1(re) {
      return match(re)[1];
    }

    function parseMessage() {
      let value = parsePattern();
      let attrs = parseAttributes();

      if (attrs === null) {
        if (value === null) {
          throw new FluentError("Expected message value or attributes");
        }

        return value;
      }

      return {
        value,
        attrs
      };
    }

    function parseAttributes() {
      let attrs = {};

      while (test(RE_ATTRIBUTE_START)) {
        let name = match1(RE_ATTRIBUTE_START);
        let value = parsePattern();

        if (value === null) {
          throw new FluentError("Expected attribute value");
        }

        attrs[name] = value;
      }

      return Object.keys(attrs).length > 0 ? attrs : null;
    }

    function parsePattern() {
      // First try to parse any simple text on the same line as the id.
      if (test(RE_TEXT_RUN)) {
        var first = match1(RE_TEXT_RUN);
      } // If there's a placeable on the first line, parse a complex pattern.


      if (source[cursor] === "{" || source[cursor] === "}") {
        // Re-use the text parsed above, if possible.
        return parsePatternElements(first ? [first] : [], Infinity);
      } // RE_TEXT_VALUE stops at newlines. Only continue parsing the pattern if
      // what comes after the newline is indented.


      let indent = parseIndent();

      if (indent) {
        if (first) {
          // If there's text on the first line, the blank block is part of the
          // translation content in its entirety.
          return parsePatternElements([first, indent], indent.length);
        } // Otherwise, we're dealing with a block pattern, i.e. a pattern which
        // starts on a new line. Discrad the leading newlines but keep the
        // inline indent; it will be used by the dedentation logic.


        indent.value = trim(indent.value, RE_LEADING_NEWLINES);
        return parsePatternElements([indent], indent.length);
      }

      if (first) {
        // It was just a simple inline text after all.
        return trim(first, RE_TRAILING_SPACES);
      }

      return null;
    } // Parse a complex pattern as an array of elements.


    function parsePatternElements(elements = [], commonIndent) {
      let placeableCount = 0;

      while (true) {
        if (test(RE_TEXT_RUN)) {
          elements.push(match1(RE_TEXT_RUN));
          continue;
        }

        if (source[cursor] === "{") {
          if (++placeableCount > MAX_PLACEABLES) {
            throw new FluentError("Too many placeables");
          }

          elements.push(parsePlaceable());
          continue;
        }

        if (source[cursor] === "}") {
          throw new FluentError("Unbalanced closing brace");
        }

        let indent = parseIndent();

        if (indent) {
          elements.push(indent);
          commonIndent = Math.min(commonIndent, indent.length);
          continue;
        }

        break;
      }

      let lastIndex = elements.length - 1; // Trim the trailing spaces in the last element if it's a TextElement.

      if (typeof elements[lastIndex] === "string") {
        elements[lastIndex] = trim(elements[lastIndex], RE_TRAILING_SPACES);
      }

      let baked = [];

      for (let element of elements) {
        if (element.type === "indent") {
          // Dedent indented lines by the maximum common indent.
          element = element.value.slice(0, element.value.length - commonIndent);
        } else if (element.type === "str") {
          // Optimize StringLiterals into their value.
          element = element.value;
        }

        if (element) {
          baked.push(element);
        }
      }

      return baked;
    }

    function parsePlaceable() {
      consumeToken(TOKEN_BRACE_OPEN, FluentError);
      let selector = parseInlineExpression();

      if (consumeToken(TOKEN_BRACE_CLOSE)) {
        return selector;
      }

      if (consumeToken(TOKEN_ARROW)) {
        let variants = parseVariants();
        consumeToken(TOKEN_BRACE_CLOSE, FluentError);
        return {
          type: "select",
          selector,
          ...variants
        };
      }

      throw new FluentError("Unclosed placeable");
    }

    function parseInlineExpression() {
      if (source[cursor] === "{") {
        // It's a nested placeable.
        return parsePlaceable();
      }

      if (test(RE_REFERENCE)) {
        let [, sigil, name, attr = null] = match(RE_REFERENCE);

        if (sigil === "$") {
          return {
            type: "var",
            name
          };
        }

        if (consumeToken(TOKEN_PAREN_OPEN)) {
          let args = parseArguments();

          if (sigil === "-") {
            // A parameterized term: -term(...).
            return {
              type: "term",
              name,
              attr,
              args
            };
          }

          if (RE_FUNCTION_NAME.test(name)) {
            return {
              type: "func",
              name,
              args
            };
          }

          throw new FluentError("Function names must be all upper-case");
        }

        if (sigil === "-") {
          // A non-parameterized term: -term.
          return {
            type: "term",
            name,
            attr,
            args: []
          };
        }

        return {
          type: "mesg",
          name,
          attr
        };
      }

      return parseLiteral();
    }

    function parseArguments() {
      let args = [];

      while (true) {
        switch (source[cursor]) {
          case ")":
            // End of the argument list.
            cursor++;
            return args;

          case undefined:
            // EOF
            throw new FluentError("Unclosed argument list");
        }

        args.push(parseArgument()); // Commas between arguments are treated as whitespace.

        consumeToken(TOKEN_COMMA);
      }
    }

    function parseArgument() {
      let expr = parseInlineExpression();

      if (expr.type !== "mesg") {
        return expr;
      }

      if (consumeToken(TOKEN_COLON)) {
        // The reference is the beginning of a named argument.
        return {
          type: "narg",
          name: expr.name,
          value: parseLiteral()
        };
      } // It's a regular message reference.


      return expr;
    }

    function parseVariants() {
      let variants = [];
      let count = 0;
      let star;

      while (test(RE_VARIANT_START)) {
        if (consumeChar("*")) {
          star = count;
        }

        let key = parseVariantKey();
        let value = parsePattern();

        if (value === null) {
          throw new FluentError("Expected variant value");
        }

        variants[count++] = {
          key,
          value
        };
      }

      if (count === 0) {
        return null;
      }

      if (star === undefined) {
        throw new FluentError("Expected default variant");
      }

      return {
        variants,
        star
      };
    }

    function parseVariantKey() {
      consumeToken(TOKEN_BRACKET_OPEN, FluentError);
      let key = test(RE_NUMBER_LITERAL) ? parseNumberLiteral() : match1(RE_IDENTIFIER);
      consumeToken(TOKEN_BRACKET_CLOSE, FluentError);
      return key;
    }

    function parseLiteral() {
      if (test(RE_NUMBER_LITERAL)) {
        return parseNumberLiteral();
      }

      if (source[cursor] === "\"") {
        return parseStringLiteral();
      }

      throw new FluentError("Invalid expression");
    }

    function parseNumberLiteral() {
      let [, value, fraction = ""] = match(RE_NUMBER_LITERAL);
      let precision = fraction.length;
      return {
        type: "num",
        value: parseFloat(value),
        precision
      };
    }

    function parseStringLiteral() {
      consumeChar("\"", FluentError);
      let value = "";

      while (true) {
        value += match1(RE_STRING_RUN);

        if (source[cursor] === "\\") {
          value += parseEscapeSequence();
          continue;
        }

        if (consumeChar("\"")) {
          return {
            type: "str",
            value
          };
        } // We've reached an EOL of EOF.


        throw new FluentError("Unclosed string literal");
      }
    } // Unescape known escape sequences.


    function parseEscapeSequence() {
      if (test(RE_STRING_ESCAPE)) {
        return match1(RE_STRING_ESCAPE);
      }

      if (test(RE_UNICODE_ESCAPE)) {
        let [, codepoint4, codepoint6] = match(RE_UNICODE_ESCAPE);
        let codepoint = parseInt(codepoint4 || codepoint6, 16);
        return codepoint <= 0xD7FF || 0xE000 <= codepoint // It's a Unicode scalar value.
        ? String.fromCodePoint(codepoint) // Lonely surrogates can cause trouble when the parsing result is
        // saved using UTF-8. Use U+FFFD REPLACEMENT CHARACTER instead.
        : "�";
      }

      throw new FluentError("Unknown escape sequence");
    } // Parse blank space. Return it if it looks like indent before a pattern
    // line. Skip it othwerwise.


    function parseIndent() {
      let start = cursor;
      consumeToken(TOKEN_BLANK); // Check the first non-blank character after the indent.

      switch (source[cursor]) {
        case ".":
        case "[":
        case "*":
        case "}":
        case undefined:
          // EOF
          // A special character. End the Pattern.
          return false;

        case "{":
          // Placeables don't require indentation (in EBNF: block-placeable).
          // Continue the Pattern.
          return makeIndent(source.slice(start, cursor));
      } // If the first character on the line is not one of the special characters
      // listed above, it's a regular text character. Check if there's at least
      // one space of indent before it.


      if (source[cursor - 1] === " ") {
        // It's an indented text character (in EBNF: indented-char). Continue
        // the Pattern.
        return makeIndent(source.slice(start, cursor));
      } // A not-indented text character is likely the identifier of the next
      // message. End the Pattern.


      return false;
    } // Trim blanks in text according to the given regex.


    function trim(text, re) {
      return text.replace(re, "");
    } // Normalize a blank block and extract the indent details.


    function makeIndent(blank) {
      let value = blank.replace(RE_BLANK_LINES, "\n");
      let length = RE_INDENT.exec(blank)[1].length;
      return {
        type: "indent",
        value,
        length
      };
    }
  }

}
// CONCATENATED MODULE: ./node_modules/fluent/src/bundle.js


/**
 * Message bundles are single-language stores of translations.  They are
 * responsible for parsing translation resources in the Fluent syntax and can
 * format translation units (entities) to strings.
 *
 * Always use `FluentBundle.format` to retrieve translation units from a
 * bundle. Translations can contain references to other entities or variables,
 * conditional logic in form of select expressions, traits which describe their
 * grammatical features, and can use Fluent builtins which make use of the
 * `Intl` formatters to format numbers, dates, lists and more into the
 * bundle's language. See the documentation of the Fluent syntax for more
 * information.
 */

class bundle_FluentBundle {
  /**
   * Create an instance of `FluentBundle`.
   *
   * The `locales` argument is used to instantiate `Intl` formatters used by
   * translations.  The `options` object can be used to configure the bundle.
   *
   * Examples:
   *
   *     const bundle = new FluentBundle(locales);
   *
   *     const bundle = new FluentBundle(locales, { useIsolating: false });
   *
   *     const bundle = new FluentBundle(locales, {
   *       useIsolating: true,
   *       functions: {
   *         NODE_ENV: () => process.env.NODE_ENV
   *       }
   *     });
   *
   * Available options:
   *
   *   - `functions` - an object of additional functions available to
   *                   translations as builtins.
   *
   *   - `useIsolating` - boolean specifying whether to use Unicode isolation
   *                    marks (FSI, PDI) for bidi interpolations.
   *                    Default: true
   *
   *   - `transform` - a function used to transform string parts of patterns.
   *
   * @param   {string|Array<string>} locales - Locale or locales of the bundle
   * @param   {Object} [options]
   * @returns {FluentBundle}
   */
  constructor(locales, {
    functions = {},
    useIsolating = true,
    transform = v => v
  } = {}) {
    this.locales = Array.isArray(locales) ? locales : [locales];
    this._terms = new Map();
    this._messages = new Map();
    this._functions = functions;
    this._useIsolating = useIsolating;
    this._transform = transform;
    this._intls = new WeakMap();
  }
  /*
   * Return an iterator over public `[id, message]` pairs.
   *
   * @returns {Iterator}
   */


  get messages() {
    return this._messages[Symbol.iterator]();
  }
  /*
   * Check if a message is present in the bundle.
   *
   * @param {string} id - The identifier of the message to check.
   * @returns {bool}
   */


  hasMessage(id) {
    return this._messages.has(id);
  }
  /*
   * Return the internal representation of a message.
   *
   * The internal representation should only be used as an argument to
   * `FluentBundle.format`.
   *
   * @param {string} id - The identifier of the message to check.
   * @returns {Any}
   */


  getMessage(id) {
    return this._messages.get(id);
  }
  /**
   * Add a translation resource to the bundle.
   *
   * The translation resource must use the Fluent syntax.  It will be parsed by
   * the bundle and each translation unit (message) will be available in the
   * bundle by its identifier.
   *
   *     bundle.addMessages('foo = Foo');
   *     bundle.getMessage('foo');
   *
   *     // Returns a raw representation of the 'foo' message.
   *
   *     bundle.addMessages('bar = Bar');
   *     bundle.addMessages('bar = Newbar', { allowOverrides: true });
   *     bundle.getMessage('bar');
   *
   *     // Returns a raw representation of the 'bar' message: Newbar.
   *
   * Parsed entities should be formatted with the `format` method in case they
   * contain logic (references, select expressions etc.).
   *
   * Available options:
   *
   *   - `allowOverrides` - boolean specifying whether it's allowed to override
   *                      an existing message or term with a new value.
   *                      Default: false
   *
   * @param   {string} source - Text resource with translations.
   * @param   {Object} [options]
   * @returns {Array<Error>}
   */


  addMessages(source, options) {
    const res = resource_FluentResource.fromString(source);
    return this.addResource(res, options);
  }
  /**
   * Add a translation resource to the bundle.
   *
   * The translation resource must be an instance of FluentResource,
   * e.g. parsed by `FluentResource.fromString`.
   *
   *     let res = FluentResource.fromString("foo = Foo");
   *     bundle.addResource(res);
   *     bundle.getMessage('foo');
   *
   *     // Returns a raw representation of the 'foo' message.
   *
   *     let res = FluentResource.fromString("bar = Bar");
   *     bundle.addResource(res);
   *     res = FluentResource.fromString("bar = Newbar");
   *     bundle.addResource(res, { allowOverrides: true });
   *     bundle.getMessage('bar');
   *
   *     // Returns a raw representation of the 'bar' message: Newbar.
   *
   * Parsed entities should be formatted with the `format` method in case they
   * contain logic (references, select expressions etc.).
   *
   * Available options:
   *
   *   - `allowOverrides` - boolean specifying whether it's allowed to override
   *                      an existing message or term with a new value.
   *                      Default: false
   *
   * @param   {FluentResource} res - FluentResource object.
   * @param   {Object} [options]
   * @returns {Array<Error>}
   */


  addResource(res, {
    allowOverrides = false
  } = {}) {
    const errors = [];

    for (const [id, value] of res) {
      if (id.startsWith("-")) {
        // Identifiers starting with a dash (-) define terms. Terms are private
        // and cannot be retrieved from FluentBundle.
        if (allowOverrides === false && this._terms.has(id)) {
          errors.push(`Attempt to override an existing term: "${id}"`);
          continue;
        }

        this._terms.set(id, value);
      } else {
        if (allowOverrides === false && this._messages.has(id)) {
          errors.push(`Attempt to override an existing message: "${id}"`);
          continue;
        }

        this._messages.set(id, value);
      }
    }

    return errors;
  }
  /**
   * Format a message to a string or null.
   *
   * Format a raw `message` from the bundle into a string (or a null if it has
   * a null value).  `args` will be used to resolve references to variables
   * passed as arguments to the translation.
   *
   * In case of errors `format` will try to salvage as much of the translation
   * as possible and will still return a string.  For performance reasons, the
   * encountered errors are not returned but instead are appended to the
   * `errors` array passed as the third argument.
   *
   *     const errors = [];
   *     bundle.addMessages('hello = Hello, { $name }!');
   *     const hello = bundle.getMessage('hello');
   *     bundle.format(hello, { name: 'Jane' }, errors);
   *
   *     // Returns 'Hello, Jane!' and `errors` is empty.
   *
   *     bundle.format(hello, undefined, errors);
   *
   *     // Returns 'Hello, name!' and `errors` is now:
   *
   *     [<ReferenceError: Unknown variable: name>]
   *
   * @param   {Object | string}    message
   * @param   {Object | undefined} args
   * @param   {Array}              errors
   * @returns {?string}
   */


  format(message, args, errors) {
    // optimize entities which are simple strings with no attributes
    if (typeof message === "string") {
      return this._transform(message);
    } // optimize entities with null values


    if (message === null || message.value === null) {
      return null;
    } // optimize simple-string entities with attributes


    if (typeof message.value === "string") {
      return this._transform(message.value);
    }

    return resolve(this, args, message, errors);
  }

  _memoizeIntlObject(ctor, opts) {
    const cache = this._intls.get(ctor) || {};
    const id = JSON.stringify(opts);

    if (!cache[id]) {
      cache[id] = new ctor(this.locales, opts);

      this._intls.set(ctor, cache);
    }

    return cache[id];
  }

}
// CONCATENATED MODULE: ./node_modules/fluent/src/util.js
function nonBlank(line) {
  return !/^\s*$/.test(line);
}

function countIndent(line) {
  const [indent] = line.match(/^\s*/);
  return indent.length;
}
/**
 * Template literal tag for dedenting FTL code.
 *
 * Strip the common indent of non-blank lines. Remove blank lines.
 *
 * @param {Array<string>} strings
 */


function ftl(strings) {
  const [code] = strings;
  const lines = code.split("\n").filter(nonBlank);
  const indents = lines.map(countIndent);
  const common = Math.min(...indents);
  const indent = new RegExp(`^\\s{${common}}`);
  return lines.map(line => line.replace(indent, "")).join("\n");
}
// CONCATENATED MODULE: ./node_modules/fluent/src/index.js
/*
 * @module fluent
 * @overview
 *
 * `fluent` is a JavaScript implementation of Project Fluent, a localization
 * framework designed to unleash the expressive power of the natural language.
 *
 */





// CONCATENATED MODULE: ./content-src/asrouter/rich-text-strings.js
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "RICH_TEXT_KEYS", function() { return RICH_TEXT_KEYS; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "generateBundles", function() { return generateBundles; });

/**
 * Properties that allow rich text MUST be added to this list.
 *   key: the localization_id that should be used
 *   value: a property or array of properties on the message.content object
 */

const RICH_TEXT_CONFIG = {
  "text": ["text", "scene1_text"],
  "success_text": "success_text",
  "error_text": "error_text",
  "scene2_text": "scene2_text",
  "privacy_html": "scene2_privacy_html",
  "disclaimer_html": "scene2_disclaimer_html"
};
const RICH_TEXT_KEYS = Object.keys(RICH_TEXT_CONFIG);
/**
 * Generates an array of messages suitable for fluent's localization provider
 * including all needed strings for rich text.
 * @param {object} content A .content object from an ASR message (i.e. message.content)
 * @returns {FluentBundle[]} A array containing the fluent message context
 */

function generateBundles(content) {
  const bundle = new bundle_FluentBundle("en-US");
  RICH_TEXT_KEYS.forEach(key => {
    const attrs = RICH_TEXT_CONFIG[key];
    const attrsToTry = Array.isArray(attrs) ? [...attrs] : [attrs];
    let string = "";

    while (!string && attrsToTry.length) {
      const attr = attrsToTry.pop();
      string = content[attr];
    }

    bundle.addMessages(`${key} = ${string}`);
  });
  return [bundle];
}

/***/ }),
/* 53 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);

// EXTERNAL MODULE: ./common/Actions.jsm
var Actions = __webpack_require__(2);

// CONCATENATED MODULE: ./common/Dedupe.jsm
class Dedupe {
  constructor(createKey) {
    this.createKey = createKey || this.defaultCreateKey;
  }

  defaultCreateKey(item) {
    return item;
  }
  /**
   * Dedupe any number of grouped elements favoring those from earlier groups.
   *
   * @param {Array} groups Contains an arbitrary number of arrays of elements.
   * @returns {Array} A matching array of each provided group deduped.
   */


  group(...groups) {
    const globalKeys = new Set();
    const result = [];

    for (const values of groups) {
      const valueMap = new Map();

      for (const value of values) {
        const key = this.createKey(value);

        if (!globalKeys.has(key) && !valueMap.has(key)) {
          valueMap.set(key, value);
        }
      }

      result.push(valueMap);
      valueMap.forEach((value, key) => globalKeys.add(key));
    }

    return result.map(m => Array.from(m.values()));
  }

}
// CONCATENATED MODULE: ./common/Reducers.jsm
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TOP_SITES_DEFAULT_ROWS", function() { return TOP_SITES_DEFAULT_ROWS; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TOP_SITES_MAX_SITES_PER_ROW", function() { return TOP_SITES_MAX_SITES_PER_ROW; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "INITIAL_STATE", function() { return INITIAL_STATE; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "insertPinned", function() { return insertPinned; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "reducers", function() { return reducers; });
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */




const TOP_SITES_DEFAULT_ROWS = 1;
const TOP_SITES_MAX_SITES_PER_ROW = 8;
const dedupe = new Dedupe(site => site && site.url);
const INITIAL_STATE = {
  App: {
    // Have we received real data from the app yet?
    initialized: false
  },
  TopSites: {
    // Have we received real data from history yet?
    initialized: false,
    // The history (and possibly default) links
    rows: [],
    // Used in content only to dispatch action to TopSiteForm.
    editForm: null,
    // Used in content only to open the SearchShortcutsForm modal.
    showSearchShortcutsForm: false,
    // The list of available search shortcuts.
    searchShortcuts: []
  },
  Prefs: {
    initialized: false,
    values: {}
  },
  Dialog: {
    visible: false,
    data: {}
  },
  Sections: [],
  Search: {
    // When search hand-off is enabled, we render a big button that is styled to
    // look like a search textbox. If the button is clicked, we style
    // the button as if it was a focused search box and show a fake cursor but
    // really focus the awesomebar without the focus styles ("hidden focus").
    fakeFocus: false,
    // Hide the search box after handing off to AwesomeBar and user starts typing.
    hide: false
  }
};

function App(prevState = INITIAL_STATE.App, action) {
  switch (action.type) {
    case Actions["actionTypes"].INIT:
      return Object.assign({}, prevState, action.data || {}, {
        initialized: true
      });

    default:
      return prevState;
  }
}

/**
 * insertPinned - Inserts pinned links in their specified slots
 *
 * @param {array} a list of links
 * @param {array} a list of pinned links
 * @return {array} resulting list of links with pinned links inserted
 */


function insertPinned(links, pinned) {
  // Remove any pinned links
  const pinnedUrls = pinned.map(link => link && link.url);
  let newLinks = links.filter(link => link ? !pinnedUrls.includes(link.url) : false);
  newLinks = newLinks.map(link => {
    if (link && link.isPinned) {
      delete link.isPinned;
      delete link.pinIndex;
    }

    return link;
  }); // Then insert them in their specified location

  pinned.forEach((val, index) => {
    if (!val) {
      return;
    }

    let link = Object.assign({}, val, {
      isPinned: true,
      pinIndex: index
    });

    if (index > newLinks.length) {
      newLinks[index] = link;
    } else {
      newLinks.splice(index, 0, link);
    }
  });
  return newLinks;
}

function TopSites(prevState = INITIAL_STATE.TopSites, action) {
  let hasMatch;
  let newRows;

  switch (action.type) {
    case Actions["actionTypes"].TOP_SITES_UPDATED:
      if (!action.data || !action.data.links) {
        return prevState;
      }

      return Object.assign({}, prevState, {
        initialized: true,
        rows: action.data.links
      }, action.data.pref ? {
        pref: action.data.pref
      } : {});

    case Actions["actionTypes"].TOP_SITES_PREFS_UPDATED:
      return Object.assign({}, prevState, {
        pref: action.data.pref
      });

    case Actions["actionTypes"].TOP_SITES_EDIT:
      return Object.assign({}, prevState, {
        editForm: {
          index: action.data.index,
          previewResponse: null
        }
      });

    case Actions["actionTypes"].TOP_SITES_CANCEL_EDIT:
      return Object.assign({}, prevState, {
        editForm: null
      });

    case Actions["actionTypes"].TOP_SITES_OPEN_SEARCH_SHORTCUTS_MODAL:
      return Object.assign({}, prevState, {
        showSearchShortcutsForm: true
      });

    case Actions["actionTypes"].TOP_SITES_CLOSE_SEARCH_SHORTCUTS_MODAL:
      return Object.assign({}, prevState, {
        showSearchShortcutsForm: false
      });

    case Actions["actionTypes"].PREVIEW_RESPONSE:
      if (!prevState.editForm || action.data.url !== prevState.editForm.previewUrl) {
        return prevState;
      }

      return Object.assign({}, prevState, {
        editForm: {
          index: prevState.editForm.index,
          previewResponse: action.data.preview,
          previewUrl: action.data.url
        }
      });

    case Actions["actionTypes"].PREVIEW_REQUEST:
      if (!prevState.editForm) {
        return prevState;
      }

      return Object.assign({}, prevState, {
        editForm: {
          index: prevState.editForm.index,
          previewResponse: null,
          previewUrl: action.data.url
        }
      });

    case Actions["actionTypes"].PREVIEW_REQUEST_CANCEL:
      if (!prevState.editForm) {
        return prevState;
      }

      return Object.assign({}, prevState, {
        editForm: {
          index: prevState.editForm.index,
          previewResponse: null
        }
      });

    case Actions["actionTypes"].SCREENSHOT_UPDATED:
      newRows = prevState.rows.map(row => {
        if (row && row.url === action.data.url) {
          hasMatch = true;
          return Object.assign({}, row, {
            screenshot: action.data.screenshot
          });
        }

        return row;
      });
      return hasMatch ? Object.assign({}, prevState, {
        rows: newRows
      }) : prevState;

    case Actions["actionTypes"].PLACES_BOOKMARK_ADDED:
      if (!action.data) {
        return prevState;
      }

      newRows = prevState.rows.map(site => {
        if (site && site.url === action.data.url) {
          const {
            bookmarkGuid,
            bookmarkTitle,
            dateAdded
          } = action.data;
          return Object.assign({}, site, {
            bookmarkGuid,
            bookmarkTitle,
            bookmarkDateCreated: dateAdded
          });
        }

        return site;
      });
      return Object.assign({}, prevState, {
        rows: newRows
      });

    case Actions["actionTypes"].PLACES_BOOKMARK_REMOVED:
      if (!action.data) {
        return prevState;
      }

      newRows = prevState.rows.map(site => {
        if (site && site.url === action.data.url) {
          const newSite = Object.assign({}, site);
          delete newSite.bookmarkGuid;
          delete newSite.bookmarkTitle;
          delete newSite.bookmarkDateCreated;
          return newSite;
        }

        return site;
      });
      return Object.assign({}, prevState, {
        rows: newRows
      });

    case Actions["actionTypes"].PLACES_LINK_DELETED:
      if (!action.data) {
        return prevState;
      }

      newRows = prevState.rows.filter(site => action.data.url !== site.url);
      return Object.assign({}, prevState, {
        rows: newRows
      });

    case Actions["actionTypes"].UPDATE_SEARCH_SHORTCUTS:
      return { ...prevState,
        searchShortcuts: action.data.searchShortcuts
      };

    default:
      return prevState;
  }
}

function Dialog(prevState = INITIAL_STATE.Dialog, action) {
  switch (action.type) {
    case Actions["actionTypes"].DIALOG_OPEN:
      return Object.assign({}, prevState, {
        visible: true,
        data: action.data
      });

    case Actions["actionTypes"].DIALOG_CANCEL:
      return Object.assign({}, prevState, {
        visible: false
      });

    case Actions["actionTypes"].DELETE_HISTORY_URL:
      return Object.assign({}, INITIAL_STATE.Dialog);

    default:
      return prevState;
  }
}

function Prefs(prevState = INITIAL_STATE.Prefs, action) {
  let newValues;

  switch (action.type) {
    case Actions["actionTypes"].PREFS_INITIAL_VALUES:
      return Object.assign({}, prevState, {
        initialized: true,
        values: action.data
      });

    case Actions["actionTypes"].PREF_CHANGED:
      newValues = Object.assign({}, prevState.values);
      newValues[action.data.name] = action.data.value;
      return Object.assign({}, prevState, {
        values: newValues
      });

    default:
      return prevState;
  }
}

function Sections(prevState = INITIAL_STATE.Sections, action) {
  let hasMatch;
  let newState;

  switch (action.type) {
    case Actions["actionTypes"].SECTION_DEREGISTER:
      return prevState.filter(section => section.id !== action.data);

    case Actions["actionTypes"].SECTION_REGISTER:
      // If section exists in prevState, update it
      newState = prevState.map(section => {
        if (section && section.id === action.data.id) {
          hasMatch = true;
          return Object.assign({}, section, action.data);
        }

        return section;
      }); // Otherwise, append it

      if (!hasMatch) {
        const initialized = !!(action.data.rows && action.data.rows.length > 0);
        const section = Object.assign({
          title: "",
          rows: [],
          enabled: false
        }, action.data, {
          initialized
        });
        newState.push(section);
      }

      return newState;

    case Actions["actionTypes"].SECTION_UPDATE:
      newState = prevState.map(section => {
        if (section && section.id === action.data.id) {
          // If the action is updating rows, we should consider initialized to be true.
          // This can be overridden if initialized is defined in the action.data
          const initialized = action.data.rows ? {
            initialized: true
          } : {}; // Make sure pinned cards stay at their current position when rows are updated.
          // Disabling a section (SECTION_UPDATE with empty rows) does not retain pinned cards.

          if (action.data.rows && action.data.rows.length > 0 && section.rows.find(card => card.pinned)) {
            const rows = Array.from(action.data.rows);
            section.rows.forEach((card, index) => {
              if (card.pinned) {
                // Only add it if it's not already there.
                if (rows[index].guid !== card.guid) {
                  rows.splice(index, 0, card);
                }
              }
            });
            return Object.assign({}, section, initialized, Object.assign({}, action.data, {
              rows
            }));
          }

          return Object.assign({}, section, initialized, action.data);
        }

        return section;
      });

      if (!action.data.dedupeConfigurations) {
        return newState;
      }

      action.data.dedupeConfigurations.forEach(dedupeConf => {
        newState = newState.map(section => {
          if (section.id === dedupeConf.id) {
            const dedupedRows = dedupeConf.dedupeFrom.reduce((rows, dedupeSectionId) => {
              const dedupeSection = newState.find(s => s.id === dedupeSectionId);
              const [, newRows] = dedupe.group(dedupeSection.rows, rows);
              return newRows;
            }, section.rows);
            return Object.assign({}, section, {
              rows: dedupedRows
            });
          }

          return section;
        });
      });
      return newState;

    case Actions["actionTypes"].SECTION_UPDATE_CARD:
      return prevState.map(section => {
        if (section && section.id === action.data.id && section.rows) {
          const newRows = section.rows.map(card => {
            if (card.url === action.data.url) {
              return Object.assign({}, card, action.data.options);
            }

            return card;
          });
          return Object.assign({}, section, {
            rows: newRows
          });
        }

        return section;
      });

    case Actions["actionTypes"].PLACES_BOOKMARK_ADDED:
      if (!action.data) {
        return prevState;
      }

      return prevState.map(section => Object.assign({}, section, {
        rows: section.rows.map(item => {
          // find the item within the rows that is attempted to be bookmarked
          if (item.url === action.data.url) {
            const {
              bookmarkGuid,
              bookmarkTitle,
              dateAdded
            } = action.data;
            return Object.assign({}, item, {
              bookmarkGuid,
              bookmarkTitle,
              bookmarkDateCreated: dateAdded,
              type: "bookmark"
            });
          }

          return item;
        })
      }));

    case Actions["actionTypes"].PLACES_BOOKMARK_REMOVED:
      if (!action.data) {
        return prevState;
      }

      return prevState.map(section => Object.assign({}, section, {
        rows: section.rows.map(item => {
          // find the bookmark within the rows that is attempted to be removed
          if (item.url === action.data.url) {
            const newSite = Object.assign({}, item);
            delete newSite.bookmarkGuid;
            delete newSite.bookmarkTitle;
            delete newSite.bookmarkDateCreated;

            if (!newSite.type || newSite.type === "bookmark") {
              newSite.type = "history";
            }

            return newSite;
          }

          return item;
        })
      }));

    case Actions["actionTypes"].PLACES_LINK_DELETED:
    case Actions["actionTypes"].PLACES_LINK_BLOCKED:
      if (!action.data) {
        return prevState;
      }

      return prevState.map(section => Object.assign({}, section, {
        rows: section.rows.filter(site => site.url !== action.data.url)
      }));

    default:
      return prevState;
  }
}

function Search(prevState = INITIAL_STATE.Search, action) {
  switch (action.type) {
    case Actions["actionTypes"].HIDE_SEARCH:
      return Object.assign({ ...prevState,
        hide: true
      });

    case Actions["actionTypes"].FAKE_FOCUS_SEARCH:
      return Object.assign({ ...prevState,
        fakeFocus: true
      });

    case Actions["actionTypes"].SHOW_SEARCH:
      return Object.assign({ ...prevState,
        hide: false,
        fakeFocus: false
      });

    default:
      return prevState;
  }
}

var reducers = {
  TopSites,
  App,
  Prefs,
  Dialog,
  Sections,
  Search
};

/***/ }),
/* 54 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);

// EXTERNAL MODULE: ./common/Actions.jsm
var Actions = __webpack_require__(2);

// CONCATENATED MODULE: ./content-src/components/Card/types.js
const cardContextTypes = {
  history: {
    fluentID: "newtab-label-visited",
    icon: "history-item"
  },
  bookmark: {
    fluentID: "newtab-label-bookmarked",
    icon: "bookmark-added"
  },
  trending: {
    fluentID: "newtab-label-recommended",
    icon: "trending"
  },
  download: {
    fluentID: "newtab-label-download",
    icon: "download"
  }
};
// EXTERNAL MODULE: external "ReactRedux"
var external_ReactRedux_ = __webpack_require__(24);

// EXTERNAL MODULE: ./content-src/components/LinkMenu/LinkMenu.jsx + 1 modules
var LinkMenu = __webpack_require__(56);

// EXTERNAL MODULE: external "React"
var external_React_ = __webpack_require__(9);
var external_React_default = /*#__PURE__*/__webpack_require__.n(external_React_);

// EXTERNAL MODULE: ./content-src/lib/screenshot-utils.js
var screenshot_utils = __webpack_require__(37);

// CONCATENATED MODULE: ./content-src/components/Card/Card.jsx
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_Card", function() { return Card_Card; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Card", function() { return Card; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "PlaceholderCard", function() { return PlaceholderCard; });





 // Keep track of pending image loads to only request once

const gImageLoading = new Map();
/**
 * Card component.
 * Cards are found within a Section component and contain information about a link such
 * as preview image, page title, page description, and some context about if the page
 * was visited, bookmarked, trending etc...
 * Each Section can make an unordered list of Cards which will create one instane of
 * this class. Each card will then get a context menu which reflects the actions that
 * can be done on this Card.
 */

class Card_Card extends external_React_default.a.PureComponent {
  constructor(props) {
    super(props);
    this.state = {
      activeCard: null,
      imageLoaded: false,
      showContextMenu: false,
      cardImage: null
    };
    this.onMenuButtonClick = this.onMenuButtonClick.bind(this);
    this.onMenuUpdate = this.onMenuUpdate.bind(this);
    this.onLinkClick = this.onLinkClick.bind(this);
  }
  /**
   * Helper to conditionally load an image and update state when it loads.
   */


  async maybeLoadImage() {
    // No need to load if it's already loaded or no image
    const {
      cardImage
    } = this.state;

    if (!cardImage) {
      return;
    }

    const imageUrl = cardImage.url;

    if (!this.state.imageLoaded) {
      // Initialize a promise to share a load across multiple card updates
      if (!gImageLoading.has(imageUrl)) {
        const loaderPromise = new Promise((resolve, reject) => {
          const loader = new Image();
          loader.addEventListener("load", resolve);
          loader.addEventListener("error", reject);
          loader.src = imageUrl;
        }); // Save and remove the promise only while it's pending

        gImageLoading.set(imageUrl, loaderPromise);
        loaderPromise.catch(ex => ex).then(() => gImageLoading.delete(imageUrl)).catch();
      } // Wait for the image whether just started loading or reused promise


      await gImageLoading.get(imageUrl); // Only update state if we're still waiting to load the original image

      if (screenshot_utils["ScreenshotUtils"].isRemoteImageLocal(this.state.cardImage, this.props.link.image) && !this.state.imageLoaded) {
        this.setState({
          imageLoaded: true
        });
      }
    }
  }
  /**
   * Helper to obtain the next state based on nextProps and prevState.
   *
   * NOTE: Rename this method to getDerivedStateFromProps when we update React
   *       to >= 16.3. We will need to update tests as well. We cannot rename this
   *       method to getDerivedStateFromProps now because there is a mismatch in
   *       the React version that we are using for both testing and production.
   *       (i.e. react-test-render => "16.3.2", react => "16.2.0").
   *
   * See https://github.com/airbnb/enzyme/blob/master/packages/enzyme-adapter-react-16/package.json#L43.
   */


  static getNextStateFromProps(nextProps, prevState) {
    const {
      image
    } = nextProps.link;
    const imageInState = screenshot_utils["ScreenshotUtils"].isRemoteImageLocal(prevState.cardImage, image);
    let nextState = null; // Image is updating.

    if (!imageInState && nextProps.link) {
      nextState = {
        imageLoaded: false
      };
    }

    if (imageInState) {
      return nextState;
    } // Since image was updated, attempt to revoke old image blob URL, if it exists.


    screenshot_utils["ScreenshotUtils"].maybeRevokeBlobObjectURL(prevState.cardImage);
    nextState = nextState || {};
    nextState.cardImage = screenshot_utils["ScreenshotUtils"].createLocalImageObject(image);
    return nextState;
  }

  onMenuButtonClick(event) {
    event.preventDefault();
    this.setState({
      activeCard: this.props.index,
      showContextMenu: true
    });
  }

  onLinkClick(event) {
    event.preventDefault();

    if (this.props.link.type === "download") {
      this.props.dispatch(Actions["actionCreators"].OnlyToMain({
        type: Actions["actionTypes"].SHOW_DOWNLOAD_FILE,
        data: this.props.link
      }));
    } else {
      const {
        altKey,
        button,
        ctrlKey,
        metaKey,
        shiftKey
      } = event;
      this.props.dispatch(Actions["actionCreators"].OnlyToMain({
        type: Actions["actionTypes"].OPEN_LINK,
        data: Object.assign(this.props.link, {
          event: {
            altKey,
            button,
            ctrlKey,
            metaKey,
            shiftKey
          }
        })
      }));
    }

    if (this.props.isWebExtension) {
      this.props.dispatch(Actions["actionCreators"].WebExtEvent(Actions["actionTypes"].WEBEXT_CLICK, {
        source: this.props.eventSource,
        url: this.props.link.url,
        action_position: this.props.index
      }));
    }
  }

  onMenuUpdate(showContextMenu) {
    this.setState({
      showContextMenu
    });
  }

  componentDidMount() {
    this.maybeLoadImage();
  }

  componentDidUpdate() {
    this.maybeLoadImage();
  } // NOTE: Remove this function when we update React to >= 16.3 since React will
  //       call getDerivedStateFromProps automatically. We will also need to
  //       rename getNextStateFromProps to getDerivedStateFromProps.


  componentWillMount() {
    const nextState = Card_Card.getNextStateFromProps(this.props, this.state);

    if (nextState) {
      this.setState(nextState);
    }
  } // NOTE: Remove this function when we update React to >= 16.3 since React will
  //       call getDerivedStateFromProps automatically. We will also need to
  //       rename getNextStateFromProps to getDerivedStateFromProps.


  componentWillReceiveProps(nextProps) {
    const nextState = Card_Card.getNextStateFromProps(nextProps, this.state);

    if (nextState) {
      this.setState(nextState);
    }
  }

  componentWillUnmount() {
    screenshot_utils["ScreenshotUtils"].maybeRevokeBlobObjectURL(this.state.cardImage);
  }

  render() {
    const {
      index,
      className,
      link,
      dispatch,
      contextMenuOptions,
      eventSource,
    } = this.props;
    const {
      props
    } = this;
    const title = link.title || link.hostname;
    const isContextMenuOpen = this.state.showContextMenu && this.state.activeCard === index; // Display "now" as "trending" until we have new strings #3402

    const {
      icon,
      fluentID
    } = cardContextTypes[link.type === "now" ? "trending" : link.type] || {};
    const hasImage = this.state.cardImage || link.hasImage;
    const imageStyle = {
      backgroundImage: this.state.cardImage ? `url(${this.state.cardImage.url})` : "none"
    };
    const outerClassName = ["card-outer", className, isContextMenuOpen && "active", props.placeholder && "placeholder"].filter(v => v).join(" ");
    return external_React_default.a.createElement("li", {
      className: outerClassName
    }, external_React_default.a.createElement("a", {
      href: link.url,
      onClick: !props.placeholder ? this.onLinkClick : undefined
    }, external_React_default.a.createElement("div", {
      className: "card"
    }, external_React_default.a.createElement("div", {
      className: "card-preview-image-outer"
    }, hasImage && external_React_default.a.createElement("div", {
      className: `card-preview-image${this.state.imageLoaded ? " loaded" : ""}`,
      style: imageStyle
    })), external_React_default.a.createElement("div", {
      className: "card-details"
    }, link.type === "download" && external_React_default.a.createElement("div", {
      className: "card-host-name alternate",
      "data-l10n-id": "newtab-menu-show-file"
    }), link.hostname && external_React_default.a.createElement("div", {
      className: "card-host-name"
    }, link.hostname.slice(0, 100), link.type === "download" && `  \u2014 ${link.description}`), external_React_default.a.createElement("div", {
      className: ["card-text", icon ? "" : "no-context", link.description ? "" : "no-description", link.hostname ? "" : "no-host-name"].join(" ")
    }, external_React_default.a.createElement("h4", {
      className: "card-title",
      dir: "auto"
    }, link.title), external_React_default.a.createElement("p", {
      className: "card-description",
      dir: "auto"
    }, link.description)), external_React_default.a.createElement("div", {
      className: "card-context"
    }, icon && !link.context && external_React_default.a.createElement("span", {
      "aria-haspopup": "true",
      className: `card-context-icon icon icon-${icon}`
    }), link.icon && link.context && external_React_default.a.createElement("span", {
      "aria-haspopup": "true",
      className: "card-context-icon icon",
      style: {
        backgroundImage: `url('${link.icon}')`
      }
    }), fluentID && !link.context && external_React_default.a.createElement("div", {
      className: "card-context-label",
      "data-l10n-id": fluentID
    }), link.context && external_React_default.a.createElement("div", {
      className: "card-context-label"
    }, link.context))))), !props.placeholder && external_React_default.a.createElement("button", {
      "aria-haspopup": "true",
      "data-l10n-id": "newtab-menu-content-tooltip",
      "data-l10n-args": JSON.stringify({
        title
      }),
      className: "context-menu-button icon",
      onClick: this.onMenuButtonClick
    }), isContextMenuOpen && external_React_default.a.createElement(LinkMenu["LinkMenu"], {
      dispatch: dispatch,
      index: index,
      source: eventSource,
      onUpdate: this.onMenuUpdate,
      options: link.contextMenuOptions || contextMenuOptions,
      site: link,
    }));
  }

}
Card_Card.defaultProps = {
  link: {}
};
const Card = Object(external_ReactRedux_["connect"])(state => ({
  platform: state.Prefs.values.platform
}))(Card_Card);
const PlaceholderCard = props => external_React_default.a.createElement(Card, {
  placeholder: true,
  className: props.className
});

/***/ }),
/* 55 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);

// EXTERNAL MODULE: ./common/Actions.jsm
var Actions = __webpack_require__(2);

// EXTERNAL MODULE: ./content-src/components/A11yLinkButton/A11yLinkButton.jsx
var A11yLinkButton = __webpack_require__(32);

// EXTERNAL MODULE: external "React"
var external_React_ = __webpack_require__(9);
var external_React_default = /*#__PURE__*/__webpack_require__.n(external_React_);

// EXTERNAL MODULE: ./content-src/components/TopSites/TopSitesConstants.js
var TopSitesConstants = __webpack_require__(44);

// CONCATENATED MODULE: ./content-src/components/TopSites/TopSiteFormInput.jsx

class TopSiteFormInput_TopSiteFormInput extends external_React_default.a.PureComponent {
  constructor(props) {
    super(props);
    this.state = {
      validationError: this.props.validationError
    };
    this.onChange = this.onChange.bind(this);
    this.onMount = this.onMount.bind(this);
    this.onClearIconPress = this.onClearIconPress.bind(this);
  }

  componentWillReceiveProps(nextProps) {
    if (nextProps.shouldFocus && !this.props.shouldFocus) {
      this.input.focus();
    }

    if (nextProps.validationError && !this.props.validationError) {
      this.setState({
        validationError: true
      });
    } // If the component is in an error state but the value was cleared by the parent


    if (this.state.validationError && !nextProps.value) {
      this.setState({
        validationError: false
      });
    }
  }

  onClearIconPress(event) {
    // If there is input in the URL or custom image URL fields,
    // and we hit 'enter' while tabbed over the clear icon,
    // we should execute the function to clear the field.
    if (event.key === "Enter") {
      this.props.onClear();
    }
  }

  onChange(ev) {
    if (this.state.validationError) {
      this.setState({
        validationError: false
      });
    }

    this.props.onChange(ev);
  }

  onMount(input) {
    this.input = input;
  }

  renderLoadingOrCloseButton() {
    const showClearButton = this.props.value && this.props.onClear;

    if (this.props.loading) {
      return external_React_default.a.createElement("div", {
        className: "loading-container"
      }, external_React_default.a.createElement("div", {
        className: "loading-animation"
      }));
    } else if (showClearButton) {
      return external_React_default.a.createElement("button", {
        type: "button",
        className: "icon icon-clear-input icon-button-style",
        onClick: this.props.onClear,
        onKeyPress: this.onClearIconPress
      });
    }

    return null;
  }

  render() {
    const {
      typeUrl
    } = this.props;
    const {
      validationError
    } = this.state;
    return external_React_default.a.createElement("label", null, external_React_default.a.createElement("span", {
      "data-l10n-id": this.props.titleId
    }), external_React_default.a.createElement("div", {
      className: `field ${typeUrl ? "url" : ""}${validationError ? " invalid" : ""}`
    }, external_React_default.a.createElement("input", {
      type: "text",
      value: this.props.value,
      ref: this.onMount,
      onChange: this.onChange,
      "data-l10n-id": this.props.placeholderId // Set focus on error if the url field is valid or when the input is first rendered and is empty
      // eslint-disable-next-line jsx-a11y/no-autofocus
      ,
      autoFocus: this.props.shouldFocus,
      disabled: this.props.loading
    }), this.renderLoadingOrCloseButton(), validationError && external_React_default.a.createElement("aside", {
      className: "error-tooltip",
      "data-l10n-id": this.props.errorMessageId
    })));
  }

}
TopSiteFormInput_TopSiteFormInput.defaultProps = {
  showClearButton: false,
  value: "",
  validationError: false
};
// EXTERNAL MODULE: ./content-src/components/TopSites/TopSite.jsx
var TopSite = __webpack_require__(46);

// CONCATENATED MODULE: ./content-src/components/TopSites/TopSiteForm.jsx
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TopSiteForm", function() { return TopSiteForm_TopSiteForm; });






class TopSiteForm_TopSiteForm extends external_React_default.a.PureComponent {
  constructor(props) {
    super(props);
    const {
      site
    } = props;
    this.state = {
      label: site ? site.label || site.hostname : "",
      url: site ? site.url : "",
      validationError: false,
      customScreenshotUrl: site ? site.customScreenshotURL : "",
      showCustomScreenshotForm: site ? site.customScreenshotURL : false
    };
    this.onClearScreenshotInput = this.onClearScreenshotInput.bind(this);
    this.onLabelChange = this.onLabelChange.bind(this);
    this.onUrlChange = this.onUrlChange.bind(this);
    this.onCancelButtonClick = this.onCancelButtonClick.bind(this);
    this.onClearUrlClick = this.onClearUrlClick.bind(this);
    this.onDoneButtonClick = this.onDoneButtonClick.bind(this);
    this.onCustomScreenshotUrlChange = this.onCustomScreenshotUrlChange.bind(this);
    this.onPreviewButtonClick = this.onPreviewButtonClick.bind(this);
    this.onEnableScreenshotUrlForm = this.onEnableScreenshotUrlForm.bind(this);
    this.validateUrl = this.validateUrl.bind(this);
  }

  onLabelChange(event) {
    this.setState({
      "label": event.target.value
    });
  }

  onUrlChange(event) {
    this.setState({
      url: event.target.value,
      validationError: false
    });
  }

  onClearUrlClick() {
    this.setState({
      url: "",
      validationError: false
    });
  }

  onEnableScreenshotUrlForm() {
    this.setState({
      showCustomScreenshotForm: true
    });
  }

  _updateCustomScreenshotInput(customScreenshotUrl) {
    this.setState({
      customScreenshotUrl,
      validationError: false
    });
    this.props.dispatch({
      type: Actions["actionTypes"].PREVIEW_REQUEST_CANCEL
    });
  }

  onCustomScreenshotUrlChange(event) {
    this._updateCustomScreenshotInput(event.target.value);
  }

  onClearScreenshotInput() {
    this._updateCustomScreenshotInput("");
  }

  onCancelButtonClick(ev) {
    ev.preventDefault();
    this.props.onClose();
  }

  onDoneButtonClick(ev) {
    ev.preventDefault();

    if (this.validateForm()) {
      const site = {
        url: this.cleanUrl(this.state.url)
      };
      const {
        index
      } = this.props;

      if (this.state.label !== "") {
        site.label = this.state.label;
      }

      if (this.state.customScreenshotUrl) {
        site.customScreenshotURL = this.cleanUrl(this.state.customScreenshotUrl);
      } else if (this.props.site && this.props.site.customScreenshotURL) {
        // Used to flag that previously cached screenshot should be removed
        site.customScreenshotURL = null;
      }

      this.props.dispatch(Actions["actionCreators"].AlsoToMain({
        type: Actions["actionTypes"].TOP_SITES_PIN,
        data: {
          site,
          index
        }
      }));
      this.props.onClose();
    }
  }

  onPreviewButtonClick(event) {
    event.preventDefault();

    if (this.validateForm()) {
      this.props.dispatch(Actions["actionCreators"].AlsoToMain({
        type: Actions["actionTypes"].PREVIEW_REQUEST,
        data: {
          url: this.cleanUrl(this.state.customScreenshotUrl)
        }
      }));
    }
  }

  cleanUrl(url) {
    // If we are missing a protocol, prepend http://
    if (!url.startsWith("http:") && !url.startsWith("https:")) {
      return `http://${url}`;
    }

    return url;
  }

  _tryParseUrl(url) {
    try {
      return new URL(url);
    } catch (e) {
      return null;
    }
  }

  validateUrl(url) {
    const validProtocols = ["http:", "https:"];

    const urlObj = this._tryParseUrl(url) || this._tryParseUrl(this.cleanUrl(url));

    return urlObj && validProtocols.includes(urlObj.protocol);
  }

  validateCustomScreenshotUrl() {
    const {
      customScreenshotUrl
    } = this.state;
    return !customScreenshotUrl || this.validateUrl(customScreenshotUrl);
  }

  validateForm() {
    const validate = this.validateUrl(this.state.url) && this.validateCustomScreenshotUrl();

    if (!validate) {
      this.setState({
        validationError: true
      });
    }

    return validate;
  }

  _renderCustomScreenshotInput() {
    const {
      customScreenshotUrl
    } = this.state;
    const requestFailed = this.props.previewResponse === "";
    const validationError = this.state.validationError && !this.validateCustomScreenshotUrl() || requestFailed; // Set focus on error if the url field is valid or when the input is first rendered and is empty

    const shouldFocus = validationError && this.validateUrl(this.state.url) || !customScreenshotUrl;
    const isLoading = this.props.previewResponse === null && customScreenshotUrl && this.props.previewUrl === this.cleanUrl(customScreenshotUrl);

    if (!this.state.showCustomScreenshotForm) {
      return external_React_default.a.createElement(A11yLinkButton["A11yLinkButton"], {
        onClick: this.onEnableScreenshotUrlForm,
        className: "enable-custom-image-input",
        "data-l10n-id": "newtab-topsites-use-image-link"
      });
    }

    return external_React_default.a.createElement("div", {
      className: "custom-image-input-container"
    }, external_React_default.a.createElement(TopSiteFormInput_TopSiteFormInput, {
      errorMessageId: requestFailed ? "newtab-topsites-image-validation" : "newtab-topsites-url-validation",
      loading: isLoading,
      onChange: this.onCustomScreenshotUrlChange,
      onClear: this.onClearScreenshotInput,
      shouldFocus: shouldFocus,
      typeUrl: true,
      value: customScreenshotUrl,
      validationError: validationError,
      titleId: "newtab-topsites-image-url-label",
      placeholderId: "newtab-topsites-url-input"
    }));
  }

  render() {
    const {
      customScreenshotUrl
    } = this.state;
    const requestFailed = this.props.previewResponse === ""; // For UI purposes, editing without an existing link is "add"

    const showAsAdd = !this.props.site;
    const previous = this.props.site && this.props.site.customScreenshotURL || "";
    const changed = customScreenshotUrl && this.cleanUrl(customScreenshotUrl) !== previous; // Preview mode if changes were made to the custom screenshot URL and no preview was received yet
    // or the request failed

    const previewMode = changed && !this.props.previewResponse;
    const previewLink = Object.assign({}, this.props.site);

    if (this.props.previewResponse) {
      previewLink.screenshot = this.props.previewResponse;
      previewLink.customScreenshotURL = this.props.previewUrl;
    } // Handles the form submit so an enter press performs the correct action


    const onSubmit = previewMode ? this.onPreviewButtonClick : this.onDoneButtonClick;
    return external_React_default.a.createElement("form", {
      className: "topsite-form",
      onSubmit: onSubmit
    }, external_React_default.a.createElement("div", {
      className: "form-input-container"
    }, external_React_default.a.createElement("h3", {
      className: "section-title grey-title",
      "data-l10n-id": showAsAdd ? "newtab-topsites-add-topsites-header" : "newtab-topsites-edit-topsites-header"
    }), external_React_default.a.createElement("div", {
      className: "fields-and-preview"
    }, external_React_default.a.createElement("div", {
      className: "form-wrapper"
    }, external_React_default.a.createElement(TopSiteFormInput_TopSiteFormInput, {
      onChange: this.onLabelChange,
      value: this.state.label,
      titleId: "newtab-topsites-title-label",
      placeholderId: "newtab-topsites-title-input"
    }), external_React_default.a.createElement(TopSiteFormInput_TopSiteFormInput, {
      onChange: this.onUrlChange,
      shouldFocus: this.state.validationError && !this.validateUrl(this.state.url),
      value: this.state.url,
      onClear: this.onClearUrlClick,
      validationError: this.state.validationError && !this.validateUrl(this.state.url),
      titleId: "newtab-topsites-url-label",
      typeUrl: true,
      placeholderId: "newtab-topsites-url-input",
      errorMessageId: "newtab-topsites-url-validation"
    }), this._renderCustomScreenshotInput()), external_React_default.a.createElement(TopSite["TopSiteLink"], {
      link: previewLink,
      defaultStyle: requestFailed,
      title: this.state.label
    }))), external_React_default.a.createElement("section", {
      className: "actions"
    }, external_React_default.a.createElement("button", {
      className: "cancel",
      type: "button",
      onClick: this.onCancelButtonClick,
      "data-l10n-id": "newtab-topsites-cancel-button"
    }), previewMode ? external_React_default.a.createElement("button", {
      className: "done preview",
      type: "submit",
      "data-l10n-id": "newtab-topsites-preview-button"
    }) : external_React_default.a.createElement("button", {
      className: "done",
      type: "submit",
      "data-l10n-id": showAsAdd ? "newtab-topsites-add-button" : "newtab-topsites-save-button"
    })));
  }

}
TopSiteForm_TopSiteForm.defaultProps = {
  site: null,
  index: -1
};

/***/ }),
/* 56 */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);

// EXTERNAL MODULE: ./common/Actions.jsm
var Actions = __webpack_require__(2);

// EXTERNAL MODULE: external "ReactRedux"
var external_ReactRedux_ = __webpack_require__(24);

// EXTERNAL MODULE: ./content-src/components/ContextMenu/ContextMenu.jsx
var ContextMenu = __webpack_require__(28);

// CONCATENATED MODULE: ./content-src/lib/link-menu-options.js


const _OpenInPrivateWindow = site => ({
  id: "newtab-menu-open-new-private-window",
  icon: "new-window-private",
  action: Actions["actionCreators"].OnlyToMain({
    type: Actions["actionTypes"].OPEN_PRIVATE_WINDOW,
    data: {
      url: site.url,
      referrer: site.referrer
    }
  }),
});
/**
 * List of functions that return items that can be included as menu options in a
 * LinkMenu. All functions take the site as the first parameter, and optionally
 * the index of the site.
 */


const LinkMenuOptions = {
  Separator: () => ({
    type: "separator"
  }),
  EmptyItem: () => ({
    type: "empty"
  }),
  RemoveBookmark: site => ({
    id: "newtab-menu-remove-bookmark",
    icon: "bookmark-added",
    action: Actions["actionCreators"].AlsoToMain({
      type: Actions["actionTypes"].DELETE_BOOKMARK_BY_ID,
      data: site.bookmarkGuid
    }),
  }),
  AddBookmark: site => ({
    id: "newtab-menu-bookmark",
    icon: "bookmark-hollow",
    action: Actions["actionCreators"].AlsoToMain({
      type: Actions["actionTypes"].BOOKMARK_URL,
      data: {
        url: site.url,
        title: site.title,
        type: site.type
      }
    }),
  }),
  OpenInNewWindow: site => ({
    id: "newtab-menu-open-new-window",
    icon: "new-window",
    action: Actions["actionCreators"].AlsoToMain({
      type: Actions["actionTypes"].OPEN_NEW_WINDOW,
      data: {
        referrer: site.referrer,
        typedBonus: site.typedBonus,
        url: site.url
      }
    }),
  }),
  BlockUrl: (site, index, eventSource) => ({
    id: "newtab-menu-dismiss",
    icon: "dismiss",
    action: Actions["actionCreators"].AlsoToMain({
      type: Actions["actionTypes"].BLOCK_URL,
      data: {
        url: site.open_url || site.url
      }
    }),
  }),
  // This is an option for web extentions which will result in remove items from
  // memory and notify the web extenion, rather than using the built-in block list.
  WebExtDismiss: (site, index, eventSource) => ({
    id: "menu_action_webext_dismiss",
    string_id: "newtab-menu-dismiss",
    icon: "dismiss",
    action: Actions["actionCreators"].WebExtEvent(Actions["actionTypes"].WEBEXT_DISMISS, {
      source: eventSource,
      url: site.url,
      action_position: index
    })
  }),
  DeleteUrl: (site, index, eventSource, isEnabled, siteInfo) => ({
    id: "newtab-menu-delete-history",
    icon: "delete",
    action: {
      type: Actions["actionTypes"].DIALOG_OPEN,
      data: {
        onConfirm: [Actions["actionCreators"].AlsoToMain({
          type: Actions["actionTypes"].DELETE_HISTORY_URL,
          data: {
            url: site.url,
            forceBlock: site.bookmarkGuid
          }
        })],
        eventSource,
        body_string_id: ["newtab-confirm-delete-history-p1", "newtab-confirm-delete-history-p2"],
        confirm_button_string_id: "newtab-topsites-delete-history-button",
        cancel_button_string_id: "newtab-topsites-cancel-button",
        icon: "modal-delete"
      }
    },
  }),
  ShowFile: site => ({
    id: "newtab-menu-show-file",
    icon: "search",
    action: Actions["actionCreators"].OnlyToMain({
      type: Actions["actionTypes"].SHOW_DOWNLOAD_FILE,
      data: {
        url: site.url
      }
    })
  }),
  OpenFile: site => ({
    id: "newtab-menu-open-file",
    icon: "open-file",
    action: Actions["actionCreators"].OnlyToMain({
      type: Actions["actionTypes"].OPEN_DOWNLOAD_FILE,
      data: {
        url: site.url
      }
    })
  }),
  CopyDownloadLink: site => ({
    id: "newtab-menu-copy-download-link",
    icon: "copy",
    action: Actions["actionCreators"].OnlyToMain({
      type: Actions["actionTypes"].COPY_DOWNLOAD_LINK,
      data: {
        url: site.url
      }
    })
  }),
  GoToDownloadPage: site => ({
    id: "newtab-menu-go-to-download-page",
    icon: "download",
    action: Actions["actionCreators"].OnlyToMain({
      type: Actions["actionTypes"].OPEN_LINK,
      data: {
        url: site.referrer
      }
    }),
    disabled: !site.referrer
  }),
  RemoveDownload: site => ({
    id: "newtab-menu-remove-download",
    icon: "delete",
    action: Actions["actionCreators"].OnlyToMain({
      type: Actions["actionTypes"].REMOVE_DOWNLOAD_FILE,
      data: {
        url: site.url
      }
    })
  }),
  PinTopSite: ({
    url,
    searchTopSite,
    label
  }, index) => ({
    id: "newtab-menu-pin",
    icon: "pin",
    action: Actions["actionCreators"].AlsoToMain({
      type: Actions["actionTypes"].TOP_SITES_PIN,
      data: {
        site: {
          url,
          ...(searchTopSite && {
            searchTopSite,
            label
          })
        },
        index
      }
    }),
  }),
  UnpinTopSite: site => ({
    id: "newtab-menu-unpin",
    icon: "unpin",
    action: Actions["actionCreators"].AlsoToMain({
      type: Actions["actionTypes"].TOP_SITES_UNPIN,
      data: {
        site: {
          url: site.url
        }
      }
    }),
  }),
  EditTopSite: (site, index) => ({
    id: "newtab-menu-edit-topsites",
    icon: "edit",
    action: {
      type: Actions["actionTypes"].TOP_SITES_EDIT,
      data: {
        index
      }
    }
  }),
  CheckBookmark: site => site.bookmarkGuid ? LinkMenuOptions.RemoveBookmark(site) : LinkMenuOptions.AddBookmark(site),
  CheckPinTopSite: (site, index) => site.isPinned ? LinkMenuOptions.UnpinTopSite(site) : LinkMenuOptions.PinTopSite(site, index),
  OpenInPrivateWindow: (site, index, eventSource, isEnabled) => isEnabled ? _OpenInPrivateWindow(site) : LinkMenuOptions.EmptyItem()
};
// EXTERNAL MODULE: external "React"
var external_React_ = __webpack_require__(9);
var external_React_default = /*#__PURE__*/__webpack_require__.n(external_React_);

// CONCATENATED MODULE: ./content-src/components/LinkMenu/LinkMenu.jsx
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "_LinkMenu", function() { return LinkMenu_LinkMenu; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LinkMenu", function() { return LinkMenu; });





const DEFAULT_SITE_MENU_OPTIONS = ["CheckPinTopSite", "EditTopSite", "Separator", "OpenInNewWindow", "OpenInPrivateWindow", "Separator", "BlockUrl"];
class LinkMenu_LinkMenu extends external_React_default.a.PureComponent {
  getOptions() {
    const {
      props
    } = this;
    const {
      site,
      index,
      source,
      isPrivateBrowsingEnabled,
      siteInfo,
      platform
    } = props; // Handle special case of default site

    const propOptions = !site.isDefault || site.searchTopSite ? props.options : DEFAULT_SITE_MENU_OPTIONS;
    const options = propOptions.map(o => LinkMenuOptions[o](site, index, source, isPrivateBrowsingEnabled, siteInfo, platform)).map(option => {
      const {
        action,
        id,
        type
      } = option;

      if (!type && id) {
        option.onClick = () => {
          props.dispatch(action);
        };
      }

      return option;
    }); // This is for accessibility to support making each item tabbable.
    // We want to know which item is the first and which item
    // is the last, so we can close the context menu accordingly.

    options[0].first = true;
    options[options.length - 1].last = true;
    return options;
  }

  render() {
    return external_React_default.a.createElement(ContextMenu["ContextMenu"], {
      onUpdate: this.props.onUpdate,
      onShow: this.props.onShow,
      options: this.getOptions()
    });
  }

}

const getState = state => ({
  isPrivateBrowsingEnabled: state.Prefs.values.isPrivateBrowsingEnabled,
  platform: state.Prefs.values.platform
});

const LinkMenu = Object(external_ReactRedux_["connect"])(getState)(LinkMenu_LinkMenu);

/***/ })
/******/ ]);