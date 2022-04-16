/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "DDLogCategory.h"

namespace mozilla {

const char* const kDDLogCategoryShortStrings[kDDLogCategoryCount] = {
    "con", "dcn", "des", "lnk", "ulk", "prp", "evt",
    "api", "log", "mze", "mzw", "mzi", "mzd", "mzv"};
const char* const kDDLogCategoryLongStrings[kDDLogCategoryCount] = {
    "Construction",
    "Derived Construction",
    "Destruction",
    "Link",
    "Unlink",
    "Property",
    "Event",
    "API",
    "Log",
    "MozLog-Error",
    "MozLog-Warning",
    "MozLog-Info",
    "MozLog-Debug",
    "MozLog-Verbose"};

}  // namespace mozilla
