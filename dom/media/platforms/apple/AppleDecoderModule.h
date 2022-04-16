/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_AppleDecoderModule_h
#define mozilla_AppleDecoderModule_h

#include "PlatformDecoderModule.h"

namespace mozilla {

class AppleDecoderModule : public PlatformDecoderModule {
 public:
  AppleDecoderModule();
  virtual ~AppleDecoderModule();

  nsresult Startup() override;

  // Decode thread.
  already_AddRefed<MediaDataDecoder> CreateVideoDecoder(
      const CreateDecoderParams& aParams) override;

  // Decode thread.
  already_AddRefed<MediaDataDecoder> CreateAudioDecoder(
      const CreateDecoderParams& aParams) override;

  bool SupportsMimeType(const nsACString& aMimeType,
                        DecoderDoctorDiagnostics* aDiagnostics) const override;

  static void Init();

  static bool sCanUseHardwareVideoDecoder;

 private:
  static bool sInitialized;
};

}  // namespace mozilla

#endif  // mozilla_AppleDecoderModule_h
