/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 */

[Exposed=(Window,Worker)]
interface ProgressEvent : Event
{
  constructor(DOMString type, optional ProgressEventInit eventInitDict = {});

  readonly attribute boolean lengthComputable;
  readonly attribute unsigned long long loaded;
  readonly attribute unsigned long long total;
};

dictionary ProgressEventInit : EventInit
{
  boolean lengthComputable = false;
  unsigned long long loaded = 0;
  unsigned long long total = 0;
};
