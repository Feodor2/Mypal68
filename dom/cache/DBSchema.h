/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_cache_DBSchema_h
#define mozilla_dom_cache_DBSchema_h

#include "mozilla/Attributes.h"
#include "mozilla/dom/cache/Types.h"
#include "nsError.h"
#include "nsString.h"
#include "nsTArrayForwardDeclare.h"

class mozIStorageConnection;
struct nsID;

namespace mozilla {
namespace dom {
namespace cache {

class CacheQueryParams;
class CacheRequest;
class CacheResponse;
struct SavedRequest;
struct SavedResponse;

namespace db {

// Note, this cannot be executed within a transaction.
nsresult CreateOrMigrateSchema(mozIStorageConnection* aConn);

// Note, this cannot be executed within a transaction.
nsresult InitializeConnection(mozIStorageConnection* aConn);

nsresult CreateCacheId(mozIStorageConnection* aConn, CacheId* aCacheIdOut);

nsresult DeleteCacheId(mozIStorageConnection* aConn, CacheId aCacheId,
                       nsTArray<nsID>& aDeletedBodyIdListOut,
                       int64_t* aDeletedPaddingSizeOut);

// TODO: Consider removing unused IsCacheOrphaned after writing cleanup code.
// (bug 1110446)
nsresult IsCacheOrphaned(mozIStorageConnection* aConn, CacheId aCacheId,
                         bool* aOrphanedOut);

nsresult FindOrphanedCacheIds(mozIStorageConnection* aConn,
                              nsTArray<CacheId>& aOrphanedListOut);

nsresult FindOverallPaddingSize(mozIStorageConnection* aConn,
                                int64_t* aOverallPaddingSizeOut);

nsresult GetKnownBodyIds(mozIStorageConnection* aConn,
                         nsTArray<nsID>& aBodyIdListOut);

nsresult CacheMatch(mozIStorageConnection* aConn, CacheId aCacheId,
                    const CacheRequest& aRequest,
                    const CacheQueryParams& aParams, bool* aFoundResponseOut,
                    SavedResponse* aSavedResponseOut);

nsresult CacheMatchAll(mozIStorageConnection* aConn, CacheId aCacheId,
                       const Maybe<CacheRequest>& aMaybeRequest,
                       const CacheQueryParams& aParams,
                       nsTArray<SavedResponse>& aSavedResponsesOut);

nsresult CachePut(mozIStorageConnection* aConn, CacheId aCacheId,
                  const CacheRequest& aRequest, const nsID* aRequestBodyId,
                  const CacheResponse& aResponse, const nsID* aResponseBodyId,
                  nsTArray<nsID>& aDeletedBodyIdListOut,
                  int64_t* aDeletedPaddingSizeOut);

nsresult CacheDelete(mozIStorageConnection* aConn, CacheId aCacheId,
                     const CacheRequest& aRequest,
                     const CacheQueryParams& aParams,
                     nsTArray<nsID>& aDeletedBodyIdListOut,
                     int64_t* aDeletedPaddingSizeOut, bool* aSuccessOut);

nsresult CacheKeys(mozIStorageConnection* aConn, CacheId aCacheId,
                   const Maybe<CacheRequest>& aMaybeRequest,
                   const CacheQueryParams& aParams,
                   nsTArray<SavedRequest>& aSavedRequestsOut);

nsresult StorageMatch(mozIStorageConnection* aConn, Namespace aNamespace,
                      const CacheRequest& aRequest,
                      const CacheQueryParams& aParams, bool* aFoundResponseOut,
                      SavedResponse* aSavedResponseOut);

nsresult StorageGetCacheId(mozIStorageConnection* aConn, Namespace aNamespace,
                           const nsAString& aKey, bool* aFoundCacheOut,
                           CacheId* aCacheIdOut);

nsresult StoragePutCache(mozIStorageConnection* aConn, Namespace aNamespace,
                         const nsAString& aKey, CacheId aCacheId);

nsresult StorageForgetCache(mozIStorageConnection* aConn, Namespace aNamespace,
                            const nsAString& aKey);

nsresult StorageGetKeys(mozIStorageConnection* aConn, Namespace aNamespace,
                        nsTArray<nsString>& aKeysOut);

// Note, this works best when its NOT executed within a transaction.
nsresult IncrementalVacuum(mozIStorageConnection* aConn);

// We will wipe out databases with a schema versions less than this.  Newer
// versions will be migrated on open to the latest schema version.
extern const int32_t kFirstShippedSchemaVersion;

}  // namespace db
}  // namespace cache
}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_cache_DBSchema_h
