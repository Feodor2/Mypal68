/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Telemetry.h"
#include "TelemetryOrigin.h"

#include "nsDataHashtable.h"
#include "nsIObserverService.h"
#include "nsPrintfCString.h"
#include "nsTArray.h"
#include "TelemetryCommon.h"
#include "TelemetryOriginEnums.h"

#include "js/Array.h"  // JS::NewArrayObject
#include "mozilla/Atomics.h"
#include "mozilla/Base64.h"
#include "mozilla/Preferences.h"
#include "mozilla/Pair.h"
#include "mozilla/Services.h"
#include "mozilla/StaticMutex.h"
#include "mozilla/Tuple.h"
#include "mozilla/UniquePtr.h"

#include <cmath>
#include <type_traits>

using mozilla::Get;
using mozilla::MakePair;
using mozilla::MakeTuple;
using mozilla::MakeUnique;
using mozilla::MallocSizeOf;
using mozilla::Pair;
using mozilla::StaticMutex;
using mozilla::StaticMutexAutoLock;
using mozilla::Tuple;
using mozilla::UniquePtr;
using mozilla::Telemetry::OriginMetricID;
using mozilla::Telemetry::Common::ToJSString;

/***********************************************************************
 *
 * Firefox Origin Telemetry
 * Docs:
 * https://firefox-source-docs.mozilla.org/toolkit/components/telemetry/telemetry/collection/origin.html
 *
 * Origin Telemetry stores pairs of information (metric, origin) which boils
 * down to "$metric happened on $origin".
 *
 * Prio can only encode up-to-2046-length bit vectors. The process of
 * transforming these pairs of information into bit vectors is called "App
 * Encoding". The bit vectors are then "Prio Encoded" into binary goop. The
 * binary goop is then "Base64 Encoded" into strings.
 *
 */

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// PRIVATE TYPES

namespace {

class OriginMetricIDHashKey : public PLDHashEntryHdr {
 public:
  typedef const OriginMetricID& KeyType;
  typedef const OriginMetricID* KeyTypePointer;

  explicit OriginMetricIDHashKey(KeyTypePointer aKey) : mValue(*aKey) {}
  OriginMetricIDHashKey(OriginMetricIDHashKey&& aOther)
      : PLDHashEntryHdr(std::move(aOther)), mValue(std::move(aOther.mValue)) {}
  ~OriginMetricIDHashKey() = default;

  KeyType GetKey() const { return mValue; }
  bool KeyEquals(KeyTypePointer aKey) const { return *aKey == mValue; }

  static KeyTypePointer KeyToPointer(KeyType aKey) { return &aKey; }
  static PLDHashNumber HashKey(KeyTypePointer aKey) {
    return static_cast<std::underlying_type<OriginMetricID>::type>(*aKey);
  }
  enum { ALLOW_MEMMOVE = true };

 private:
  const OriginMetricID mValue;
};

}  // anonymous namespace

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// PRIVATE STATE, SHARED BY ALL THREADS
//
// Access for all of this state (except gInitDone) must be guarded by
// gTelemetryOriginMutex.

namespace {

// This is a StaticMutex rather than a plain Mutex (1) so that
// it gets initialised in a thread-safe manner the first time
// it is used, and (2) because it is never de-initialised, and
// a normal Mutex would show up as a leak in BloatView.  StaticMutex
// also has the "OffTheBooks" property, so it won't show as a leak
// in BloatView.
// Another reason to use a StaticMutex instead of a plain Mutex is
// that, due to the nature of Telemetry, we cannot rely on having a
// mutex initialized in InitializeGlobalState. Unfortunately, we
// cannot make sure that no other function is called before this point.
static StaticMutex gTelemetryOriginMutex;

typedef nsTArray<Tuple<const char*, const char*>> OriginHashesList;
UniquePtr<OriginHashesList> gOriginHashesList;

typedef nsDataHashtable<nsCStringHashKey, size_t> OriginToIndexMap;
UniquePtr<OriginToIndexMap> gOriginToIndexMap;

typedef nsDataHashtable<nsCStringHashKey, size_t> HashToIndexMap;
UniquePtr<HashToIndexMap> gHashToIndexMap;

typedef nsDataHashtable<nsCStringHashKey, uint32_t> OriginBag;
typedef nsDataHashtable<OriginMetricIDHashKey, OriginBag> IdToOriginBag;

UniquePtr<IdToOriginBag> gMetricToOriginBag;

mozilla::Atomic<bool, mozilla::Relaxed> gInitDone(false);

// Useful for app-encoded data
typedef nsTArray<Pair<OriginMetricID, nsTArray<nsTArray<bool>>>>
    IdBoolsPairArray;

// Prio has a maximum supported number of bools it can encode at a time.
// This means a single metric may result in several encoded payloads if the
// number of origins exceeds the number of bools.
// Each encoded payload corresponds to an element in the `prioData` array in the
// "prio" ping.
// This number is the number of encoded payloads needed per metric, and is
// equivalent to "how many bitvectors do we need to encode this whole list of
// origins?"
static uint32_t gPrioDatasPerMetric;

// The number of "meta-origins": in-band metadata about origin telemetry.
// Currently 1: the "unknown origin recorded" meta-origin.
static uint32_t kNumMetaOrigins = 1;

NS_NAMED_LITERAL_CSTRING(kUnknownOrigin, "__UNKNOWN__");

}  // namespace

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// PRIVATE: thread-safe helpers

