/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "WMFMediaDataDecoder.h"
#include "VideoUtils.h"
#include "WMFUtils.h"
#include "mozilla/Telemetry.h"
#include "nsTArray.h"

#include "mozilla/Logging.h"
#include "mozilla/SyncRunnable.h"

#define LOG(...) MOZ_LOG(sPDMLog, mozilla::LogLevel::Debug, (__VA_ARGS__))

namespace mozilla {

WMFMediaDataDecoder::WMFMediaDataDecoder(MFTManager* aMFTManager,
                                         TaskQueue* aTaskQueue)
    : mTaskQueue(aTaskQueue), mMFTManager(aMFTManager) {}

WMFMediaDataDecoder::~WMFMediaDataDecoder() {}

RefPtr<MediaDataDecoder::InitPromise> WMFMediaDataDecoder::Init() {
  MOZ_ASSERT(!mIsShutDown);
  return InitPromise::CreateAndResolve(mMFTManager->GetType(), __func__);
}

RefPtr<ShutdownPromise> WMFMediaDataDecoder::Shutdown() {
  MOZ_DIAGNOSTIC_ASSERT(!mIsShutDown);

  mIsShutDown = true;

  if (mTaskQueue) {
    return InvokeAsync(mTaskQueue, this, __func__,
                       &WMFMediaDataDecoder::ProcessShutdown);
  }
  return ProcessShutdown();
}

RefPtr<ShutdownPromise> WMFMediaDataDecoder::ProcessShutdown() {
  if (mMFTManager) {
    mMFTManager->Shutdown();
    mMFTManager = nullptr;
  }
  return ShutdownPromise::CreateAndResolve(true, __func__);
}

// Inserts data into the decoder's pipeline.
RefPtr<MediaDataDecoder::DecodePromise> WMFMediaDataDecoder::Decode(
    MediaRawData* aSample) {
  MOZ_DIAGNOSTIC_ASSERT(!mIsShutDown);

  return InvokeAsync<MediaRawData*>(
      mTaskQueue, this, __func__, &WMFMediaDataDecoder::ProcessDecode, aSample);
}

RefPtr<MediaDataDecoder::DecodePromise> WMFMediaDataDecoder::ProcessError(
    HRESULT aError, const char* aReason) {

  // TODO: For the error DXGI_ERROR_DEVICE_RESET, we could return
  // NS_ERROR_DOM_MEDIA_NEED_NEW_DECODER to get the latest device. Maybe retry
  // up to 3 times.
  return DecodePromise::CreateAndReject(
      MediaResult(NS_ERROR_DOM_MEDIA_DECODE_ERR,
                  RESULT_DETAIL("%s:%x", aReason, aError)),
      __func__);
}

RefPtr<MediaDataDecoder::DecodePromise> WMFMediaDataDecoder::ProcessDecode(
    MediaRawData* aSample) {
  DecodedData results;
  HRESULT hr = mMFTManager->Input(aSample);
  if (hr == MF_E_NOTACCEPTING) {
    hr = ProcessOutput(results);
    if (FAILED(hr) && hr != MF_E_TRANSFORM_NEED_MORE_INPUT) {
      return ProcessError(hr, "MFTManager::Output(1)");
    }
    hr = mMFTManager->Input(aSample);
  }

  if (FAILED(hr)) {
    NS_WARNING("MFTManager rejected sample");
    return ProcessError(hr, "MFTManager::Input");
  }

  if (!mLastTime || aSample->mTime > *mLastTime) {
    mLastTime = Some(aSample->mTime);
    mLastDuration = aSample->mDuration;
  }
  mSamplesCount++;
  mDrainStatus = DrainStatus::DRAINABLE;
  mLastStreamOffset = aSample->mOffset;

  hr = ProcessOutput(results);
  if (SUCCEEDED(hr) || hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
    return DecodePromise::CreateAndResolve(std::move(results), __func__);
  }
  return ProcessError(hr, "MFTManager::Output(2)");
}

HRESULT
WMFMediaDataDecoder::ProcessOutput(DecodedData& aResults) {
  RefPtr<MediaData> output;
  HRESULT hr = S_OK;
  while (SUCCEEDED(hr = mMFTManager->Output(mLastStreamOffset, output))) {
    MOZ_ASSERT(output.get(), "Upon success, we must receive an output");
    aResults.AppendElement(std::move(output));
    if (mDrainStatus == DrainStatus::DRAINING) {
      break;
    }
  }
  return hr;
}

RefPtr<MediaDataDecoder::FlushPromise> WMFMediaDataDecoder::ProcessFlush() {
  if (mMFTManager) {
    mMFTManager->Flush();
  }
  mDrainStatus = DrainStatus::DRAINED;
  mSamplesCount = 0;
  mLastTime.reset();
  return FlushPromise::CreateAndResolve(true, __func__);
}

RefPtr<MediaDataDecoder::FlushPromise> WMFMediaDataDecoder::Flush() {
  MOZ_DIAGNOSTIC_ASSERT(!mIsShutDown);

  return InvokeAsync(mTaskQueue, this, __func__,
                     &WMFMediaDataDecoder::ProcessFlush);
}

RefPtr<MediaDataDecoder::DecodePromise> WMFMediaDataDecoder::ProcessDrain() {
  if (!mMFTManager || mDrainStatus == DrainStatus::DRAINED) {
    return DecodePromise::CreateAndResolve(DecodedData(), __func__);
  }

  if (mDrainStatus != DrainStatus::DRAINING) {
    // Order the decoder to drain...
    mMFTManager->Drain();
    mDrainStatus = DrainStatus::DRAINING;
  }

  // Then extract all available output.
  DecodedData results;
  HRESULT hr = ProcessOutput(results);
  if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
    mDrainStatus = DrainStatus::DRAINED;
  }
  if (SUCCEEDED(hr) || hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
    if (results.Length() > 0 &&
        results.LastElement()->mType == MediaData::Type::VIDEO_DATA) {
      const RefPtr<MediaData>& data = results.LastElement();
      if (mSamplesCount == 1 && data->mTime == media::TimeUnit::Zero()) {
        // WMF is unable to calculate a duration if only a single sample
        // was parsed. Additionally, the pts always comes out at 0 under those
        // circumstances.
        // Seeing that we've only fed the decoder a single frame, the pts
        // and duration are known, it's of the last sample.
        data->mTime = *mLastTime;
      }
      if (data->mTime == *mLastTime) {
        // The WMF Video decoder is sometimes unable to provide a valid duration
        // on the last sample even if it has been first set through
        // SetSampleTime (appears to always happen on Windows 7). So we force
        // set the duration of the last sample as it was input.
        data->mDuration = mLastDuration;
      }
    }
    return DecodePromise::CreateAndResolve(std::move(results), __func__);
  }
  return ProcessError(hr, "MFTManager::Output");
}

