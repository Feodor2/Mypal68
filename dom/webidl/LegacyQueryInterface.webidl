/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 */

interface nsISupports;

[NoInterfaceObject,
 // Need Exposed here, because this is a mixin onto things like Event
 // that are exposed in workers.
 Exposed=(Window,Worker)]
interface LegacyQueryInterface {
  // Legacy QueryInterface, only exposed to chrome code on the main thread.
  [Exposed=Window, ChromeOnly]
  nsISupports QueryInterface(any iid);
};

DOMParser implements LegacyQueryInterface;
Document implements LegacyQueryInterface;
DocumentFragment implements LegacyQueryInterface;
Element implements LegacyQueryInterface;
Event implements LegacyQueryInterface;
Selection implements LegacyQueryInterface;
TreeColumns implements LegacyQueryInterface;
TreeContentView implements LegacyQueryInterface;
Window implements LegacyQueryInterface;
XMLHttpRequest implements LegacyQueryInterface;
