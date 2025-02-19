/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * For more information on this interface, please see
 * http://www.whatwg.org/specs/web-apps/current-work/#channel-messaging
 */

[Exposed=(Window,Worker)]
interface MessageChannel {
  [Throws]
  constructor();

  readonly attribute MessagePort port1;
  readonly attribute MessagePort port2;
};
