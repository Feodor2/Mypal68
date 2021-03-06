/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.dlc;

import android.content.Context;
import android.support.annotation.VisibleForTesting;

import org.mozilla.gecko.dlc.catalog.DownloadContent;
import org.mozilla.gecko.dlc.catalog.DownloadContentCatalog;

import java.io.File;

/**
 * CleanupAction: Remove content that is no longer needed.
 */
public class CleanupAction extends BaseAction {
    @Override
    public void perform(Context context, DownloadContentCatalog catalog) {
        for (DownloadContent content : catalog.getContentToDelete()) {
            if (!content.isAssetArchive()) {
                continue; // We do not know how to clean up this content. But this means we didn't
                          // download it anyways.
            }

            cleanupContent(context, catalog, content);
        }
    }

    @VisibleForTesting void cleanupContent(Context context, DownloadContentCatalog catalog, DownloadContent content) {
        try {
            final File file = getDestinationFile(context, content);

            if (!file.exists()) {
                // File does not exist. As good as deleting.
                catalog.remove(content);
                return;
            }

            if (file.delete()) {
                // File has been deleted. Now remove it from the catalog.
                catalog.remove(content);
            }
        } catch (UnrecoverableDownloadContentException e) {
            // We can't recover. Pretend the content is removed. It probably never existed in
            // the first place.
            catalog.remove(content);
        } catch (RecoverableDownloadContentException e) {
            // Try again next time.
        }
    }
}
