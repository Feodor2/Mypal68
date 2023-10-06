/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "IMFYCbCrImage.h"
#include "DeviceManagerD3D9.h"
#include "mozilla/gfx/DeviceManagerDx.h"
#include "mozilla/gfx/gfxVars.h"
#include "mozilla/gfx/Types.h"
#include "mozilla/layers/TextureD3D11.h"
#include "mozilla/layers/CompositableClient.h"
#include "mozilla/layers/CompositableForwarder.h"
#include "mozilla/layers/D3D11YCbCrImage.h"
#include "mozilla/layers/TextureClient.h"
#include "d3d9.h"

namespace mozilla {
namespace layers {

IMFYCbCrImage::IMFYCbCrImage(IMFMediaBuffer* aBuffer, IMF2DBuffer* a2DBuffer,
                             KnowsCompositor* aKnowsCompositor,
                             ImageContainer* aContainer)
    : RecyclingPlanarYCbCrImage(nullptr),
      mBuffer(aBuffer),
      m2DBuffer(a2DBuffer) {
  mAllocator = aContainer->GetD3D11YCbCrRecycleAllocator(aKnowsCompositor);
}

IMFYCbCrImage::~IMFYCbCrImage() {
  if (m2DBuffer) {
    m2DBuffer->Unlock2D();
  } else {
    mBuffer->Unlock();
  }
}

/* static */
bool IMFYCbCrImage::CopyDataToTexture(const Data& aData, ID3D11Device* aDevice,
                                      DXGIYCbCrTextureData* aTextureData) {
  MOZ_ASSERT(aTextureData);

  HRESULT hr;
  RefPtr<ID3D10Multithread> mt;

  hr = aDevice->QueryInterface((ID3D10Multithread**)getter_AddRefs(mt));
  if (FAILED(hr)) {
    return false;
  }

  if (!mt->GetMultithreadProtected()) {
    return false;
  }

  if (!gfx::DeviceManagerDx::Get()->CanInitializeKeyedMutexTextures()) {
    return false;
  }

  ID3D11Texture2D* textureY = aTextureData->GetD3D11Texture(0);
  ID3D11Texture2D* textureCb = aTextureData->GetD3D11Texture(1);
  ID3D11Texture2D* textureCr = aTextureData->GetD3D11Texture(2);

  D3D11MTAutoEnter mtAutoEnter(mt.forget());

  RefPtr<ID3D11DeviceContext> ctx;
  aDevice->GetImmediateContext(getter_AddRefs(ctx));
  if (!ctx) {
    gfxCriticalError() << "Failed to get immediate context.";
    return false;
  }

  // The documentation here seems to suggest using the immediate mode context
  // on more than one thread is not allowed:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476891(v=vs.85).aspx
  // The Debug Layer seems to imply it is though. When the ID3D10Multithread
  // layer is on. The Enter/Leave of the critical section shouldn't even be
  // required but were added for extra security.

  {
    AutoLockD3D11Texture lockY(textureY);
    AutoLockD3D11Texture lockCr(textureCr);
    AutoLockD3D11Texture lockCb(textureCb);
    D3D11MTAutoEnter mtAutoEnter(mt.forget());

    D3D11_BOX box;
    box.front = box.top = box.left = 0;
    box.back = 1;
    box.right = aData.mYSize.width;
    box.bottom = aData.mYSize.height;
    ctx->UpdateSubresource(textureY, 0, &box, aData.mYChannel, aData.mYStride,
                           0);

    box.right = aData.mCbCrSize.width;
    box.bottom = aData.mCbCrSize.height;
    ctx->UpdateSubresource(textureCb, 0, &box, aData.mCbChannel,
                           aData.mCbCrStride, 0);
    ctx->UpdateSubresource(textureCr, 0, &box, aData.mCrChannel,
                           aData.mCbCrStride, 0);
  }

  return true;
}

static already_AddRefed<IDirect3DTexture9> InitTextures(
    IDirect3DDevice9* aDevice, const IntSize& aSize, _D3DFORMAT aFormat,
    RefPtr<IDirect3DSurface9>& aSurface, HANDLE& aHandle,
    D3DLOCKED_RECT& aLockedRect) {
  if (!aDevice) {
    return nullptr;
  }

  RefPtr<IDirect3DTexture9> result;
  if (FAILED(aDevice->CreateTexture(aSize.width, aSize.height, 1, 0, aFormat,
                                    D3DPOOL_DEFAULT, getter_AddRefs(result),
                                    &aHandle))) {
    return nullptr;
  }
  if (!result) {
    return nullptr;
  }

  RefPtr<IDirect3DTexture9> tmpTexture;
  if (FAILED(aDevice->CreateTexture(aSize.width, aSize.height, 1, 0, aFormat,
                                    D3DPOOL_SYSTEMMEM,
                                    getter_AddRefs(tmpTexture), nullptr))) {
    return nullptr;
  }
  if (!tmpTexture) {
    return nullptr;
  }

  tmpTexture->GetSurfaceLevel(0, getter_AddRefs(aSurface));
  if (FAILED(aSurface->LockRect(&aLockedRect, nullptr, 0)) ||
      !aLockedRect.pBits) {
    NS_WARNING("Could not lock surface");
    return nullptr;
  }

  return result.forget();
}

static bool FinishTextures(IDirect3DDevice9* aDevice,
                           IDirect3DTexture9* aTexture,
                           IDirect3DSurface9* aSurface) {
  if (!aDevice) {
    return false;
  }

  HRESULT hr = aSurface->UnlockRect();
  if (FAILED(hr)) {
    return false;
  }

  RefPtr<IDirect3DSurface9> dstSurface;
  hr = aTexture->GetSurfaceLevel(0, getter_AddRefs(dstSurface));
  if (FAILED(hr)) {
    return false;
  }

  hr = aDevice->UpdateSurface(aSurface, nullptr, dstSurface, nullptr);
  if (FAILED(hr)) {
    return false;
  }
  return true;
}

static bool UploadData(IDirect3DDevice9* aDevice,
                       RefPtr<IDirect3DTexture9>& aTexture, HANDLE& aHandle,
                       uint8_t* aSrc, const gfx::IntSize& aSrcSize,
                       int32_t aSrcStride) {
  RefPtr<IDirect3DSurface9> surf;
  D3DLOCKED_RECT rect;
  aTexture = InitTextures(aDevice, aSrcSize, D3DFMT_A8, surf, aHandle, rect);
  if (!aTexture) {
    return false;
  }

  if (aSrcStride == rect.Pitch) {
    memcpy(rect.pBits, aSrc, rect.Pitch * aSrcSize.height);
  } else {
    for (int i = 0; i < aSrcSize.height; i++) {
      memcpy((uint8_t*)rect.pBits + i * rect.Pitch, aSrc + i * aSrcStride,
             aSrcSize.width);
    }
  }

  return FinishTextures(aDevice, aTexture, surf);
}

DXGIYCbCrTextureData* IMFYCbCrImage::GetD3D9TextureData(Data aData,
                                                        gfx::IntSize aSize) {
  RefPtr<IDirect3DDevice9> device = DeviceManagerD3D9::GetDevice();
  if (!device) {
    return nullptr;
  }

  RefPtr<IDirect3DTexture9> textureY;
  HANDLE shareHandleY = 0;
  if (!UploadData(device, textureY, shareHandleY, aData.mYChannel, aData.mYSize,
                  aData.mYStride)) {
    return nullptr;
  }

  RefPtr<IDirect3DTexture9> textureCb;
  HANDLE shareHandleCb = 0;
  if (!UploadData(device, textureCb, shareHandleCb, aData.mCbChannel,
                  aData.mCbCrSize, aData.mCbCrStride)) {
    return nullptr;
  }

  RefPtr<IDirect3DTexture9> textureCr;
  HANDLE shareHandleCr = 0;
  if (!UploadData(device, textureCr, shareHandleCr, aData.mCrChannel,
                  aData.mCbCrSize, aData.mCbCrStride)) {
    return nullptr;
  }

  RefPtr<IDirect3DQuery9> query;
  HRESULT hr = device->CreateQuery(D3DQUERYTYPE_EVENT, getter_AddRefs(query));
  hr = query->Issue(D3DISSUE_END);

  int iterations = 0;
  bool valid = false;
  while (iterations < 10) {
    HRESULT hr = query->GetData(nullptr, 0, D3DGETDATA_FLUSH);
    if (hr == S_FALSE) {
      Sleep(1);
      iterations++;
      continue;
    }
    if (hr == S_OK) {
      valid = true;
    }
    break;
  }

  if (!valid) {
    return nullptr;
  }

  return DXGIYCbCrTextureData::Create(
      textureY, textureCb, textureCr, shareHandleY, shareHandleCb,
      shareHandleCr, aSize, aData.mYSize, aData.mCbCrSize, aData.mColorDepth,
      aData.mYUVColorSpace, aData.mColorRange);
}

TextureClient* IMFYCbCrImage::GetD3D9TextureClient(
    KnowsCompositor* aKnowsCompositor) {
  DXGIYCbCrTextureData* textureData = GetD3D9TextureData(mData, GetSize());
  if (textureData == nullptr) {
    return nullptr;
  }

  mTextureClient = TextureClient::CreateWithData(
      textureData, TextureFlags::DEFAULT, aKnowsCompositor->GetTextureForwarder());

  return mTextureClient;
}

TextureClient* IMFYCbCrImage::GetD3D11TextureClient(
    KnowsCompositor* aKnowsCompositor) {
  if (!mAllocator) {
    return nullptr;
  }

  RefPtr<ID3D11Device> device = gfx::DeviceManagerDx::Get()->GetImageDevice();
  if (!device) {
    return nullptr;
  }

  {
    DXGIYCbCrTextureAllocationHelper helper(mData, TextureFlags::DEFAULT,
                                            device);
    mTextureClient = mAllocator->CreateOrRecycle(helper);
  }

  if (!mTextureClient) {
    return nullptr;
  }

  DXGIYCbCrTextureData* data =
      mTextureClient->GetInternalData()->AsDXGIYCbCrTextureData();

  if (!CopyDataToTexture(mData, device, data)) {
    // Failed to copy data
    mTextureClient = nullptr;
    return nullptr;
  }

  return mTextureClient;
}

TextureClient* IMFYCbCrImage::GetTextureClient(
    KnowsCompositor* aKnowsCompositor) {
  if (mTextureClient) {
    return mTextureClient;
  }

  RefPtr<ID3D11Device> device = gfx::DeviceManagerDx::Get()->GetImageDevice();

  LayersBackend backend = aKnowsCompositor->GetCompositorBackendType();
  if (!device || backend != LayersBackend::LAYERS_D3D11) {
    if (backend == LayersBackend::LAYERS_D3D9) {
      return GetD3D9TextureClient(aKnowsCompositor);
    }
    return nullptr;
  }
  return GetD3D11TextureClient(aKnowsCompositor);
}

}  // namespace layers
}  // namespace mozilla
