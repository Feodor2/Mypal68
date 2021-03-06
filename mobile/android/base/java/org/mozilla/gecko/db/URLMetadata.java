/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
package org.mozilla.gecko.db;

import java.util.Collection;
import java.util.List;
import java.util.Map;

import org.json.JSONObject;

import org.mozilla.gecko.annotation.RobocopTarget;

import android.content.ContentResolver;

@RobocopTarget
public interface URLMetadata {
    public Map<String, Object> fromJSON(JSONObject obj);
    public Map<String, Map<String, Object>> getForURLs(final ContentResolver cr,
                                                       final Collection<String> urls,
                                                       final List<String> columns);
    public void save(final ContentResolver cr, final Map<String, Object> data);
}
