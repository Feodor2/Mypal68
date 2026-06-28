/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_ReadableStreamPipeTo_h
#define mozilla_dom_ReadableStreamPipeTo_h

#include "mozilla/Attributes.h"
#include "mozilla/AlreadyAddRefed.h"

namespace mozilla {

class ErrorResult;

namespace dom {

class AbortSignal;
class Promise;
class ReadableStream;
class WritableStream;

namespace streams_abstract {
MOZ_CAN_RUN_SCRIPT already_AddRefed<Promise> ReadableStreamPipeTo(
    ReadableStream* aSource, WritableStream* aDest, bool aPreventClose,
    bool aPreventAbort, bool aPreventCancel, AbortSignal* aSignal,
    ErrorResult& aRv);
}

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_ReadableStreamPipeTo_h
