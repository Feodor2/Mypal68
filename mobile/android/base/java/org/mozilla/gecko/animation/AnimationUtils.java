/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */


package org.mozilla.gecko.animation;

import android.content.Context;

public class AnimationUtils {
    private static long mShortDuration = -1;

    public static long getShortDuration(Context context) {
        if (mShortDuration < 0) {
            mShortDuration = context.getResources().getInteger(android.R.integer.config_shortAnimTime);
        }
        return mShortDuration;
    }
}

