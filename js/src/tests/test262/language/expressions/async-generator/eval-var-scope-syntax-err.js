// This file was procedurally generated from the following sources:
// - src/function-forms/eval-var-scope-syntax-err.case
// - src/function-forms/error-no-strict/async-gen-func-expr.template
/*---
description: sloppy direct eval in params introduces var (async generator function expression in sloppy code)
esid: sec-asyncgenerator-definitions-evaluation
features: [default-parameters, async-iteration]
flags: [generated, noStrict]
info: |
    AsyncGeneratorExpression : async [no LineTerminator here] function * ( FormalParameters ) {
        AsyncGeneratorBody }

        [...]
        3. Let closure be ! AsyncGeneratorFunctionCreate(Normal, FormalParameters,
           AsyncGeneratorBody, scope, strict).
        [...]



    Runtime Semantics: IteratorBindingInitialization
    FormalParameter : BindingElement

    1. Return the result of performing IteratorBindingInitialization for BindingElement with arguments iteratorRecord and environment.

---*/


var callCount = 0;
var f;
f = async function*(a = eval("var a = 42")) {

  callCount = callCount + 1;
};

assert.throws(SyntaxError, function() {
  f();
});
assert.sameValue(callCount, 0, 'generator function body not evaluated');

reportCompare(0, 0);
