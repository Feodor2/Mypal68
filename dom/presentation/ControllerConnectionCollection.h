/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_ControllerConnectionCollection_h
#define mozilla_dom_ControllerConnectionCollection_h

#include "mozilla/StaticPtr.h"
#include "mozilla/WeakPtr.h"
#include "nsString.h"
#include "nsTArray.h"

namespace mozilla {
namespace dom {

class PresentationConnection;

class ControllerConnectionCollection final {
 public:
  static ControllerConnectionCollection* GetSingleton();

  void AddConnection(PresentationConnection* aConnection, const uint8_t aRole);

  void RemoveConnection(PresentationConnection* aConnection,
                        const uint8_t aRole);

  already_AddRefed<PresentationConnection> FindConnection(uint64_t aWindowId,
                                                          const nsAString& aId,
                                                          const uint8_t aRole);

 private:
  friend class StaticAutoPtr<ControllerConnectionCollection>;

  ControllerConnectionCollection();
  virtual ~ControllerConnectionCollection();

  static StaticAutoPtr<ControllerConnectionCollection> sSingleton;
  nsTArray<WeakPtr<PresentationConnection>> mConnections;
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_ControllerConnectionCollection_h
