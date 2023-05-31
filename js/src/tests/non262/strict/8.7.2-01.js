// |reftest| skip-if(Android)
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 */

/* Check that assignment to a let-bound variable is permitted in both lenient and strict modes. */

/* Assigning to a let-declared variable is okay in strict and loose modes. */
assertEq(testLenientAndStrict('let let_declared; let_declared=1',
                              completesNormally,
                              completesNormally),
         true);

reportCompare(true, true);
