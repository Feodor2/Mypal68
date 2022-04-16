/* Any copyright is dedicated to the Public Domain.
   http://creativecommons.org/publicdomain/zero/1.0/ */

registerCleanupFunction(async function() {
  await task_resetState();
  await PlacesUtils.history.clear();
});

add_task(async function test_indicatorDrop() {
  await SpecialPowers.pushPrefEnv({
    set: [["browser.download.autohideButton", false]],
  });
  let downloadButton = document.getElementById("downloads-button");
  ok(downloadButton, "download button present");
  await promiseButtonShown(downloadButton.id);

  let EventUtils = {};
  Services.scriptloader.loadSubScript(
    "chrome://mochikit/content/tests/SimpleTest/EventUtils.js",
    EventUtils
  );

  // Ensure that state is reset in case previous tests didn't finish.
  await task_resetState();

  await setDownloadDir();

  startServer();

  await simulateDropAndCheck(window, downloadButton, [httpUrl("file1.txt")]);
  await simulateDropAndCheck(window, downloadButton, [
    httpUrl("file1.txt"),
    httpUrl("file2.txt"),
    httpUrl("file3.txt"),
  ]);
});
