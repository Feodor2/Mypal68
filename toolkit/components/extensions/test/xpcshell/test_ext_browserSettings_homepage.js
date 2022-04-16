"use strict";

add_task(async function test_homepage_get_without_set() {
  async function background() {
    let homepage = await browser.browserSettings.homepageOverride.get({});
    browser.test.sendMessage("homepage", homepage);
  }

  let extension = ExtensionTestUtils.loadExtension({
    background,
    manifest: {
      permissions: ["browserSettings"],
    },
  });

  let defaultHomepage = Services.prefs.getStringPref(
    "browser.startup.homepage"
  );

  await extension.startup();
  let homepage = await extension.awaitMessage("homepage");
  equal(
    homepage.value,
    defaultHomepage,
    "The homepageOverride setting has the expected value."
  );
  equal(
    homepage.levelOfControl,
    "not_controllable",
    "The homepageOverride setting has the expected levelOfControl."
  );

  await extension.unload();
});
