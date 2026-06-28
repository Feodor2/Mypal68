/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_net_TRRServiceParent_h
#define mozilla_net_TRRServiceParent_h

#include "mozilla/DataStorage.h"
#include "mozilla/net/PTRRServiceParent.h"
#include "nsIObserver.h"
#include "nsWeakReference.h"

namespace mozilla {
namespace net {

class TRRServiceParent : public nsIObserver,
                         public nsSupportsWeakReference,
                         public PTRRServiceParent {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIOBSERVER

  TRRServiceParent() = default;
  void Init();

 private:
  virtual ~TRRServiceParent() = default;
};

}  // namespace net
}  // namespace mozilla

#endif  // mozilla_net_TRRServiceParent_h
