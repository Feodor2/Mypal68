/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_serviceworkeroppromise_h__
#define mozilla_dom_serviceworkeroppromise_h__

#include <utility>

#include "mozilla/MozPromise.h"
#include "mozilla/dom/SafeRefPtr.h"
#include "mozilla/dom/ServiceWorkerOpArgs.h"

namespace mozilla {
namespace dom {

class InternalResponse;

using SynthesizeResponseArgs =
    std::pair<SafeRefPtr<InternalResponse>, FetchEventRespondWithClosure>;

using FetchEventRespondWithResult =
    Variant<SynthesizeResponseArgs, ResetInterceptionArgs,
            CancelInterceptionArgs>;

using FetchEventRespondWithPromise =
    MozPromise<FetchEventRespondWithResult, nsresult, true>;

using ServiceWorkerOpPromise =
    MozPromise<ServiceWorkerOpResult, nsresult, true>;

using ServiceWorkerFetchEventOpPromise =
    MozPromise<ServiceWorkerFetchEventOpResult, nsresult, true>;

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_serviceworkeroppromise_h__
