/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ParentChannelWrapper.h"
#include "nsIRedirectChannelRegistrar.h"

namespace mozilla {
namespace net {

NS_IMPL_ISUPPORTS(ParentChannelWrapper, nsIParentChannel, nsIStreamListener,
                  nsIRequestObserver);

void ParentChannelWrapper::Register(uint64_t aRegistrarId) {
  nsCOMPtr<nsIRedirectChannelRegistrar> registrar =
      RedirectChannelRegistrar::GetOrCreate();
  nsCOMPtr<nsIChannel> dummy;
  MOZ_ALWAYS_SUCCEEDS(
      NS_LinkRedirectChannels(aRegistrarId, this, getter_AddRefs(dummy)));

#ifdef DEBUG
  // The channel registered with the RedirectChannelRegistrar will be the inner
  // channel when dealing with view-source loads.
  if (nsCOMPtr<nsIViewSourceChannel> viewSource = do_QueryInterface(mChannel)) {
    MOZ_ASSERT(dummy == viewSource->GetInnerChannel());
  } else {
    MOZ_ASSERT(dummy == mChannel);
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////
// nsIParentChannel
////////////////////////////////////////////////////////////////////////////////

NS_IMETHODIMP
ParentChannelWrapper::SetParentListener(
    mozilla::net::ParentChannelListener* listener) {
  return NS_OK;
}

NS_IMETHODIMP
ParentChannelWrapper::NotifyFlashPluginStateChanged(
    nsIHttpChannel::FlashPluginState aState) {
  // For now, only HttpChannel use this attribute.
  RefPtr<HttpBaseChannel> httpChannel = do_QueryObject(mChannel);
  if (httpChannel) {
    httpChannel->SetFlashPluginState(aState);
  }
  return NS_OK;
}

NS_IMETHODIMP
ParentChannelWrapper::Delete() { return NS_OK; }

NS_IMETHODIMP
ParentChannelWrapper::GetRemoteType(nsACString& aRemoteType) {
  aRemoteType = VoidCString();
  return NS_OK;
}

}  // namespace net
}  // namespace mozilla

#undef LOG
