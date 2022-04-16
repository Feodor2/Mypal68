/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_storage_FileSystemModule_h
#define mozilla_storage_FileSystemModule_h

#include "nscore.h"

struct sqlite3;

namespace mozilla {
namespace storage {

int RegisterFileSystemModule(sqlite3* aDB, const char* aName);

}  // namespace storage
}  // namespace mozilla

#endif  // mozilla_storage_FileSystemModule_h
