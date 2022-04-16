/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.geckoview;

import android.support.annotation.AnyThread;
import android.support.annotation.Nullable;

/**
 * This is used to receive async responses from delegate methods.
 */
public interface GeckoResponse<T> {
    /**
     * Called when async processing has finished.
     *
     * @param value The value contained in the response.
     */
    @AnyThread
    void respond(@Nullable T value);
}
