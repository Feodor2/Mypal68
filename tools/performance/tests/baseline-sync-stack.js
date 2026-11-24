/* Exercise nested expressions and observable evaluation order during Baseline compilation. */
"use strict";

const { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");
if (!Services.prefs.getBoolPref("javascript.options.baselinejit") ||
    Services.prefs.getIntPref("javascript.options.baselinejit.threshold") !== 100 ||
    Services.prefs.getIntPref("javascript.options.ion.threshold") <= 300) {
  throw new Error("Unexpected startup JIT preferences");
}

function makeNestedExpression(depth) {
  let expression = "read(" + (depth - 1) + ")";
  for (let index = depth - 2; index >= 0; index--) {
    expression = "read(" + index + ") + (" + expression + ")";
  }
  return new Function("read", "return " + expression + ";");
}

function checkDepth(depth) {
  const calculate = makeNestedExpression(depth);
  const expected = depth * (depth - 1) / 2;
  for (let iteration = 0; iteration < 300; iteration++) {
    let next = 0;
    const value = calculate(function(index) {
      if (index !== next++) {
        throw new Error("Incorrect evaluation order at depth " + depth);
      }
      return index;
    });
    if (value !== expected || next !== depth) {
      throw new Error("Incorrect result at depth " + depth);
    }
  }

  let next = 0;
  const stopAt = Math.floor(depth / 2);
  const sentinel = {};
  try {
    calculate(function(index) {
      if (index !== next++) {
        throw new Error("Incorrect exception order at depth " + depth);
      }
      if (index === stopAt) {
        throw sentinel;
      }
      return index;
    });
    throw new Error("Expected an exception at depth " + depth);
  } catch (error) {
    if (error !== sentinel || next !== stopAt + 1) {
      throw error;
    }
  }
}

for (const depth of [1, 2, 3, 8, 32, 96]) {
  checkDepth(depth);
}

function* generatorWithStack(input) {
  const item = { value: input };
  const values = [item.value, yield input + 1];
  try {
    yield values[0] + values[1];
  } finally {
    item.value++;
  }
  return item.value;
}

for (let iteration = 0; iteration < 300; iteration++) {
  const generator = generatorWithStack(iteration);
  const first = generator.next();
  const second = generator.next(5);
  const last = generator.next();
  if (first.done || first.value !== iteration + 1 ||
      second.done || second.value !== iteration + 5 ||
      !last.done || last.value !== iteration + 1) {
    throw new Error("Generator stack or resume result differs");
  }
}

print("BASELINE_SYNC_STACK_OK");
