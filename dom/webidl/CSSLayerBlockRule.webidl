/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * https://drafts.csswg.org/css-cascade-5/#the-csslayerblockrule-interface
 */
[Exposed=Window, Pref="layout.css.cascade-layers.enabled"]
interface CSSLayerBlockRule : CSSGroupingRule {
  readonly attribute UTF8String name;
};
