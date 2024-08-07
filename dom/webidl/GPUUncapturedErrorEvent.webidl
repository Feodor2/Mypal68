/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * https://gpuweb.github.io/gpuweb/
 */

[
    Pref="dom.webgpu.enable",
    Exposed=Window,
]
interface GPUUncapturedErrorEvent : Event {
    constructor(DOMString type, GPUUncapturedErrorEventInit gpuUncapturedErrorEventInitDict);
    readonly attribute GPUError error;
};

dictionary GPUUncapturedErrorEventInit : EventInit {
    required GPUError error;
};
