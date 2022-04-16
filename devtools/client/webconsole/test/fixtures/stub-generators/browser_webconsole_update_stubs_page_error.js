/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";

ChromeUtils.import("resource://gre/modules/osfile.jsm", {});

add_task(async function() {
  const fileContent = await generatePageErrorStubs();
  const filePath = OS.Path.join(`${BASE_PATH}/stubs`, "pageError.js");
  await OS.File.writeAtomic(filePath, fileContent);
  ok(true, "Make the test not fail");
});
