/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_recordreplay_DirtyMemoryHandler_h
#define mozilla_recordreplay_DirtyMemoryHandler_h

namespace mozilla {
namespace recordreplay {

// Set up a handler to catch SEGV hardware exceptions and pass them on to
// HandleDirtyMemoryFault in MemorySnapshot.h for handling.
void SetupDirtyMemoryHandler();

}  // namespace recordreplay
}  // namespace mozilla

#endif  // mozilla_recordreplay_DirtyMemoryHandler_h
