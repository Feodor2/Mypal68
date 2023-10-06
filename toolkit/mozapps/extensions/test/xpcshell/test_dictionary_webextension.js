/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */
"use strict";

XPCOMUtils.defineLazyServiceGetter(
  this,
  "spellCheck",
  "@mozilla.org/spellchecker/engine;1",
  "mozISpellCheckingEngine"
);

add_task(async function setup() {
  createAppInfo("xpcshell@tests.mozilla.org", "XPCShell", "61", "61");

  // Initialize the URLPreloader so that we can load the built-in
  // add-ons list, which contains the list of built-in dictionaries.
  AddonTestUtils.initializeURLPreloader();

  await promiseStartupManager();
});

add_task(async function test_validation() {
  await Assert.rejects(
    promiseInstallWebExtension({
      manifest: {
        applications: {
          gecko: { id: "en-US-no-dic@dictionaries.mozilla.org" },
        },
        dictionaries: {
          "en-US": "en-US.dic",
        },
      },
    }),
    /Expected file to be downloaded for install/
  );

  await Assert.rejects(
    promiseInstallWebExtension({
      manifest: {
        applications: {
          gecko: { id: "en-US-no-aff@dictionaries.mozilla.org" },
        },
        dictionaries: {
          "en-US": "en-US.dic",
        },
      },

      files: {
        "en-US.dic": "",
      },
    }),
    /Expected file to be downloaded for install/
  );

  let addon = await promiseInstallWebExtension({
    manifest: {
      applications: { gecko: { id: "en-US-1@dictionaries.mozilla.org" } },
      dictionaries: {
        "en-US": "en-US.dic",
      },
    },

    files: {
      "en-US.dic": "",
      "en-US.aff": "",
    },
  });

  let addon2 = await promiseInstallWebExtension({
    manifest: {
      applications: { gecko: { id: "en-US-2@dictionaries.mozilla.org" } },
      dictionaries: {
        "en-US": "dictionaries/en-US.dic",
      },
    },

    files: {
      "dictionaries/en-US.dic": "",
      "dictionaries/en-US.aff": "",
    },
  });

  await addon.uninstall();
  await addon2.uninstall();
});

const WORD = "Flehgragh";

add_task(async function test_registration() {
  spellCheck.dictionary = "en-US";

  ok(!spellCheck.check(WORD), "Word should not pass check before add-on loads");

  let addon = await promiseInstallWebExtension({
    manifest: {
      applications: { gecko: { id: "en-US@dictionaries.mozilla.org" } },
      dictionaries: {
        "en-US": "en-US.dic",
      },
    },

    files: {
      "en-US.dic": `2\n${WORD}\nnativ/A\n`,
      "en-US.aff": `
SFX A Y 1
SFX A   0       en         [^elr]
      `,
    },
  });

  ok(
    spellCheck.check(WORD),
    "Word should pass check while add-on load is loaded"
  );
  ok(spellCheck.check("nativen"), "Words should have correct affixes");

  await addon.uninstall();

  await new Promise(executeSoon);

  ok(
    !spellCheck.check(WORD),
    "Word should not pass check after add-on unloads"
  );
});
