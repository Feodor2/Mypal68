/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "DrawEventRecorder.h"

namespace mozilla {
namespace layout {

void DrawEventRecorderPRFileDesc::RecordEvent(
    const gfx::RecordedEvent& aEvent) {
  aEvent.RecordToStream(mOutputStream);

  Flush();
}

DrawEventRecorderPRFileDesc::~DrawEventRecorderPRFileDesc() {
  if (IsOpen()) {
    Close();
  }
}

void DrawEventRecorderPRFileDesc::Flush() { mOutputStream.Flush(); }

bool DrawEventRecorderPRFileDesc::IsOpen() { return mOutputStream.IsOpen(); }

void DrawEventRecorderPRFileDesc::OpenFD(PRFileDesc* aFd) {
  MOZ_DIAGNOSTIC_ASSERT(!IsOpen());

  mOutputStream.OpenFD(aFd);
  WriteHeader(mOutputStream);
}

void DrawEventRecorderPRFileDesc::Close() {
  MOZ_DIAGNOSTIC_ASSERT(IsOpen());

  mOutputStream.Close();
}

}  // namespace layout
}  // namespace mozilla
