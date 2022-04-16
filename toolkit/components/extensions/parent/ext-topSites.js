"use strict";

ChromeUtils.defineModuleGetter(
  this,
  "NewTabUtils",
  "resource://gre/modules/NewTabUtils.jsm"
);

this.topSites = class extends ExtensionAPI {
  getAPI(context) {
    return {
      topSites: {
        get: async function(options) {
          let links = await NewTabUtils.activityStreamLinks.getTopSites({
            ignoreBlocked: options.includeBlocked,
            onePerDomain: options.onePerDomain,
            numItems: options.limit,
            includeFavicon: options.includeFavicon,
          });
          return links.map(link => {
            return {
              url: link.url,
              title: link.title,
              favicon: link.favicon,
            };
          });
        },
      },
    };
  }
};
