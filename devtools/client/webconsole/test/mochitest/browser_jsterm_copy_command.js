/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

// Tests that the `copy` console helper works as intended.

"use strict";

const text =
  "Lorem ipsum dolor sit amet, consectetur adipisicing " +
  "elit, sed do eiusmod tempor incididunt ut labore et dolore magna " +
  "aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco " +
  "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure " +
  "dolor in reprehenderit in voluptate velit esse cillum dolore eu " +
  "fugiat nulla pariatur. Excepteur sint occaecat cupidatat non " +
  "proident, sunt in culpa qui officia deserunt mollit anim id est laborum." +
  new Date();

const id = "select-me";
const TEST_URI = `data:text/html;charset=utf-8,
<body>
  <div>
    <h1>Testing copy command</h1>
    <p>This is some example text</p>
    <p id="${id}">${text}</p>
  </div>
  <div><p></p></div>
</body>`;

add_task(async function() {
  // Run test with legacy JsTerm
  await pushPref("devtools.webconsole.jsterm.codeMirror", false);
  await performTests();
  // And then run it with the CodeMirror-powered one.
  await pushPref("devtools.webconsole.jsterm.codeMirror", true);
  await performTests();
});

async function performTests() {
  const { jsterm } = await openNewTabAndConsole(TEST_URI);
  const random = Math.random();
  const string = "Text: " + random;
  const obj = { a: 1, b: "foo", c: random };

  await testCopy(jsterm, random, random.toString());
  await testCopy(jsterm, JSON.stringify(string), string);
  await testCopy(jsterm, obj.toSource(), JSON.stringify(obj, null, "  "));

  const outerHTML = await ContentTask.spawn(
    gBrowser.selectedBrowser,
    id,
    function(elementId) {
      return content.document.getElementById(elementId).outerHTML;
    }
  );
  await testCopy(jsterm, `$("#${id}")`, outerHTML);
}

function testCopy(jsterm, stringToCopy, expectedResult) {
  return waitForClipboardPromise(() => {
    info(`Attempting to copy: "${stringToCopy}"`);
    const command = `copy(${stringToCopy})`;
    info(`Executing command: "${command}"`);
    jsterm.execute(command);
  }, expectedResult);
}
