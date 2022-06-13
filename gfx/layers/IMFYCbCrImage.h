/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GFX_IMFYCBCRIMAGE_H
#define GFX_IMFYCBCRIMAGE_H

#include "mozilla/layers/TextureD3D11.h"
#include "mozilla/RefPtr.h"
#include "ImageContainer.h"
#include "mfidl.h"

namespace mozilla {
namespace layers {

class IMFYCbCrImage : public RecyclingPlanarYCbCrImage {
 public:
  IMFYCbCrImage(IMFMediaBuffer* aBuffer, IMF2DBuffer* a2DBuffer,
                KnowsCompositor* aKnowsCompositor, ImageContainer* aContainer);

  bool IsValid() const override { return true; }

  TextureClient* GetTextureClient(KnowsCompositor* aForwarder) override;
  static DXGIYCbCrTextureData* GetD3D9TextureData(Data aData,
                                                  gfx::IntSize aSize);

 protected:
  TextureClient* GetD3D9TextureClient(KnowsCompositor* aForwarder);
  TextureClient* GetD3D11TextureClient(KnowsCompositor* aForwarder);
  static bool CopyDataToTexture(const Data& aData, ID3D11Device* aDevice,
                                DXGIYCbCrTextureData* aTextureData);

  virtual ~IMFYCbCrImage();

  RefPtr<IMFMediaBuffer> mBuffer;
  RefPtr<IMF2DBuffer> m2DBuffer;
  RefPtr<D3D11YCbCrRecycleAllocator> mAllocator;
  RefPtr<TextureClient> mTextureClient;
};

}  // namespace layers
}  // namespace mozilla

#endif  // GFX_D3DSURFACEIMAGE_H
