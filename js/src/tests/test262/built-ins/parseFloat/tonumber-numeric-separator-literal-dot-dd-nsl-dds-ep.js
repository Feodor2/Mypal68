// |reftest| skip-if(release_or_beta) -- numeric-separator-literal is not released yet
// Copyright (C) 2017 the V8 project authors. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-tonumber-applied-to-the-string-type
description: >
  The NSL does not affect strings parsed by parseFloat - . StrDecimalDigits StrExponentPart
info: |
  StrUnsignedDecimalLiteral :::
    . StrDecimalDigits StrExponentPart

  StrDecimalDigits :::
    DecimalDigit
    ...
    StrDecimalDigits DecimalDigit

features: [numeric-separator-literal]
---*/

assert.sameValue(parseFloat(".1_01e2"), .1);

reportCompare(0, 0);
