/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This is the interface that the regexp engine exposes to SpiderMonkey. */

#ifndef regexp_RegExpAPI_h
#define regexp_RegExpAPI_h

#include "mozilla/Maybe.h"
#include "mozilla/MemoryReporting.h"

#include "frontend/TokenStream.h"
#include "irregexp/RegExpTypes.h"
#include "vm/JSContext.h"
#include "vm/RegExpShared.h"

namespace js {
namespace irregexp {

Isolate* CreateIsolate(JSContext* cx);
void DestroyIsolate(Isolate* isolate);

size_t IsolateSizeOfIncludingThis(Isolate* isolate,
                                  mozilla::MallocSizeOf mallocSizeOf);

bool CheckPatternSyntax(JSContext* cx, frontend::TokenStreamAnyChars& ts,
                        const mozilla::Range<const char16_t> chars,
                        JS::RegExpFlags flags,
                        mozilla::Maybe<uint32_t> line = mozilla::Nothing(),
                        mozilla::Maybe<uint32_t> column = mozilla::Nothing());
bool CheckPatternSyntax(JSContext* cx, frontend::TokenStreamAnyChars& ts,
                        HandleAtom pattern, JS::RegExpFlags flags);

bool CompilePattern(JSContext* cx, MutableHandleRegExpShared re,
                    HandleLinearString input, RegExpShared::CodeKind codeKind);

RegExpRunStatus Execute(JSContext* cx, MutableHandleRegExpShared re,
                        HandleLinearString input, size_t start,
                        VectorMatchPairs* matches);

RegExpRunStatus ExecuteForFuzzing(JSContext* cx, HandleAtom pattern,
                                  HandleLinearString input,
                                  JS::RegExpFlags flags, size_t startIndex,
                                  VectorMatchPairs* matches,
                                  RegExpShared::CodeKind codeKind);

bool GrowBacktrackStack(v8::internal::RegExpStack* regexp_stack);

uint32_t CaseInsensitiveCompareNonUnicode(const char16_t* substring1,
                                          const char16_t* substring2,
                                          size_t byteLength);
uint32_t CaseInsensitiveCompareUnicode(const char16_t* substring1,
                                       const char16_t* substring2,
                                       size_t byteLength);

}  // namespace irregexp
}  // namespace js

#endif /* regexp_RegExpAPI_h */
