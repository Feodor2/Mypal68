// |reftest| skip-if(!this.uneval)

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * Date: 28 August 2001
 *
 * SUMMARY: A [DontEnum] prop, if overridden, should appear in uneval().
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=90596
 *
 * NOTE: some inefficiencies in the test are made for the sake of readability.
 * Sorting properties alphabetically is done for definiteness in comparisons.
 */
//-----------------------------------------------------------------------------
var UBound = 0;
var BUGNUMBER = 90596;
var summary = 'A [DontEnum] prop, if overridden, should appear in uneval()';
var cnCOMMA = ',';
var cnLBRACE = '{';
var cnRBRACE = '}';
var cnLPAREN = '(';
var cnRPAREN = ')';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];
var obj = {};


status = inSection(1);
obj = {toString:9};
actual = uneval(obj);
expect = '({toString:9})';
addThis();

status = inSection(2);
obj = {hasOwnProperty:"Hi"};
actual = uneval(obj);
expect = '({hasOwnProperty:"Hi"})';
addThis();

status = inSection(3);
obj = {toString:9, hasOwnProperty:"Hi"};
actual = uneval(obj);
expect = '({toString:9, hasOwnProperty:"Hi"})';
addThis();

status = inSection(4);
obj = {prop1:1, toString:9, hasOwnProperty:"Hi"};
actual = uneval(obj);
expect = '({prop1:1, toString:9, hasOwnProperty:"Hi"})';
addThis();


// TRY THE SAME THING IN EVAL CODE
var s = '';

status = inSection(5);
s = 'obj = {toString:9}';
eval(s);
actual = uneval(obj);
expect = '({toString:9})';
addThis();

status = inSection(6);
s = 'obj = {hasOwnProperty:"Hi"}';
eval(s);
actual = uneval(obj);
expect = '({hasOwnProperty:"Hi"})';
addThis();

status = inSection(7);
s = 'obj = {toString:9, hasOwnProperty:"Hi"}';
eval(s);
actual = uneval(obj);
expect = '({toString:9, hasOwnProperty:"Hi"})';
addThis();

status = inSection(8);
s = 'obj = {prop1:1, toString:9, hasOwnProperty:"Hi"}';
eval(s);
actual = uneval(obj);
expect = '({prop1:1, toString:9, hasOwnProperty:"Hi"})';
addThis();


// TRY THE SAME THING IN FUNCTION CODE
function A()
{
  status = inSection(9);
  var s = 'obj = {toString:9}';
  eval(s);
  actual = uneval(obj);
  expect = '({toString:9})';
  addThis();
}
A();

function B()
{
  status = inSection(10);
  var s = 'obj = {hasOwnProperty:"Hi"}';
  eval(s);
  actual = uneval(obj);
  expect = '({hasOwnProperty:"Hi"})';
  addThis();
}
B();

function C()
{
  status = inSection(11);
  var s = 'obj = {toString:9, hasOwnProperty:"Hi"}';
  eval(s);
  actual = uneval(obj);
  expect = '({toString:9, hasOwnProperty:"Hi"})';
  addThis();
}
C();

function D()
{
  status = inSection(12);
  var s = 'obj = {prop1:1, toString:9, hasOwnProperty:"Hi"}';
  eval(s);
  actual = uneval(obj);
  expect = '({prop1:1, toString:9, hasOwnProperty:"Hi"})';
  addThis();
}
D();



//-----------------------------------------------------------------------------
test();
//-----------------------------------------------------------------------------



/*
 * Sort properties alphabetically -
 */
function addThis()
{
  statusitems[UBound] = status;
  actualvalues[UBound] = sortThis(actual);
  expectedvalues[UBound] = sortThis(expect);
  UBound++;
}


/*
 * Takes string of form '({"c", "b", "a", 2})' and returns '({"a","b","c",2})'
 */
function sortThis(sList)
{
  sList = compactThis(sList);
  sList = stripParens(sList);
  sList = stripBraces(sList);
  var arr = sList.split(cnCOMMA);
  arr = arr.sort();
  var ret = String(arr);
  ret = addBraces(ret);
  ret = addParens(ret);
  return ret;
}


/*
 * Strips out any whitespace from the text -
 */
function compactThis(text)
{
  var charCode = 0;
  var ret = '';

  for (var i=0; i<text.length; i++)
  {
    charCode = text.charCodeAt(i);

    if (!isWhiteSpace(charCode))
      ret += text.charAt(i);
  }

  return ret;
}


function isWhiteSpace(charCode)
{
  switch (charCode)
  {
  case (0x0009):
  case (0x000B):
  case (0x000C):
  case (0x0020):
  case (0x000A):  // '\n'
  case (0x000D):  // '\r'
    return true;
    break;

  default:
    return false;
  }
}


/*
 * strips off parens at beginning and end of text -
 */
function stripParens(text)
{
  // remember to escape the parens...
  var arr = text.match(/^\((.*)\)$/);

  // defend against a null match...
  if (arr != null && arr[1] != null)
    return arr[1];
  return text;
}


/*
 * strips off braces at beginning and end of text -
 */
function stripBraces(text)
{
  // remember to escape the braces...
  var arr = text.match(/^\{(.*)\}$/);

  // defend against a null match...
  if (arr != null && arr[1] != null)
    return arr[1];
  return text;
}


function addBraces(text)
{
  return cnLBRACE + text + cnRBRACE;
}


function addParens(text)
{
  return cnLPAREN + text + cnRPAREN;
}


function test()
{
  printBugNumber(BUGNUMBER);
  printStatus (summary);

  for (var i=0; i<UBound; i++)
  {
    reportCompare(expectedvalues[i], actualvalues[i], statusitems[i]);
  }
}
