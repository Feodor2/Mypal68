/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//-----------------------------------------------------------------------------
var BUGNUMBER = 462879;
var summary = 'Do not assert: UPVAR_FRAME_SKIP(uva->vector[i]) == 1';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
test();
//-----------------------------------------------------------------------------

function test()
{
  printBugNumber(BUGNUMBER);
  printStatus (summary);

  try
  {
    (function(c){eval("eval('c.x')")})();
  }
  catch(ex)
  {
  }
 
  reportCompare(expect, actual, summary);
}
