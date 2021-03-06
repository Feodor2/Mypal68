/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//-----------------------------------------------------------------------------
var BUGNUMBER = 311071;
var summary = 'treat &lt;! as the start of a comment to end of line';
var actual = '';
var expect = '';


printBugNumber(BUGNUMBER);
printStatus (summary);

expect = 'foo';
actual = 'foo'; <!-- comment hack -->; actual = 'bar'; 
reportCompare(expect, actual, summary);
