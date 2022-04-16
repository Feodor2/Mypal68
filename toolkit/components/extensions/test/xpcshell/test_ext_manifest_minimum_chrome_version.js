"use strict";

add_task(async function test_manifest_minimum_chrome_version() {
  let normalized = await ExtensionTestUtils.normalizeManifest({
    minimum_chrome_version: "42",
  });

  equal(normalized.error, undefined, "Should not have an error");
  equal(normalized.errors.length, 0, "Should not have warnings");
});
