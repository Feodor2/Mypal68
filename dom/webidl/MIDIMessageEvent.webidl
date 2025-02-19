/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * https://webaudio.github.io/web-midi-api/
 */

[SecureContext,
 Pref="dom.webmidi.enabled",
 Exposed=Window]
interface MIDIMessageEvent : Event
{
  [Throws]
  constructor(DOMString type, optional MIDIMessageEventInit eventInitDict = {});

  [Throws]
  readonly attribute Uint8Array  data;
};

dictionary MIDIMessageEventInit : EventInit
{
  Uint8Array  data;
};
