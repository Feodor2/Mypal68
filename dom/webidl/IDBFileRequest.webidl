/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

[Exposed=Window]
interface IDBFileRequest : DOMRequest {
  readonly attribute IDBFileHandle? fileHandle;
  // this is deprecated due to renaming in the spec
  readonly attribute IDBFileHandle? lockedFile; // now fileHandle

  attribute EventHandler onprogress;
};
