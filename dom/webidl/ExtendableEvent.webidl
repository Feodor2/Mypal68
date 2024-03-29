/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * For more information on this interface, please see
 * http://slightlyoff.github.io/ServiceWorker/spec/service_worker/index.html
 */

[Exposed=ServiceWorker]
interface ExtendableEvent : Event {
  constructor(DOMString type, optional ExtendableEventInit eventInitDict = {});

  // https://github.com/slightlyoff/ServiceWorker/issues/261
  [Throws]
  void waitUntil(Promise<any> p);
};

dictionary ExtendableEventInit : EventInit {
  // Defined for the forward compatibility across the derived events
};
