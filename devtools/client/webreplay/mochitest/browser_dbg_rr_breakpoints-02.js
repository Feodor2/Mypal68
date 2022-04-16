/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
/* eslint-disable no-undef */

"use strict";

// Test unhandled divergence while evaluating at a breakpoint with Web Replay.
add_task(async function() {
  const dbg = await attachRecordingDebugger("doc_rr_basic.html", {
    waitForRecording: true,
  });
  const { threadClient, tab, toolbox, target } = dbg;

  const bp = await setBreakpoint(threadClient, "doc_rr_basic.html", 21);
  await rewindToLine(threadClient, 21);
  await checkEvaluateInTopFrame(target, "number", 10);
  await checkEvaluateInTopFrameThrows(target, "window.alert(3)");
  await checkEvaluateInTopFrame(target, "number", 10);
  await checkEvaluateInTopFrameThrows(target, "window.alert(3)");
  await checkEvaluateInTopFrame(target, "number", 10);
  await checkEvaluateInTopFrame(target, "testStepping2()", undefined);

  await threadClient.removeBreakpoint(bp);
  await toolbox.destroy();
  await gBrowser.removeTab(tab);
});
