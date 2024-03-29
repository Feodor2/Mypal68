/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkNormalMapSource_DEFINED
#define SkNormalMapSource_DEFINED

#include "SkNormalSource.h"

class SkNormalMapSourceImpl : public SkNormalSource {
public:
    SkNormalMapSourceImpl(sk_sp<SkShader> mapShader, const SkMatrix& invCTM)
            : fMapShader(std::move(mapShader))
            , fInvCTM(invCTM) {}

#if SK_SUPPORT_GPU
    std::unique_ptr<GrFragmentProcessor> asFragmentProcessor(const GrFPArgs& args) const override;
#endif

    SkNormalSource::Provider* asProvider(const SkShaderBase::ContextRec& rec,
                                         SkArenaAlloc* alloc) const override;

protected:
    void flatten(SkWriteBuffer& buf) const override;

    bool computeNormTotalInverse(const SkShaderBase::ContextRec& rec,
                                 SkMatrix* normTotalInverse) const;

private:
    SK_FLATTENABLE_HOOKS(SkNormalMapSourceImpl)

    class Provider : public SkNormalSource::Provider {
    public:
        Provider(const SkNormalMapSourceImpl& source, SkShaderBase::Context* mapContext);

        void fillScanLine(int x, int y, SkPoint3 output[], int count) const override;

    private:
        const SkNormalMapSourceImpl& fSource;
        SkShaderBase::Context*       fMapContext;

        typedef SkNormalSource::Provider INHERITED;
    };

    sk_sp<SkShader> fMapShader;
    SkMatrix        fInvCTM; // Inverse of the canvas total matrix, used for rotating normals.

    friend class SkNormalSource;

    typedef SkNormalSource INHERITED;
};

#endif
