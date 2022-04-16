/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.util;

public final class FloatUtils {
    private FloatUtils() {}

    public static boolean fuzzyEquals(final float a, final float b) {
        return (Math.abs(a - b) < 1e-6);
    }
}
