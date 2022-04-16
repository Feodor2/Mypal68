/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ThreadPoolCOMListener.h"

namespace mozilla {

NS_IMPL_ISUPPORTS(MSCOMInitThreadPoolListener, nsIThreadPoolListener)

NS_IMETHODIMP
MSCOMInitThreadPoolListener::OnThreadCreated() {
  HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hr)) {
    NS_WARNING("Failed to initialize MSCOM on decoder thread.");
  }
  return NS_OK;
}

NS_IMETHODIMP
MSCOMInitThreadPoolListener::OnThreadShuttingDown() {
  CoUninitialize();
  return NS_OK;
}

}  // namespace mozilla
