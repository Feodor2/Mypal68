/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.sqlite;

import org.mozilla.gecko.annotation.JNITarget;

@JNITarget
public class SQLiteBridgeException extends RuntimeException {
    static final long serialVersionUID = 1L;

    public SQLiteBridgeException() {}
    public SQLiteBridgeException(final String msg) {
        super(msg);
    }
}
