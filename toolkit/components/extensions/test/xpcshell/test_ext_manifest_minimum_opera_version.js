"use strict";

add_task(async function test_manifest_minimum_opera_version() {
  let normalized = await ExtensionTestUtils.normalizeManifest({
    minimum_opera_version: "48",
  });

  equal(normalized.error, undefined, "Should not have an error");
  equal(normalized.errors.length, 0, "Should not have warnings");
});
