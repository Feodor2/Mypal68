/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


/**
   File Name:          lexical-040.js
   Corresponds To:     7.5-2.js
   ECMA Section:       7.5 Identifiers
   Description:        Identifiers are of unlimited length
   - can contain letters, a decimal digit, _, or $
   - the first character cannot be a decimal digit
   - identifiers are case sensitive

   Author:             christine@netscape.com
   Date:               11 september 1997
*/
var SECTION = "lexical-040";
var VERSION = "JS1_4";
var TITLE   = "Identifiers";

startTest();
writeHeaderToLog( SECTION + " "+ TITLE);

var result = "Failed";
var exception = "No exception thrown";
var expect = "Passed";

try {
  eval("var 1abc;");
} catch ( e ) {
  result = expect;
  exception = e.toString();
}

new TestCase(
  SECTION,
  "var 1abc" +
  " (threw " + exception +")",
  expect,
  result );

test();
