/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_ChildProcessChannelListener_h
#define mozilla_dom_ChildProcessChannelListener_h

#include <functional>

#include "mozilla/net/NeckoChannelParams.h"
#include "nsTHashMap.h"
#include "nsIChildChannel.h"

namespace mozilla {
namespace dom {

class ChildProcessChannelListener final {
  NS_INLINE_DECL_REFCOUNTING(ChildProcessChannelListener)

  typedef std::function<void(nsIChildChannel*)> Callback;

  void RegisterCallback(uint64_t aIdentifier, Callback&& aCallback);

  void OnChannelReady(nsIChildChannel* aChannel, uint64_t aIdentifier);

  static already_AddRefed<ChildProcessChannelListener> GetSingleton();

 private:
  ChildProcessChannelListener() = default;
  ~ChildProcessChannelListener() = default;

  nsTHashMap<nsUint64HashKey, Callback> mCallbacks;
  nsTHashMap<nsUint64HashKey, nsCOMPtr<nsIChildChannel>> mChannels;
};

}  // namespace dom
}  // namespace mozilla

#endif  // !defined(mozilla_dom_ChildProcessChannelListener_h)
