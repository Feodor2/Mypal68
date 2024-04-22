/* Any copyright is dedicated to the Public Domain.
   http://creativecommons.org/publicdomain/zero/1.0/ */
/* eslint-disable no-shadow, max-nested-callbacks */

"use strict";

/**
 * Check that stepping out of a function returns the right return value.
 */

async function invokeAndPause({ global, threadFront }, expression) {
  return executeOnNextTickAndWaitForPause(
    () => Cu.evalInSandbox(expression, global),
    threadFront
  );
}

async function step(threadFront, cmd) {
  return cmd(threadFront);
}

function getPauseLocation(packet) {
  const { line, column } = packet.frame.where;
  return { line, column };
}

function getFrameFinished(packet) {
  return packet.why.frameFinished;
}

async function steps(threadFront, sequence) {
  const locations = [];
  for (const cmd of sequence) {
    const packet = await step(threadFront, cmd);
    locations.push(getPauseLocation(packet));
  }
  return locations;
}

async function testFinish({ threadFront, debuggerClient }) {
  await resume(threadFront);
  await close(debuggerClient);

  do_test_finished();
}

async function testRet(dbg) {
  let packet;
  const { threadFront } = dbg;

  info(`1. Test returning from doRet via stepping over`);
  await invokeAndPause(dbg, `doRet()`);
  await steps(threadFront, [stepOver, stepIn]);
  packet = await stepOver(threadFront);

  deepEqual(
    getPauseLocation(packet),
    { line: 6, column: 0 },
    `completion location in doRet`
  );
  deepEqual(getFrameFinished(packet), { return: 2 }, `completion value`);

  await resume(threadFront);

  info(`2. Test leaving from doRet via stepping out`);
  await invokeAndPause(dbg, `doRet()`);
  await steps(threadFront, [stepOver, stepIn]);

  packet = await stepOut(threadFront);

  deepEqual(
    getPauseLocation(packet),
    { line: 15, column: 2 },
    `completion location in doThrow`
  );

  deepEqual(
    getFrameFinished(packet),
    { return: 2 },
    `completion completion value`
  );

  await resume(threadFront);
}

async function testThrow(dbg) {
  let packet;
  const { threadFront } = dbg;

  info(`3. Test leaving from doThrow via stepping over`);
  await invokeAndPause(dbg, `doThrow()`);
  await steps(threadFront, [stepOver, stepIn]);
  packet = await step(threadFront, stepOver);

  deepEqual(
    getPauseLocation(packet),
    { line: 9, column: 8 },
    `completion location in doThrow`
  );

  deepEqual(
    getFrameFinished(packet).throw.class,
    "Error",
    `completion value class`
  );
  deepEqual(
    getFrameFinished(packet).throw.preview.message,
    "yo",
    `completion value preview`
  );

  await resume(threadFront);

  info(`4. Test leaving from doThrow via stepping out`);
  await invokeAndPause(dbg, `doThrow()`);
  await steps(threadFront, [stepOver, stepIn]);

  packet = await stepOut(threadFront);
  deepEqual(
    getPauseLocation(packet),
    { line: 23, column: 0 },
    `stepOut location in doThrow`
  );

  deepEqual(
    getFrameFinished(packet),
    { return: { type: "undefined" } },
    `completion type`
  );
}

function run_test() {
  return (async function() {
    const dbg = await setupTestFromUrl("completions.js");

    await testRet(dbg);
    await testThrow(dbg);

    await testFinish(dbg);
  })();
}
