/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GFX_CLIENTREADBACKLAYER_H
#define GFX_CLIENTREADBACKLAYER_H

#include "ClientLayerManager.h"
#include "ReadbackLayer.h"

namespace mozilla {
namespace layers {

class ClientReadbackLayer : public ReadbackLayer, public ClientLayer {
 public:
  explicit ClientReadbackLayer(ClientLayerManager* aManager)
      : ReadbackLayer(aManager, nullptr) {
    mImplData = static_cast<ClientLayer*>(this);
  }

  ShadowableLayer* AsShadowableLayer() override { return this; }
  Layer* AsLayer() override { return this; }
  void RenderLayer() override {}
};

}  // namespace layers
}  // namespace mozilla

#endif /* GFX_CLIENTREADBACKLAYER_H */
