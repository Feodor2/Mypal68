// |reftest| skip-if((function(){try{eval('c=class{x;}');return(false);}catch{return(true);}})()) error:SyntaxError -- class-fields-public is not enabled unconditionally
// This file was procedurally generated from the following sources:
// - src/class-elements/init-err-contains-super.case
// - src/class-elements/initializer-error/cls-decl-fields-arrow-fnc.template
/*---
description: Syntax error if `super()` used in class field (arrow function expression)
esid: sec-class-definitions-static-semantics-early-errors
features: [class, class-fields-public, arrow-function]
flags: [generated]
negative:
  phase: parse
  type: SyntaxError
info: |
    Static Semantics: Early Errors

      FieldDefinition:
        PropertyNameInitializeropt

      - It is a Syntax Error if Initializer is present and Initializer Contains SuperCall is true.

---*/


$DONOTEVALUATE();

class C {
  x = () => super();
}
