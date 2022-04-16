/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";

ChromeUtils.import("resource://gre/modules/osfile.jsm", {});

add_task(async function() {
  const generatedStubs = await generateConsoleApiStubs();

  const repoStubFilePath = getTestFilePath("../stubs/consoleApi.js");
  const repoStubFileContent = await OS.File.read(repoStubFilePath, {
    encoding: "utf-8",
  });

  if (generatedStubs != repoStubFileContent) {
    ok(
      false,
      "The consoleApi stubs file needs to be updated by running " +
        "`mach test devtools/client/webconsole/test/fixtures/" +
        "stub-generators/browser_webconsole_update_stubs_console_api.js`"
    );
  } else {
    ok(true, "The consoleApi stubs file is up to date");
  }
});
