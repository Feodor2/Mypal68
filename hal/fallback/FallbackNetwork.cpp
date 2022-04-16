/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Hal.h"
#include "mozilla/dom/network/Constants.h"

namespace mozilla {
namespace hal_impl {

void EnableNetworkNotifications() {}

void DisableNetworkNotifications() {}

void GetCurrentNetworkInformation(hal::NetworkInformation* aNetworkInfo) {
  aNetworkInfo->type() = dom::network::kDefaultType;
  aNetworkInfo->isWifi() = dom::network::kDefaultIsWifi;
  aNetworkInfo->dhcpGateway() = dom::network::kDefaultDHCPGateway;
}

}  // namespace hal_impl
}  // namespace mozilla
