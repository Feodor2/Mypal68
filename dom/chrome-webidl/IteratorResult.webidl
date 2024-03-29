/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/**
 * A dictionary which represents the result of a call to a next() method on a
 * JS iterator object.
 */
[GenerateConversionToJS, GenerateInit]
dictionary IteratorResult {
  required boolean done;
  any value;
};
