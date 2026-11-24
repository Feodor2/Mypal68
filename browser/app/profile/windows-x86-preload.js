// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Overlap eligible script work on the 32-bit Windows browser build.
#if defined(XP_WIN) && !defined(HAVE_64BIT_BUILD)
pref("dom.script_loader.external_scripts.speculative_omt_parse.enabled", true);
#endif
