/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vm/DateTime.h"

#include "mozilla/ArrayUtils.h"
#include "mozilla/ScopeExit.h"
#include "mozilla/TextUtils.h"
#include "mozilla/Unused.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <time.h>

#if !defined(XP_WIN)
#  include <limits.h>
#  include <unistd.h>
#endif /* !defined(XP_WIN) */

#include "js/Date.h"
#include "threading/ExclusiveData.h"

#include "util/Text.h"
#include "vm/MutexIDs.h"

static bool ComputeLocalTime(time_t local, struct tm* ptm) {
#if defined(_WIN32)
  return localtime_s(ptm, &local) == 0;
#elif defined(HAVE_LOCALTIME_R)
  return localtime_r(&local, ptm);
#else
  struct tm* otm = localtime(&local);
  if (!otm) {
    return false;
  }
  *ptm = *otm;
  return true;
#endif
}

static bool ComputeUTCTime(time_t t, struct tm* ptm) {
#if defined(_WIN32)
  return gmtime_s(ptm, &t) == 0;
#elif defined(HAVE_GMTIME_R)
  return gmtime_r(&t, ptm);
#else
  struct tm* otm = gmtime(&t);
  if (!otm) {
    return false;
  }
  *ptm = *otm;
  return true;
#endif
}

/*
 * Compute the offset in seconds from the current UTC time to the current local
 * standard time (i.e. not including any offset due to DST).
 *
 * Examples:
 *
 * Suppose we are in California, USA on January 1, 2013 at 04:00 PST (UTC-8, no
 * DST in effect), corresponding to 12:00 UTC.  This function would then return
 * -8 * SecondsPerHour, or -28800.
 *
 * Or suppose we are in Berlin, Germany on July 1, 2013 at 17:00 CEST (UTC+2,
 * DST in effect), corresponding to 15:00 UTC.  This function would then return
 * +1 * SecondsPerHour, or +3600.
 */
static int32_t UTCToLocalStandardOffsetSeconds() {
  using js::SecondsPerDay;
  using js::SecondsPerHour;
  using js::SecondsPerMinute;

  // Get the current time.
  time_t currentMaybeWithDST = time(nullptr);
  if (currentMaybeWithDST == time_t(-1)) {
    return 0;
  }

  // Break down the current time into its (locally-valued, maybe with DST)
  // components.
  struct tm local;
  if (!ComputeLocalTime(currentMaybeWithDST, &local)) {
    return 0;
  }

  // Compute a |time_t| corresponding to |local| interpreted without DST.
  time_t currentNoDST;
  if (local.tm_isdst == 0) {
    // If |local| wasn't DST, we can use the same time.
    currentNoDST = currentMaybeWithDST;
  } else {
    // If |local| respected DST, we need a time broken down into components
    // ignoring DST.  Turn off DST in the broken-down time.  Create a fresh
    // copy of |local|, because mktime() will reset tm_isdst = 1 and will
    // adjust tm_hour and tm_hour accordingly.
    struct tm localNoDST = local;
    localNoDST.tm_isdst = 0;

    // Compute a |time_t t| corresponding to the broken-down time with DST
    // off.  This has boundary-condition issues (for about the duration of
    // a DST offset) near the time a location moves to a different time
    // zone.  But 1) errors will be transient; 2) locations rarely change
    // time zone; and 3) in the absence of an API that provides the time
    // zone offset directly, this may be the best we can do.
    currentNoDST = mktime(&localNoDST);
    if (currentNoDST == time_t(-1)) {
      return 0;
    }
  }

  // Break down the time corresponding to the no-DST |local| into UTC-based
  // components.
  struct tm utc;
  if (!ComputeUTCTime(currentNoDST, &utc)) {
    return 0;
  }

  // Finally, compare the seconds-based components of the local non-DST
  // representation and the UTC representation to determine the actual
  // difference.
  int utc_secs = utc.tm_hour * SecondsPerHour + utc.tm_min * SecondsPerMinute;
  int local_secs =
      local.tm_hour * SecondsPerHour + local.tm_min * SecondsPerMinute;

  // Same-day?  Just subtract the seconds counts.
  if (utc.tm_mday == local.tm_mday) {
    return local_secs - utc_secs;
  }

  // If we have more UTC seconds, move local seconds into the UTC seconds'
  // frame of reference and then subtract.
  if (utc_secs > local_secs) {
    return (SecondsPerDay + local_secs) - utc_secs;
  }

  // Otherwise we have more local seconds, so move the UTC seconds into the
  // local seconds' frame of reference and then subtract.
  return local_secs - (utc_secs + SecondsPerDay);
}

