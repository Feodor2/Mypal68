/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkComposeImageFilter.h"
#include "SkColorSpaceXformer.h"
#include "SkImageFilterPriv.h"
#include "SkReadBuffer.h"
#include "SkSpecialImage.h"
#include "SkWriteBuffer.h"

sk_sp<SkImageFilter> SkComposeImageFilter::Make(sk_sp<SkImageFilter> outer,
                                                sk_sp<SkImageFilter> inner) {
    if (!outer) {
        return inner;
    }
    if (!inner) {
        return outer;
    }
    sk_sp<SkImageFilter> inputs[2] = { std::move(outer), std::move(inner) };
    return sk_sp<SkImageFilter>(new SkComposeImageFilter(inputs));
}

SkRect SkComposeImageFilter::computeFastBounds(const SkRect& src) const {
    SkImageFilter* outer = this->getInput(0);
    SkImageFilter* inner = this->getInput(1);

    return outer->computeFastBounds(inner->computeFastBounds(src));
}

sk_sp<SkSpecialImage> SkComposeImageFilter::onFilterImage(SkSpecialImage* source,
                                                          const Context& ctx,
                                                          SkIPoint* offset) const {
    // The bounds passed to the inner filter must be filtered by the outer
    // filter, so that the inner filter produces the pixels that the outer
    // filter requires as input. This matters if the outer filter moves pixels.
    SkIRect innerClipBounds;
    innerClipBounds = this->getInput(0)->filterBounds(ctx.clipBounds(), ctx.ctm(),
                                                      kReverse_MapDirection, &ctx.clipBounds());
    Context innerContext(ctx.ctm(), innerClipBounds, ctx.cache(), ctx.outputProperties());
    SkIPoint innerOffset = SkIPoint::Make(0, 0);
    sk_sp<SkSpecialImage> inner(this->filterInput(1, source, innerContext, &innerOffset));
    if (!inner) {
        return nullptr;
    }

    SkMatrix outerMatrix(ctx.ctm());
    outerMatrix.postTranslate(SkIntToScalar(-innerOffset.x()), SkIntToScalar(-innerOffset.y()));
    SkIRect clipBounds = ctx.clipBounds();
    clipBounds.offset(-innerOffset.x(), -innerOffset.y());
    Context outerContext(outerMatrix, clipBounds, ctx.cache(), ctx.outputProperties());

    SkIPoint outerOffset = SkIPoint::Make(0, 0);
    sk_sp<SkSpecialImage> outer(this->filterInput(0, inner.get(), outerContext, &outerOffset));
    if (!outer) {
        return nullptr;
    }

    *offset = innerOffset + outerOffset;
    return outer;
}

sk_sp<SkImageFilter> SkComposeImageFilter::onMakeColorSpace(SkColorSpaceXformer* xformer) const {
    SkASSERT(2 == this->countInputs() && this->getInput(0) && this->getInput(1));

    auto input0 = xformer->apply(this->getInput(0));
    auto input1 = xformer->apply(this->getInput(1));
    if (input0.get() != this->getInput(0) || input1.get() != this->getInput(1)) {
        return SkComposeImageFilter::Make(std::move(input0), std::move(input1));
    }
    return this->refMe();
}

SkIRect SkComposeImageFilter::onFilterBounds(const SkIRect& src, const SkMatrix& ctm,
                                             MapDirection dir, const SkIRect* inputRect) const {
    SkImageFilter* outer = this->getInput(0);
    SkImageFilter* inner = this->getInput(1);

    const SkIRect innerRect = inner->filterBounds(src, ctm, dir, inputRect);
    return outer->filterBounds(innerRect, ctm, dir, &innerRect);
}

sk_sp<SkFlattenable> SkComposeImageFilter::CreateProc(SkReadBuffer& buffer) {
    SK_IMAGEFILTER_UNFLATTEN_COMMON(common, 2);
    return SkComposeImageFilter::Make(common.getInput(0), common.getInput(1));
}
