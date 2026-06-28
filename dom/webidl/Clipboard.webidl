/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * https://w3c.github.io/clipboard-apis/
 *
 * Copyright © 2018 W3C® (MIT, ERCIM, Keio), All Rights Reserved. W3C
 * liability, trademark and document use rules apply.
 */

[SecureContext, Exposed=Window, Pref="dom.events.asyncClipboard"]
interface Clipboard : EventTarget {
  [Pref="dom.events.asyncClipboard.dataTransfer", NewObject, NeedsSubjectPrincipal]
  Promise<DataTransfer> read();
  [Func="Clipboard::ReadTextEnabled", NewObject, NeedsSubjectPrincipal]
  Promise<DOMString> readText();

  [Pref="dom.events.asyncClipboard.dataTransfer", NewObject, NeedsSubjectPrincipal]
  Promise<undefined> write(DataTransfer data);
  [NewObject, NeedsSubjectPrincipal]
  Promise<undefined> writeText(DOMString data);
};
