"use strict";

this.webRequest = class extends ExtensionAPI {
  getAPI(context) {
    let filters = new WeakSet();

    context.callOnClose({
      close() {
        for (let filter of ChromeUtils.nondeterministicGetWeakSetKeys(
          filters
        )) {
          try {
            filter.disconnect();
          } catch (e) {
            // Ignore.
          }
        }
      },
    });

    return {
      webRequest: {
        filterResponseData(requestId) {
          requestId = parseInt(requestId, 10);

          let streamFilter = context.cloneScope.StreamFilter.create(
            requestId,
            context.extension.id
          );

          filters.add(streamFilter);
          return streamFilter;
        },
      },
    };
  }
};
