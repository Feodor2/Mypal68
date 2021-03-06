/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.icons;

import android.content.Context;
import android.support.annotation.VisibleForTesting;

import org.mozilla.gecko.R;

import java.util.Iterator;
import java.util.TreeSet;
import java.util.concurrent.Future;

/**
 * A class describing a request to load an icon for a website.
 */
public class IconRequest {
    private Context context;

    // Those values are written by the IconRequestBuilder class.
    /* package-private */ String pageUrl;
    /* package-private */ boolean privileged;
    /* package-private */ boolean isPrivate;
    /* package-private */ TreeSet<IconDescriptor> icons;
    /* package-private */ boolean skipNetwork;
    /* package-private */ boolean backgroundThread;
    /* package-private */ boolean skipDisk;
    /* package-private */ boolean skipMemory;
    /* package-private */ int targetSize;
    /* package-private */ int minimumSizePxAfterScaling;
    /* package-private */ boolean prepareOnly;
    /* package-private */ float textSize;
    private IconCallback callback;

    /* package-private */ IconRequest(Context context) {
        this.context = context.getApplicationContext();
        this.icons = new TreeSet<>(new IconDescriptorComparator());

        // Setting some sensible defaults.
        this.privileged = false;
        this.isPrivate = false;
        this.skipMemory = false;
        this.skipDisk = false;
        this.skipNetwork = false;
        this.targetSize = context.getResources().getDimensionPixelSize(R.dimen.favicon_bg);
        this.minimumSizePxAfterScaling = 0;
        this.prepareOnly = false;

        // textSize is only used in IconGenerator.java for creating a icon with specific text size.
        this.textSize = 0;
    }

    /**
     * Execute this request and try to load an icon. Once an icon has been loaded successfully the
     * callback will be executed.
     *
     * The returned Future can be used to cancel the job.
     */
    public Future<IconResponse> execute(IconCallback callback) {
        setCallback(callback);

        return IconRequestExecutor.submit(this);
    }

    @VisibleForTesting void setCallback(IconCallback callback) {
        this.callback = callback;
    }

    /**
     * Get the (application) context associated with this request.
     */
    public Context getContext() {
        return context;
    }

    /**
     * Get the descriptor for the potentially best icon. This is the icon that should be loaded if
     * possible.
     */
    public IconDescriptor getBestIcon() {
        return icons.first();
    }

    /**
     * Get the URL of the page for which an icon should be loaded.
     */
    public String getPageUrl() {
        return pageUrl;
    }

    /**
     * Is this request allowed to load icons from internal data sources like the omni.ja?
     */
    public boolean isPrivileged() {
        return privileged;
    }

    /**
     * Is this request initiated from a tab in private browsing mode?
     */
    public boolean isPrivateMode() {
        return isPrivate;
    }

    /**
     * Get the number of icon descriptors associated with this request.
     */
    public int getIconCount() {
        return icons.size();
    }

    /**
     * Get the required target size of the icon.
     */
    public int getTargetSize() {
        return targetSize;
    }

    /**
     * Gets the minimum size the icon can be before we substitute a generated icon.
     *
     * N.B. the minimum size is compared to the icon *after* scaling: consider using
     * {@link org.mozilla.gecko.icons.processing.ResizingProcessor#MAX_SCALE_FACTOR}
     * when setting this value.
     */
    public int getMinimumSizePxAfterScaling() {
        return minimumSizePxAfterScaling;
    }

    /**
     * Should a loader access the network to load this icon?
     */
    public boolean shouldSkipNetwork() {
        return skipNetwork;
    }

    /**
     * Should a loader access the disk to load this icon?
     */
    public boolean shouldSkipDisk() {
        return skipDisk;
    }

    /**
     * Should a loader access the memory cache to load this icon?
     */
    public boolean shouldSkipMemory() {
        return skipMemory;
    }

    /**
     * Get an iterator to iterate over all icon descriptors associated with this request.
     */
    public Iterator<IconDescriptor> getIconIterator() {
        return icons.iterator();
    }

    /**
     * Get the required text size of the icon created by
     * {@link org.mozilla.gecko.icons.loader.IconGenerator}.
     */
    public float getTextSize() {
        return textSize;
    }

    /**
     * Create a builder to modify this request.
     *
     * Calling methods on the builder will modify this object and not create a copy.
     */
    public IconRequestBuilder modify() {
        return new IconRequestBuilder(this);
    }

    /**
     * Should the callback be executed on a background thread? By default a callback is always
     * executed on the UI thread because an icon is usually loaded in order to display it somewhere
     * in the UI.
     */
    /* package-private */ boolean shouldRunOnBackgroundThread() {
        return backgroundThread;
    }

    /* package-private */ IconCallback getCallback() {
        return callback;
    }

    /* package-private */ boolean hasIconDescriptors() {
        return !icons.isEmpty();
    }

    /**
     * Move to the next icon. This method is called after all loaders for the current best icon
     * have failed. After calling this method getBestIcon() will return the next icon to try.
     * hasIconDescriptors() should be called before requesting the next icon.
     */
    /* package-private */ void moveToNextIcon() {
        if (!icons.remove(getBestIcon())) {
            // Calling this method when there's no next icon is an error (use hasIconDescriptors()).
            // Theoretically this method can fail even if there's a next icon (like it did in bug 1331808).
            // In this case crashing to see and fix the issue is desired.
            throw new IllegalStateException("Moving to next icon failed. Could not remove first icon from set.");
        }
    }

    /**
     * Should this request be prepared but not actually load an icon?
     */
    /* package-private */ boolean shouldPrepareOnly() {
        return prepareOnly;
    }
}
