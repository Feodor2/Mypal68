/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SessionStorageManager.h"

#include "mozilla/dom/ContentChild.h"
#include "SessionStorage.h"
#include "SessionStorageCache.h"
#include "SessionStorageObserver.h"
#include "StorageUtils.h"

namespace mozilla {
namespace dom {

using namespace StorageUtils;

NS_IMPL_ISUPPORTS(SessionStorageManager, nsIDOMStorageManager,
                  nsIDOMSessionStorageManager)

SessionStorageManager::SessionStorageManager() {
  StorageObserver* observer = StorageObserver::Self();
  NS_ASSERTION(
      observer,
      "No StorageObserver, cannot observe private data delete notifications!");

  if (observer) {
    observer->AddSink(this);
  }

  if (!XRE_IsParentProcess() && NextGenLocalStorageEnabled()) {
    // When LSNG is enabled the thread IPC bridge doesn't exist, so we have to
    // create own protocol to distribute chrome observer notifications to
    // content processes.
    mObserver = SessionStorageObserver::Get();

    if (!mObserver) {
      ContentChild* contentActor = ContentChild::GetSingleton();
      MOZ_ASSERT(contentActor);

      RefPtr<SessionStorageObserver> observer = new SessionStorageObserver();

      SessionStorageObserverChild* actor =
          new SessionStorageObserverChild(observer);

      MOZ_ALWAYS_TRUE(
          contentActor->SendPSessionStorageObserverConstructor(actor));

      observer->SetActor(actor);

      mObserver = std::move(observer);
    }
  }
}

SessionStorageManager::~SessionStorageManager() {
  StorageObserver* observer = StorageObserver::Self();
  if (observer) {
    observer->RemoveSink(this);
  }
}

NS_IMETHODIMP
SessionStorageManager::PrecacheStorage(nsIPrincipal* aPrincipal,
                                       nsIPrincipal* aStoragePrincipal,
                                       Storage** aRetval) {
  // Nothing to preload.
  return NS_OK;
}

NS_IMETHODIMP
SessionStorageManager::GetSessionStorageCache(
    nsIPrincipal* aPrincipal, nsIPrincipal* aStoragePrincipal,
    RefPtr<SessionStorageCache>* aRetVal) {
  return GetSessionStorageCacheHelper(aStoragePrincipal, true, nullptr,
                                      aRetVal);
}

nsresult SessionStorageManager::GetSessionStorageCacheHelper(
    nsIPrincipal* aPrincipal, bool aMakeIfNeeded,
    SessionStorageCache* aCloneFrom, RefPtr<SessionStorageCache>* aRetVal) {
  *aRetVal = nullptr;

  nsAutoCString originKey;
  nsAutoCString originAttributes;
  nsresult rv = aPrincipal->GetStorageOriginKey(originKey);
  aPrincipal->OriginAttributesRef().CreateSuffix(originAttributes);
  if (NS_FAILED(rv)) {
    return NS_ERROR_NOT_AVAILABLE;
  }

  OriginKeyHashTable* table;
  if (!mOATable.Get(originAttributes, &table)) {
    if (aMakeIfNeeded) {
      table =
          mOATable
              .InsertOrUpdate(originAttributes, MakeUnique<OriginKeyHashTable>())
              .get();
    } else {
      return NS_OK;
    }
  }

  RefPtr<SessionStorageCache> cache;
  if (!table->Get(originKey, getter_AddRefs(cache))) {
    if (aMakeIfNeeded) {
      if (aCloneFrom) {
        cache = aCloneFrom->Clone();
      } else {
        cache = new SessionStorageCache();
      }
      table->LookupOrInsert(originKey, cache);
    } else {
      return NS_OK;
    }
  }

  *aRetVal = std::move(cache);

  return NS_OK;
}

NS_IMETHODIMP
SessionStorageManager::CreateStorage(mozIDOMWindow* aWindow,
                                     nsIPrincipal* aPrincipal,
                                     nsIPrincipal* aStoragePrincipal,
                                     const nsAString& aDocumentURI,
                                     bool aPrivate, Storage** aRetval) {
  RefPtr<SessionStorageCache> cache;
  nsresult rv = GetSessionStorageCache(aPrincipal, aStoragePrincipal, &cache);
  if (NS_FAILED(rv)) {
    return rv;
  }

  nsCOMPtr<nsPIDOMWindowInner> inner = nsPIDOMWindowInner::From(aWindow);

  RefPtr<SessionStorage> storage =
      new SessionStorage(inner, aPrincipal, aStoragePrincipal, cache, this,
                         aDocumentURI, aPrivate);

  storage.forget(aRetval);
  return NS_OK;
}

NS_IMETHODIMP
SessionStorageManager::GetStorage(mozIDOMWindow* aWindow,
                                  nsIPrincipal* aPrincipal,
                                  nsIPrincipal* aStoragePrincipal,
                                  bool aPrivate, Storage** aRetval) {
  *aRetval = nullptr;

  RefPtr<SessionStorageCache> cache;
  nsresult rv =
      GetSessionStorageCacheHelper(aStoragePrincipal, false, nullptr, &cache);
  if (NS_FAILED(rv) || !cache) {
    return rv;
  }

  nsCOMPtr<nsPIDOMWindowInner> inner = nsPIDOMWindowInner::From(aWindow);

  RefPtr<SessionStorage> storage = new SessionStorage(
      inner, aPrincipal, aStoragePrincipal, cache, this, u""_ns, aPrivate);

  storage.forget(aRetval);
  return NS_OK;
}

NS_IMETHODIMP
SessionStorageManager::CloneStorage(Storage* aStorage) {
  if (NS_WARN_IF(!aStorage)) {
    return NS_ERROR_UNEXPECTED;
  }

  if (aStorage->Type() != Storage::eSessionStorage) {
    return NS_ERROR_UNEXPECTED;
  }

  RefPtr<SessionStorageCache> cache;
  return GetSessionStorageCacheHelper(
      aStorage->StoragePrincipal(), true,
      static_cast<SessionStorage*>(aStorage)->Cache(), &cache);
}

NS_IMETHODIMP
SessionStorageManager::CheckStorage(nsIPrincipal* aPrincipal, Storage* aStorage,
                                    bool* aRetval) {
  if (NS_WARN_IF(!aStorage)) {
    return NS_ERROR_UNEXPECTED;
  }

  if (!aPrincipal) {
    return NS_ERROR_NOT_AVAILABLE;
  }

  *aRetval = false;

  RefPtr<SessionStorageCache> cache;
  nsresult rv =
      GetSessionStorageCacheHelper(aPrincipal, false, nullptr, &cache);
  if (NS_FAILED(rv) || !cache) {
    return rv;
  }

  if (aStorage->Type() != Storage::eSessionStorage) {
    return NS_OK;
  }

  RefPtr<SessionStorage> sessionStorage =
      static_cast<SessionStorage*>(aStorage);
  if (sessionStorage->Cache() != cache) {
    return NS_OK;
  }

  if (!StorageUtils::PrincipalsEqual(aStorage->StoragePrincipal(),
                                     aPrincipal)) {
    return NS_OK;
  }

  *aRetval = true;
  return NS_OK;
}

void SessionStorageManager::ClearStorages(
    ClearStorageType aType, const OriginAttributesPattern& aPattern,
    const nsACString& aOriginScope) {
  for (const auto& oaEntry : mOATable) {
    OriginAttributes oa;
    DebugOnly<bool> ok = oa.PopulateFromSuffix(oaEntry.GetKey());
    MOZ_ASSERT(ok);
    if (!aPattern.Matches(oa)) {
      // This table doesn't match the given origin attributes pattern
      continue;
    }

    OriginKeyHashTable* table = oaEntry.GetWeak();
    for (const auto& originKeyEntry : *table) {
      if (aOriginScope.IsEmpty() ||
          StringBeginsWith(originKeyEntry.GetKey(), aOriginScope)) {
        const auto cache = originKeyEntry.GetData();
        if (aType == eAll) {
          cache->Clear(SessionStorageCache::eDefaultSetType, false);
          cache->Clear(SessionStorageCache::eSessionSetType, false);
        } else {
          MOZ_ASSERT(aType == eSessionOnly);
          cache->Clear(SessionStorageCache::eSessionSetType, false);
        }
      }
    }
  }
}

nsresult SessionStorageManager::Observe(
    const char* aTopic, const nsAString& aOriginAttributesPattern,
    const nsACString& aOriginScope) {
  OriginAttributesPattern pattern;
  if (!pattern.Init(aOriginAttributesPattern)) {
    NS_ERROR("Cannot parse origin attributes pattern");
    return NS_ERROR_FAILURE;
  }

  // Clear everything, caches + database
  if (!strcmp(aTopic, "cookie-cleared")) {
    ClearStorages(eAll, pattern, ""_ns);
    return NS_OK;
  }

  // Clear from caches everything that has been stored
  // while in session-only mode
  if (!strcmp(aTopic, "session-only-cleared")) {
    ClearStorages(eSessionOnly, pattern, aOriginScope);
    return NS_OK;
  }

  // Clear everything (including so and pb data) from caches and database
  // for the given domain and subdomains.
  if (!strcmp(aTopic, "browser:purge-sessionStorage")) {
    ClearStorages(eAll, pattern, aOriginScope);
    return NS_OK;
  }

  if (!strcmp(aTopic, "profile-change")) {
    // For case caches are still referenced - clear them completely
    ClearStorages(eAll, pattern, ""_ns);
    mOATable.Clear();
    return NS_OK;
  }

  return NS_OK;
}

}  // namespace dom
}  // namespace mozilla
