/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_recordreplay_WeakPointer_h
#define mozilla_recordreplay_WeakPointer_h

namespace mozilla {
namespace recordreplay {

// See RecordReplay.h for a description of the record/replay weak pointer API.

// Initialize weak pointer state.
void InitializeWeakPointers();

}  // namespace recordreplay
}  // namespace mozilla

#endif  // mozilla_recordreplay_WeakPointer_h
