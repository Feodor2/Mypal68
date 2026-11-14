/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_FetchStreamReader_h
#define mozilla_dom_FetchStreamReader_h

#include "js/RootingAPI.h"
#include "js/TypeDecls.h"
#include "mozilla/Attributes.h"
#include "mozilla/WeakPtr.h"
#include "mozilla/dom/FetchBinding.h"
#include "mozilla/dom/PromiseNativeHandler.h"
#include "nsIAsyncOutputStream.h"
#include "nsIGlobalObject.h"

namespace mozilla::dom {

class ReadableStream;
class ReadableStreamDefaultReader;

class FetchStreamReader final : public nsISupports, public SupportsWeakPtr {
 public:
  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_CLASS(FetchStreamReader)

  // This creates a nsIInputStream able to retrieve data from the ReadableStream
  // object. The reading starts when StartConsuming() is called.
  static nsresult Create(JSContext* aCx, nsIGlobalObject* aGlobal,
                         FetchStreamReader** aStreamReader,
                         nsIInputStream** aInputStream);

  void ChunkSteps(JSContext* aCx, JS::Handle<JS::Value> aChunk,
                  ErrorResult& aRv);
  void ErrorSteps(JSContext* aCx, JS::Handle<JS::Value> aError,
                  ErrorResult& aRv);

  // Idempotently close the output stream and null out all state. If aCx is
  // provided, the reader will also be canceled.  aStatus must be a DOM error
  // as understood by DOMException because it will be provided as the
  // cancellation reason.
  //
  // This is a script boundary minimize annotation changes required while
  // we figure out how to handle some more tricky annotation cases (for
  // example, the destructor of this class. Tracking under Bug 1750656)
  MOZ_CAN_RUN_SCRIPT_BOUNDARY
  void CloseAndRelease(JSContext* aCx, nsresult aStatus);

  void StartConsuming(JSContext* aCx, ReadableStream* aStream,
                      ReadableStreamDefaultReader** aReader, ErrorResult& aRv);

  MOZ_CAN_RUN_SCRIPT_BOUNDARY
  void OnOutputStreamReady();

  nsIGlobalObject* GetParentObject() const { return mGlobal; }

 private:
  class OutputStreamHolder;

  explicit FetchStreamReader(nsIGlobalObject* aGlobal);
  ~FetchStreamReader();

  nsresult WriteBuffer();

  void ReportErrorToConsole(JSContext* aCx, JS::Handle<JS::Value> aValue);

  nsCOMPtr<nsIGlobalObject> mGlobal;
  nsCOMPtr<nsIEventTarget> mOwningEventTarget;

  RefPtr<OutputStreamHolder> mOutput;

  RefPtr<ReadableStreamDefaultReader> mReader;

  nsTArray<uint8_t> mBuffer;
  uint32_t mBufferRemaining = 0;
  uint32_t mBufferOffset = 0;

  bool mStreamClosed = false;
};

}  // namespace mozilla::dom

#endif  // mozilla_dom_FetchStreamReader_h