void js::DateTimeInfo::internalResetTimeZone(ResetTimeZoneMode mode) {
  // Nothing to do when an update request is already enqueued.
  if (timeZoneStatus_ == TimeZoneStatus::NeedsUpdate) {
    return;
  }

  // Mark the state as needing an update, but defer the actual update until it's
  // actually needed to delay any system calls to the last possible moment. This
  // is beneficial when this method is called during start-up, because it avoids
  // main-thread I/O blocking the process.
  if (mode == ResetTimeZoneMode::ResetEvenIfOffsetUnchanged) {
    timeZoneStatus_ = TimeZoneStatus::NeedsUpdate;
  } else {
    timeZoneStatus_ = TimeZoneStatus::UpdateIfChanged;
  }
}

void js::DateTimeInfo::updateTimeZone() {
  MOZ_ASSERT(timeZoneStatus_ != TimeZoneStatus::Valid);

  bool updateIfChanged = timeZoneStatus_ == TimeZoneStatus::UpdateIfChanged;

  timeZoneStatus_ = TimeZoneStatus::Valid;

  /*
   * The difference between local standard time and UTC will never change for
   * a given time zone.
   */
  int32_t newOffset = UTCToLocalStandardOffsetSeconds();

  if (updateIfChanged && newOffset == utcToLocalStandardOffsetSeconds_) {
    return;
  }

  utcToLocalStandardOffsetSeconds_ = newOffset;

  dstRange_.reset();
}

js::DateTimeInfo::DateTimeInfo() {
  // Set the time zone status into the invalid state, so we compute the actual
  // defaults on first access. We don't yet want to initialize neither <ctime>
  // nor ICU's time zone classes, because that may cause I/O operations slowing
  // down the JS engine initialization, which we're currently in the middle of.
  timeZoneStatus_ = TimeZoneStatus::NeedsUpdate;
}

js::DateTimeInfo::~DateTimeInfo() = default;

int64_t js::DateTimeInfo::toClampedSeconds(int64_t milliseconds) {
  int64_t seconds = milliseconds / msPerSecond;
  if (seconds > MaxTimeT) {
    seconds = MaxTimeT;
  } else if (seconds < MinTimeT) {
    /* Go ahead a day to make localtime work (does not work with 0). */
    seconds = SecondsPerDay;
  }
  return seconds;
}

int32_t js::DateTimeInfo::computeDSTOffsetMilliseconds(int64_t utcSeconds) {
  MOZ_ASSERT(utcSeconds >= MinTimeT);
  MOZ_ASSERT(utcSeconds <= MaxTimeT);

  struct tm tm;
  if (!ComputeLocalTime(static_cast<time_t>(utcSeconds), &tm)) {
    return 0;
  }

  // NB: The offset isn't computed correctly when the standard local offset
  //     at |utcSeconds| is different from |utcToLocalStandardOffsetSeconds|.
  int32_t dayoff =
      int32_t((utcSeconds + utcToLocalStandardOffsetSeconds_) % SecondsPerDay);
  int32_t tmoff = tm.tm_sec + (tm.tm_min * SecondsPerMinute) +
                  (tm.tm_hour * SecondsPerHour);

  int32_t diff = tmoff - dayoff;

  if (diff < 0) {
    diff += SecondsPerDay;
  } else if (uint32_t(diff) >= SecondsPerDay) {
    diff -= SecondsPerDay;
  }

  return diff * msPerSecond;
}

int32_t js::DateTimeInfo::internalGetDSTOffsetMilliseconds(
    int64_t utcMilliseconds) {
  int64_t utcSeconds = toClampedSeconds(utcMilliseconds);
  return getOrComputeValue(dstRange_, utcSeconds,
                           &DateTimeInfo::computeDSTOffsetMilliseconds);
}

