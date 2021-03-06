/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//-----------------------------------------------------------------------------
var BUGNUMBER = 309925;
var summary = 'Correctly parse Date strings with HH:MM(comment)';
var actual = new Date('Sep 24, 11:58(comment) 105') + '';
var expect = new Date('Sep 24, 11:58:00 (comment) 105') + '';

printBugNumber(BUGNUMBER);
printStatus (summary);
 
reportCompare(expect, actual, summary);
