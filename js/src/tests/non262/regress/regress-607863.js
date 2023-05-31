// |reftest| fails-if(!xulRuntime.shell)
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 */

var sandbox = evalcx('');
var foreign = evalcx('({ get f() { return this; }, set x(v) { result = this } })', sandbox);
var local = Object.create(foreign);

reportCompare(local, local.f, "this should be set correctly in getters");
local.x = 42;
reportCompare(local, sandbox.result, "this should be set correctly in setters");
