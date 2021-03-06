/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//-----------------------------------------------------------------------------
var BUGNUMBER = 355075;
var summary = 'Regression tests from bug 354750';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
test();
//-----------------------------------------------------------------------------

function test()
{
  printBugNumber(BUGNUMBER);
  printStatus (summary);

  options('strict');
  options('werror');
 
  function f() {
    this.a = {1: "a", 2: "b"};
    var dummy;
    for (var b in this.a)
      dummy = b;
  }

  f();

  reportCompare(expect, actual, summary);
}
