/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef FFmpegRDFTTypes_h
#define FFmpegRDFTTypes_h

struct RDFTContext;

typedef float FFTSample;

enum RDFTransformType {
  DFT_R2C,
  IDFT_C2R,
  IDFT_R2C,
  DFT_C2R,
};

extern "C" {

typedef RDFTContext* (*AvRdftInitFn)(int nbits, enum RDFTransformType trans);
typedef void (*AvRdftCalcFn)(RDFTContext* s, FFTSample* data);
typedef void (*AvRdftEndFn)(RDFTContext* s);
}

struct FFmpegRDFTFuncs {
  AvRdftInitFn init;
  AvRdftCalcFn calc;
  AvRdftEndFn end;
};

#endif  // FFmpegRDFTTypes_h
