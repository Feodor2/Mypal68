/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.geckoview;

import android.support.annotation.AnyThread;

/**
 * This represents a decision to allow or deny a request.
 */
@AnyThread
public enum AllowOrDeny {
    ALLOW, DENY;
}