int32_t js::DateTimeInfo::getOrComputeValue(RangeCache& range, int64_t seconds,
                                            ComputeFn compute) {
  range.sanityCheck();

  auto checkSanity =
      mozilla::MakeScopeExit([&range]() { range.sanityCheck(); });

  // NB: Be aware of the initial range values when making changes to this
  //     code: the first call to this method, with those initial range
  //     values, must result in a cache miss.
  MOZ_ASSERT(seconds != INT64_MIN);

  if (range.startSeconds <= seconds && seconds <= range.endSeconds) {
    return range.offsetMilliseconds;
  }

  if (range.oldStartSeconds <= seconds && seconds <= range.oldEndSeconds) {
    return range.oldOffsetMilliseconds;
  }

  range.oldOffsetMilliseconds = range.offsetMilliseconds;
  range.oldStartSeconds = range.startSeconds;
  range.oldEndSeconds = range.endSeconds;

  if (range.startSeconds <= seconds) {
    int64_t newEndSeconds =
        std::min({range.endSeconds + RangeExpansionAmount, MaxTimeT});
    if (newEndSeconds >= seconds) {
      int32_t endOffsetMilliseconds = (this->*compute)(newEndSeconds);
      if (endOffsetMilliseconds == range.offsetMilliseconds) {
        range.endSeconds = newEndSeconds;
        return range.offsetMilliseconds;
      }

      range.offsetMilliseconds = (this->*compute)(seconds);
      if (range.offsetMilliseconds == endOffsetMilliseconds) {
        range.startSeconds = seconds;
        range.endSeconds = newEndSeconds;
      } else {
        range.endSeconds = seconds;
      }
      return range.offsetMilliseconds;
    }

    range.offsetMilliseconds = (this->*compute)(seconds);
    range.startSeconds = range.endSeconds = seconds;
    return range.offsetMilliseconds;
  }

  int64_t newStartSeconds =
      std::max<int64_t>({range.startSeconds - RangeExpansionAmount, MinTimeT});
  if (newStartSeconds <= seconds) {
    int32_t startOffsetMilliseconds = (this->*compute)(newStartSeconds);
    if (startOffsetMilliseconds == range.offsetMilliseconds) {
      range.startSeconds = newStartSeconds;
      return range.offsetMilliseconds;
    }

    range.offsetMilliseconds = (this->*compute)(seconds);
    if (range.offsetMilliseconds == startOffsetMilliseconds) {
      range.startSeconds = newStartSeconds;
      range.endSeconds = seconds;
    } else {
      range.startSeconds = seconds;
    }
    return range.offsetMilliseconds;
  }

  range.startSeconds = range.endSeconds = seconds;
  range.offsetMilliseconds = (this->*compute)(seconds);
  return range.offsetMilliseconds;
}

void js::DateTimeInfo::RangeCache::reset() {
  // The initial range values are carefully chosen to result in a cache miss
  // on first use given the range of possible values. Be careful to keep
  // these values and the caching algorithm in sync!
  offsetMilliseconds = 0;
  startSeconds = endSeconds = INT64_MIN;
  oldOffsetMilliseconds = 0;
  oldStartSeconds = oldEndSeconds = INT64_MIN;

  sanityCheck();
}

void js::DateTimeInfo::RangeCache::sanityCheck() {
  auto assertRange = [](int64_t start, int64_t end) {
    MOZ_ASSERT(start <= end);
    MOZ_ASSERT_IF(start == INT64_MIN, end == INT64_MIN);
    MOZ_ASSERT_IF(end == INT64_MIN, start == INT64_MIN);
    MOZ_ASSERT_IF(start != INT64_MIN, start >= MinTimeT && end >= MinTimeT);
    MOZ_ASSERT_IF(start != INT64_MIN, start <= MaxTimeT && end <= MaxTimeT);
  };

  assertRange(startSeconds, endSeconds);
  assertRange(oldStartSeconds, oldEndSeconds);
}

/* static */ js::ExclusiveData<js::DateTimeInfo>* js::DateTimeInfo::instance;

bool js::InitDateTimeState() {
  MOZ_ASSERT(!DateTimeInfo::instance, "we should be initializing only once");

  DateTimeInfo::instance =
      js_new<ExclusiveData<DateTimeInfo>>(mutexid::DateTimeInfoMutex);
  return !!DateTimeInfo::instance;
}

/* static */
void js::FinishDateTimeState() {
  js_delete(DateTimeInfo::instance);
  DateTimeInfo::instance = nullptr;
}

void js::ResetTimeZoneInternal(ResetTimeZoneMode mode) {
  js::DateTimeInfo::resetTimeZone(mode);
}

JS_PUBLIC_API void JS::ResetTimeZone() {
  js::ResetTimeZoneInternal(js::ResetTimeZoneMode::ResetEvenIfOffsetUnchanged);
}

void js::ResyncICUDefaultTimeZone() {
  js::DateTimeInfo::resyncICUDefaultTimeZone();
}
