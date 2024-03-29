/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 */

[GenerateInitFromJSON]
dictionary WidevineCDMManifest {
  required DOMString name;
  required DOMString description;
  required DOMString version;
  required DOMString x-cdm-module-versions;
  required DOMString x-cdm-interface-versions;
  required DOMString x-cdm-host-versions;
  required DOMString x-cdm-codecs;
};
