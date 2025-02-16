/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * https://html.spec.whatwg.org/multipage/#the-hashchangeevent-interface
 */

[LegacyEventInit,
 Exposed=Window]
interface HashChangeEvent : Event
{
  constructor(DOMString type, optional HashChangeEventInit eventInitDict = {});

  readonly attribute DOMString oldURL;
  readonly attribute DOMString newURL;

  void initHashChangeEvent(DOMString typeArg,
                           optional boolean canBubbleArg = false,
                           optional boolean cancelableArg = false,
                           optional DOMString oldURLArg = "",
                           optional DOMString newURLArg = "");
};

dictionary HashChangeEventInit : EventInit
{
  DOMString oldURL = "";
  DOMString newURL = "";
};