namespace {

const char* GetNameForMetricID(OriginMetricID aId) {
  MOZ_ASSERT(aId < OriginMetricID::Count);
  return mozilla::Telemetry::MetricIDToString[static_cast<uint32_t>(aId)];
}

// Calculates the number of `prioData` elements we'd need if we were asked for
// an encoded snapshot right now.
uint32_t PrioDataCount(const StaticMutexAutoLock& lock) {
  uint32_t count = 0;
  auto iter = gMetricToOriginBag->ConstIter();
  for (; !iter.Done(); iter.Next()) {
    auto originIt = iter.Data().ConstIter();
    uint32_t maxOriginCount = 0;
    for (; !originIt.Done(); originIt.Next()) {
      maxOriginCount = std::max(maxOriginCount, originIt.Data());
    }
    count += gPrioDatasPerMetric * maxOriginCount;
  }
  return count;
}

// Takes the storage and turns it into bool arrays for Prio to encode, turning
// { metric1: [origin1, origin2, ...], ...}
// into
// [(metric1, [[shard1], [shard2], ...]), ...]
// Note: if an origin is present multiple times for a given metric, we must
// generate multiple (id, boolvectors) pairs so that they are all reported.
// Meaning
// { metric1: [origin1, origin2, origin2] }
// must turn into (with a pretend gNumBooleans of 1)
// [(metric1, [[1], [1]]), (metric1, [[0], [1]])]
nsresult AppEncodeTo(const StaticMutexAutoLock& lock,
                     IdBoolsPairArray& aResult) {
  return NS_ERROR_FAILURE;
}

}  // anonymous namespace

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// EXTERNALLY VISIBLE FUNCTIONS in namespace TelemetryOrigin::

void TelemetryOrigin::InitializeGlobalState() {
  gInitDone = false;
}

void TelemetryOrigin::DeInitializeGlobalState() {
  if (!XRE_IsParentProcess()) {
    return;
  }

  StaticMutexAutoLock locker(gTelemetryOriginMutex);
  MOZ_ASSERT(gInitDone);

  if (!gInitDone) {
    return;
  }

  gOriginHashesList = nullptr;

  gOriginToIndexMap = nullptr;

  gHashToIndexMap = nullptr;

  gMetricToOriginBag = nullptr;

  gInitDone = false;
}

nsresult TelemetryOrigin::RecordOrigin(OriginMetricID aId,
                                       const nsACString& aOrigin) {
  if (!XRE_IsParentProcess()) {
    return NS_ERROR_FAILURE;
  }

  uint32_t prioDataCount;
  {
    StaticMutexAutoLock locker(gTelemetryOriginMutex);

    // Common Telemetry error-handling practices for recording functions:
    // only illegal calls return errors whereas merely incorrect ones are mutely
    // ignored.
    if (!gInitDone) {
      return NS_OK;
    }

    size_t index;
    nsCString origin(aOrigin);
    if (gHashToIndexMap->Get(aOrigin, &index)) {
      MOZ_ASSERT(aOrigin.Equals(Get<1>((*gOriginHashesList)[index])));
      origin = Get<0>((*gOriginHashesList)[index]);
    }

    if (!gOriginToIndexMap->Contains(origin)) {
      // Only record one unknown origin per metric per snapshot.
      // (otherwise we may get swamped and blow our data budget.)
      if (gMetricToOriginBag->Contains(aId) &&
          gMetricToOriginBag->GetOrInsert(aId).Contains(kUnknownOrigin)) {
        return NS_OK;
      }
      origin = kUnknownOrigin;
    }

    auto& originBag = gMetricToOriginBag->GetOrInsert(aId);
    originBag.GetOrInsert(origin)++;

    prioDataCount = PrioDataCount(locker);
  }

  return NS_OK;
}

