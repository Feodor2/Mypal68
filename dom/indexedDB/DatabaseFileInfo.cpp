/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "DatabaseFileInfo.h"

#include "FileInfoImpl.h"

namespace mozilla::dom::indexedDB {

template class FileInfo<DatabaseFileManager>;

}  // namespace mozilla::dom::indexedDB
