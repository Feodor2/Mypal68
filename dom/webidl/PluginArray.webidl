/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 */

[LegacyUnenumerableNamedProperties,
 Exposed=Window]
interface PluginArray {
  [NeedsCallerType]
  readonly attribute unsigned long length;

  [NeedsCallerType]
  getter Plugin? item(unsigned long index);
  [NeedsCallerType]
  getter Plugin? namedItem(DOMString name);

  void refresh(optional boolean reloadDocuments = false);
};
