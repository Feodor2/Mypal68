// This file was procedurally generated from the following sources:
// - src/direct-eval-code/declare-arguments.case
// - src/direct-eval-code/default/gen-func-decl/a-preceding-parameter-is-named-arguments.template
/*---
description: Declare "arguments" and assign to it in direct eval code (Declare |arguments| when a preceding parameter is named |arguments|.)
esid: sec-evaldeclarationinstantiation
flags: [generated, noStrict]
---*/



assert.sameValue("arguments" in this, false, "No global 'arguments' binding");

function * f(arguments, p = eval("var arguments")) {

}
assert.throws(SyntaxError, f);

assert.sameValue("arguments" in this, false, "No global 'arguments' binding");

reportCompare(0, 0);
