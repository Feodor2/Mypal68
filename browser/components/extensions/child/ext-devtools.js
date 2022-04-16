"use strict";

this.devtools = class extends ExtensionAPI {
  getAPI(context) {
    return {
      devtools: {},
    };
  }
};
