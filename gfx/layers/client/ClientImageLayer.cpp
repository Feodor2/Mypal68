/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ClientLayerManager.h"  // for ClientLayerManager, etc
#include "ImageContainer.h"      // for AutoLockImage, etc
#include "ImageLayers.h"         // for ImageLayer
#include "mozilla/Attributes.h"  // for override
#include "mozilla/RefPtr.h"      // for RefPtr
#include "mozilla/layers/CompositorTypes.h"
#include "mozilla/layers/ImageClient.h"     // for ImageClient, etc
#include "mozilla/layers/LayersMessages.h"  // for ImageLayerAttributes, etc
#include "mozilla/mozalloc.h"               // for operator delete, etc
#include "nsCOMPtr.h"                       // for already_AddRefed
#include "nsDebug.h"                        // for NS_ASSERTION
#include "nsISupportsImpl.h"                // for Layer::AddRef, etc
#include "nsRegion.h"                       // for nsIntRegion

namespace mozilla {
namespace layers {

using namespace mozilla::gfx;

class ClientImageLayer : public ImageLayer, public ClientLayer {
 public:
  explicit ClientImageLayer(ClientLayerManager* aLayerManager)
      : ImageLayer(aLayerManager, static_cast<ClientLayer*>(this)),
        mImageClientTypeContainer(CompositableType::UNKNOWN) {
    MOZ_COUNT_CTOR(ClientImageLayer);
  }

 protected:
  virtual ~ClientImageLayer() {
    DestroyBackBuffer();
    MOZ_COUNT_DTOR(ClientImageLayer);
  }

  void SetContainer(ImageContainer* aContainer) override {
    ImageLayer::SetContainer(aContainer);
    mImageClientTypeContainer = CompositableType::UNKNOWN;
  }

  void SetVisibleRegion(const LayerIntRegion& aRegion) override {
    NS_ASSERTION(ClientManager()->InConstruction(),
                 "Can only set properties in construction phase");
    ImageLayer::SetVisibleRegion(aRegion);
  }

  void RenderLayer() override;

  void ClearCachedResources() override { DestroyBackBuffer(); }

  bool SupportsAsyncUpdate() override {
    if (GetImageClientType() == CompositableType::IMAGE_BRIDGE) {
      return true;
    }
    return false;
  }

  void HandleMemoryPressure() override {
    if (mImageClient) {
      mImageClient->HandleMemoryPressure();
    }
  }

  void FillSpecificAttributes(SpecificLayerAttributes& aAttrs) override {
    aAttrs = ImageLayerAttributes(mSamplingFilter, mScaleToSize, mScaleMode);
  }

  Layer* AsLayer() override { return this; }
  ShadowableLayer* AsShadowableLayer() override { return this; }

  void Disconnect() override { DestroyBackBuffer(); }

  void DestroyBackBuffer() {
    if (mImageClient) {
      mImageClient->SetLayer(nullptr);
      mImageClient->OnDetach();
      mImageClient = nullptr;
    }
  }

  CompositableClient* GetCompositableClient() override { return mImageClient; }

 protected:
  ClientLayerManager* ClientManager() {
    return static_cast<ClientLayerManager*>(mManager);
  }

  CompositableType GetImageClientType() {
    if (mImageClientTypeContainer != CompositableType::UNKNOWN) {
      return mImageClientTypeContainer;
    }

    if (mContainer->IsAsync()) {
      mImageClientTypeContainer = CompositableType::IMAGE_BRIDGE;
      return mImageClientTypeContainer;
    }

    AutoLockImage autoLock(mContainer);

    mImageClientTypeContainer = autoLock.HasImage() ? CompositableType::IMAGE
                                                    : CompositableType::UNKNOWN;
    return mImageClientTypeContainer;
  }

  RefPtr<ImageClient> mImageClient;
  CompositableType mImageClientTypeContainer;
};

void ClientImageLayer::RenderLayer() {
  RenderMaskLayers(this);

  if (!mContainer) {
    return;
  }

  if (!mImageClient || !mImageClient->UpdateImage(mContainer, GetContentFlags()
#ifdef MOZ_BUILD_WEBRENDER
                                                                  ,
                                                  Nothing()
#endif
                                                      )) {
    CompositableType type = GetImageClientType();
    if (type == CompositableType::UNKNOWN) {
      return;
    }
    TextureFlags flags = TextureFlags::DEFAULT;
    mImageClient = ImageClient::CreateImageClient(
        type, ClientManager()->AsShadowForwarder(), flags);
    if (!mImageClient) {
      return;
    }
    mImageClient->SetLayer(this);
    if (HasShadow() && !mContainer->IsAsync()) {
      mImageClient->Connect();
      ClientManager()->AsShadowForwarder()->Attach(mImageClient, this);
    }
    if (!mImageClient->UpdateImage(mContainer, GetContentFlags()
#ifdef MOZ_BUILD_WEBRENDER
                                                   ,
                                   Nothing()
#endif
                                       )) {
      return;
    }
  }
  ClientManager()->Hold(this);
}

already_AddRefed<ImageLayer> ClientLayerManager::CreateImageLayer() {
  NS_ASSERTION(InConstruction(), "Only allowed in construction phase");
  RefPtr<ClientImageLayer> layer = new ClientImageLayer(this);
  CREATE_SHADOW(Image);
  return layer.forget();
}

}  // namespace layers
}  // namespace mozilla