RefPtr<MediaDataDecoder::DecodePromise> WMFMediaDataDecoder::Drain() {
  MOZ_DIAGNOSTIC_ASSERT(!mIsShutDown);

  return InvokeAsync(mTaskQueue, this, __func__,
                     &WMFMediaDataDecoder::ProcessDrain);
}

bool WMFMediaDataDecoder::IsHardwareAccelerated(
    nsACString& aFailureReason) const {
  MOZ_ASSERT(!mIsShutDown);

  return mMFTManager && mMFTManager->IsHardwareAccelerated(aFailureReason);
}

void WMFMediaDataDecoder::SetSeekThreshold(const media::TimeUnit& aTime) {
  MOZ_DIAGNOSTIC_ASSERT(!mIsShutDown);

  RefPtr<WMFMediaDataDecoder> self = this;
  nsCOMPtr<nsIRunnable> runnable = NS_NewRunnableFunction(
      "WMFMediaDataDecoder::SetSeekThreshold", [self, aTime]() {
        media::TimeUnit threshold = aTime;
        self->mMFTManager->SetSeekThreshold(threshold);
      });
  nsresult rv = mTaskQueue->Dispatch(runnable.forget());
  MOZ_DIAGNOSTIC_ASSERT(NS_SUCCEEDED(rv));
  Unused << rv;
}

}  // namespace mozilla
