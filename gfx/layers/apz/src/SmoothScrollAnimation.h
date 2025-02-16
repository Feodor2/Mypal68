/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_layers_SmoothScrollAnimation_h_
#define mozilla_layers_SmoothScrollAnimation_h_

#include "GenericScrollAnimation.h"
#include "mozilla/ScrollOrigin.h"

namespace mozilla {
namespace layers {

class AsyncPanZoomController;

class SmoothScrollAnimation : public GenericScrollAnimation {
 public:
  SmoothScrollAnimation(AsyncPanZoomController& aApzc,
                        const nsPoint& aInitialPosition,
                        ScrollOrigin aScrollOrigin);

  SmoothScrollAnimation* AsSmoothScrollAnimation() override;
  ScrollOrigin GetScrollOrigin() const;

 private:
  ScrollOrigin mOrigin;
};

}  // namespace layers
}  // namespace mozilla

#endif  // mozilla_layers_SmoothScrollAnimation_h_
