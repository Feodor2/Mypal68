/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FFmpegLog_h__
#define __FFmpegLog_h__

#include "mozilla/Logging.h"

#define FFMPEG_LOG(...) \
  MOZ_LOG(sPDMLog, mozilla::LogLevel::Debug, (__VA_ARGS__))

#endif  // __FFmpegLog_h__