nsresult TelemetryOrigin::GetOriginSnapshot(bool aClear, JSContext* aCx,
                                            JS::MutableHandleValue aResult) {
  if (NS_WARN_IF(!XRE_IsParentProcess())) {
    return NS_ERROR_FAILURE;
  }

  if (!gInitDone) {
    return NS_OK;
  }

  // Step 1: Grab the lock, copy into stack-local storage, optionally clear.
  IdToOriginBag copy;
  {
    StaticMutexAutoLock locker(gTelemetryOriginMutex);

    if (aClear) {
      // I'd really prefer to clear after we're sure the snapshot didn't go
      // awry, but we can't hold a lock preventing recording while using JS
      // APIs. And replaying any interleaving recording sounds like too much
      // squeeze for not enough juice.

      gMetricToOriginBag->SwapElements(copy);
    } else {
      auto iter = gMetricToOriginBag->ConstIter();
      for (; !iter.Done(); iter.Next()) {
        OriginBag& bag = copy.GetOrInsert(iter.Key());
        auto originIt = iter.Data().ConstIter();
        for (; !originIt.Done(); originIt.Next()) {
          bag.Put(originIt.Key(), originIt.Data());
        }
      }
    }
  }

  // Step 2: Without the lock, generate JS datastructure for snapshotting
  JS::Rooted<JSObject*> rootObj(aCx, JS_NewPlainObject(aCx));
  if (NS_WARN_IF(!rootObj)) {
    return NS_ERROR_FAILURE;
  }
  aResult.setObject(*rootObj);
  for (auto iter = copy.ConstIter(); !iter.Done(); iter.Next()) {
    JS::RootedObject originsObj(aCx, JS_NewPlainObject(aCx));
    if (NS_WARN_IF(!originsObj)) {
      return NS_ERROR_FAILURE;
    }
    if (!JS_DefineProperty(aCx, rootObj, GetNameForMetricID(iter.Key()),
                           originsObj, JSPROP_ENUMERATE)) {
      NS_WARNING("Failed to define property in origin snapshot.");
      return NS_ERROR_FAILURE;
    }

    auto originIt = iter.Data().ConstIter();
    for (; !originIt.Done(); originIt.Next()) {
      if (!JS_DefineProperty(aCx, originsObj,
                             nsPromiseFlatCString(originIt.Key()).get(),
                             originIt.Data(), JSPROP_ENUMERATE)) {
        NS_WARNING("Failed to define origin and count in snapshot.");
        return NS_ERROR_FAILURE;
      }
    }
  }

  return NS_OK;
}

nsresult TelemetryOrigin::GetEncodedOriginSnapshot(
    bool aClear, JSContext* aCx, JS::MutableHandleValue aSnapshot) {
    return NS_ERROR_FAILURE;
}

/**
 * Resets all the stored events. This is intended to be only used in tests.
 */
void TelemetryOrigin::ClearOrigins() {
  StaticMutexAutoLock lock(gTelemetryOriginMutex);

  if (!gInitDone) {
    return;
  }

  gMetricToOriginBag->Clear();
}

size_t TelemetryOrigin::SizeOfIncludingThis(MallocSizeOf aMallocSizeOf) {
  StaticMutexAutoLock locker(gTelemetryOriginMutex);
  size_t n = 0;

  if (!gInitDone) {
    return 0;
  }

  n += gMetricToOriginBag->ShallowSizeOfIncludingThis(aMallocSizeOf);
  auto iter = gMetricToOriginBag->ConstIter();
  for (; !iter.Done(); iter.Next()) {
    // The string hashkey and count should both be contained by the hashtable.
    n += iter.Data().ShallowSizeOfIncludingThis(aMallocSizeOf);
  }

  // The string hashkey and ID should both be contained within the hashtable.
  n += gOriginToIndexMap->ShallowSizeOfIncludingThis(aMallocSizeOf);

  return n;
}

size_t TelemetryOrigin::SizeOfPrioDatasPerMetric() {
  return gPrioDatasPerMetric;
}
