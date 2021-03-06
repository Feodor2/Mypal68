/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.sqlite;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/*
 * Helper class to make the ByteBuffers returned by SQLite BLOB
 * easier to use.
 */
public class ByteBufferInputStream extends InputStream {
    private final ByteBuffer mByteBuffer;

    public ByteBufferInputStream(final ByteBuffer aByteBuffer) {
        mByteBuffer = aByteBuffer;
    }

    @Override
    public synchronized int read() throws IOException {
        if (!mByteBuffer.hasRemaining()) {
            return -1;
        }
        return mByteBuffer.get();
    }

    @Override
    public synchronized int read(final byte[] aBytes, final int aOffset, final int aLen)
            throws IOException {
        int toRead = Math.min(aLen, mByteBuffer.remaining());
        mByteBuffer.get(aBytes, aOffset, toRead);
        return toRead;
    }
}
