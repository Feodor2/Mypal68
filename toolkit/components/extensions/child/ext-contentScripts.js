"use strict";

var { ExtensionError } = ExtensionUtils;

/**
 * Represents (in the child extension process) a content script registered
 * programmatically (instead of being included in the addon manifest).
 *
 * @param {ExtensionPageContextChild} context
 *        The extension context which has registered the content script.
 * @param {string} scriptId
 *        An unique id that represents the registered content script
 *        (generated and used internally to identify it across the different processes).
 */
class ContentScriptChild {
  constructor(context, scriptId) {
    this.context = context;
    this.scriptId = scriptId;
    this.unregistered = false;
  }

  async unregister() {
    if (this.unregistered) {
      throw new ExtensionError("Content script already unregistered");
    }

    this.unregistered = true;

    await this.context.childManager.callParentAsyncFunction(
      "contentScripts.unregister",
      [this.scriptId]
    );

    this.context = null;
  }

  api() {
    const { context } = this;

    // TODO(rpl): allow to read the options related to the registered content script?
    return {
      unregister: () => {
        return context.wrapPromise(this.unregister());
      },
    };
  }
}

this.contentScripts = class extends ExtensionAPI {
  getAPI(context) {
    return {
      contentScripts: {
        register(options) {
          return context.cloneScope.Promise.resolve().then(async () => {
            const scriptId = await context.childManager.callParentAsyncFunction(
              "contentScripts.register",
              [options]
            );

            const registeredScript = new ContentScriptChild(context, scriptId);

            return Cu.cloneInto(registeredScript.api(), context.cloneScope, {
              cloneFunctions: true,
            });
          });
        },
      },
    };
  }
};
