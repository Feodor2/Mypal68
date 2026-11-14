/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FetchStreamReader.h"
#include "InternalResponse.h"
#include "mozilla/ConsoleReportCollector.h"
#include "mozilla/ErrorResult.h"
#include "mozilla/Services.h"
#include "mozilla/TaskCategory.h"
#include "mozilla/dom/AutoEntryScript.h"
#include "mozilla/dom/Promise.h"
#include "mozilla/dom/PromiseBinding.h"
#include "mozilla/dom/ReadableStream.h"
#include "mozilla/dom/ReadableStreamDefaultController.h"
#include "mozilla/dom/ReadableStreamDefaultReader.h"
#include "mozilla/dom/WorkerPrivate.h"
#include "mozilla/dom/WorkerRef.h"
#include "nsContentUtils.h"
#include "nsDebug.h"
#include "nsIAsyncInputStream.h"
#include "nsIObserver.h"
#include "nsIObserverService.h"
#include "nsIPipe.h"
#include "nsIScriptError.h"
#include "nsPIDOMWindow.h"
#include "nsWeakReference.h"
#include "jsapi.h"

namespace mozilla::dom {

// Non-CC holder around nsIAsyncOutputStream (Bug 1939295 + Bug 1941210),
// paired with DOM_WINDOW_DESTROYED_TOPIC teardown on the main thread (matching
// BodyStream) and StrongWorkerRef teardown on workers.
class FetchStreamReader::OutputStreamHolder final
    : public nsIOutputStreamCallback,
      public nsIObserver,
      public nsSupportsWeakReference {
 public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOUTPUTSTREAMCALLBACK
  NS_DECL_NSIOBSERVER

  explicit OutputStreamHolder(FetchStreamReader* aReader) : mReader(aReader) {
    MOZ_ASSERT(aReader);
  }

  nsresult Init(JSContext* aCx, nsIAsyncInputStream** aInputStream) {
    nsresult rv = NS_NewPipe2(aInputStream, getter_AddRefs(mOutput), true, true,
                              0, 0);
    if (NS_WARN_IF(NS_FAILED(rv))) {
      return rv;
    }

    if (NS_IsMainThread()) {
      nsCOMPtr<nsIObserverService> os = mozilla::services::GetObserverService();
      if (NS_WARN_IF(!os)) {
        mOutput->CloseWithStatus(NS_ERROR_DOM_INVALID_STATE_ERR);
        return NS_ERROR_DOM_INVALID_STATE_ERR;
      }
      rv = os->AddObserver(this, DOM_WINDOW_DESTROYED_TOPIC, true);
      if (NS_WARN_IF(NS_FAILED(rv))) {
        mOutput->CloseWithStatus(rv);
        return rv;
      }
      mObservingWindowDestroy = true;
    } else {
      WorkerPrivate* workerPrivate = GetWorkerPrivateFromContext(aCx);
      MOZ_ASSERT(workerPrivate);

      RefPtr<OutputStreamHolder> self = this;
      RefPtr<StrongWorkerRef> workerRef = StrongWorkerRef::Create(
          workerPrivate, "FetchStreamReader", [self]() {
            if (RefPtr<FetchStreamReader> reader = self->mReader.get()) {
              reader->CloseAndRelease(nullptr, NS_ERROR_DOM_INVALID_STATE_ERR);
            } else {
              self->Shutdown(NS_ERROR_DOM_INVALID_STATE_ERR);
            }
          });
      if (NS_WARN_IF(!workerRef)) {
        mOutput->CloseWithStatus(NS_ERROR_DOM_INVALID_STATE_ERR);
        return NS_ERROR_DOM_INVALID_STATE_ERR;
      }
      mWorkerRef = std::move(workerRef);
    }

    return NS_OK;
  }

  void Shutdown(nsresult aStatus) {
    if (mObservingWindowDestroy && NS_IsMainThread()) {
      if (nsCOMPtr<nsIObserverService> os =
              mozilla::services::GetObserverService()) {
        os->RemoveObserver(this, DOM_WINDOW_DESTROYED_TOPIC);
      }
      mObservingWindowDestroy = false;
    }

    mAsyncWaitReader = nullptr;
    mWorkerRef = nullptr;

    if (mOutput) {
      mOutput->CloseWithStatus(aStatus);
    }
  }

  nsresult AsyncWait(uint32_t aFlags, uint32_t aRequestedCount,
                     nsIEventTarget* aTarget) {
    if (!mOutput) {
      return NS_BASE_STREAM_CLOSED;
    }
    nsresult rv = mOutput->AsyncWait(this, aFlags, aRequestedCount, aTarget);
    if (NS_SUCCEEDED(rv)) {
      // Keep FetchStreamReader alive until OnOutputStreamReady runs (Bug
      // 1941210 / Bug 1942573); Shutdown() clears this on window/worker exit.
      mAsyncWaitReader = mReader.get();
    }
    return rv;
  }

  nsresult Write(const char* aBuf, uint32_t aCount, uint32_t* aResult) {
    if (!mOutput) {
      return NS_BASE_STREAM_CLOSED;
    }
    return mOutput->Write(aBuf, aCount, aResult);
  }

 private:
  ~OutputStreamHolder() { Shutdown(NS_BASE_STREAM_CLOSED); }

  // FetchStreamReader outlives us during active reads, or clears us in
  // CloseAndRelease / CC unlink; WeakPtr avoids non-CC pipe ref cycles.
  WeakPtr<FetchStreamReader> mReader;
  RefPtr<FetchStreamReader> mAsyncWaitReader;

  nsCOMPtr<nsIAsyncOutputStream> mOutput;
  RefPtr<StrongWorkerRef> mWorkerRef;
  bool mObservingWindowDestroy = false;
};

NS_IMPL_ISUPPORTS(FetchStreamReader::OutputStreamHolder,
                  nsIOutputStreamCallback, nsIObserver,
                  nsISupportsWeakReference)

NS_IMETHODIMP
FetchStreamReader::OutputStreamHolder::OnOutputStreamReady(
    nsIAsyncOutputStream* aStream) {
  MOZ_ASSERT(aStream == mOutput);
  RefPtr<FetchStreamReader> reader = std::move(mAsyncWaitReader);
  if (!reader) {
    reader = mReader.get();
  }
  if (reader) {
    reader->OnOutputStreamReady();
  }
  return NS_OK;
}

NS_IMETHODIMP
FetchStreamReader::OutputStreamHolder::Observe(nsISupports* aSubject,
                                               const char* aTopic,
                                               const char16_t* aData) {
  MOZ_ASSERT(NS_IsMainThread());
  MOZ_ASSERT(strcmp(aTopic, DOM_WINDOW_DESTROYED_TOPIC) == 0);

  RefPtr<FetchStreamReader> reader = mReader.get();
  if (!reader) {
    Shutdown(NS_BASE_STREAM_CLOSED);
    return NS_OK;
  }

  nsCOMPtr<nsPIDOMWindowInner> window =
      do_QueryInterface(reader->GetParentObject());
  if (window && SameCOMIdentity(aSubject, window)) {
    reader->CloseAndRelease(nullptr, NS_BASE_STREAM_CLOSED);
  }
  return NS_OK;
}

NS_IMPL_CYCLE_COLLECTING_ADDREF(FetchStreamReader)
NS_IMPL_CYCLE_COLLECTING_RELEASE(FetchStreamReader)

NS_IMPL_CYCLE_COLLECTION_CLASS(FetchStreamReader)

NS_IMPL_CYCLE_COLLECTION_UNLINK_BEGIN(FetchStreamReader)
  tmp->CloseAndRelease(nullptr, NS_BASE_STREAM_CLOSED);
  NS_IMPL_CYCLE_COLLECTION_UNLINK(mGlobal)
  NS_IMPL_CYCLE_COLLECTION_UNLINK(mReader)
  NS_IMPL_CYCLE_COLLECTION_UNLINK_WEAK_PTR
NS_IMPL_CYCLE_COLLECTION_UNLINK_END

NS_IMPL_CYCLE_COLLECTION_TRAVERSE_BEGIN(FetchStreamReader)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE(mGlobal)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE(mReader)
NS_IMPL_CYCLE_COLLECTION_TRAVERSE_END

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(FetchStreamReader)
  NS_INTERFACE_MAP_ENTRY(nsISupports)
NS_INTERFACE_MAP_END

/* static */
nsresult FetchStreamReader::Create(JSContext* aCx, nsIGlobalObject* aGlobal,
                                   FetchStreamReader** aStreamReader,
                                   nsIInputStream** aInputStream) {
  MOZ_ASSERT(aCx);
  MOZ_ASSERT(aGlobal);
  MOZ_ASSERT(aStreamReader);
  MOZ_ASSERT(aInputStream);

  RefPtr<FetchStreamReader> streamReader = new FetchStreamReader(aGlobal);
  RefPtr<OutputStreamHolder> output = new OutputStreamHolder(streamReader);

  nsCOMPtr<nsIAsyncInputStream> pipeIn;
  nsresult rv = output->Init(aCx, getter_AddRefs(pipeIn));
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return rv;
  }

  streamReader->mOutput = std::move(output);

  pipeIn.forget(aInputStream);
  streamReader.forget(aStreamReader);
  return NS_OK;
}

FetchStreamReader::FetchStreamReader(nsIGlobalObject* aGlobal)
    : mGlobal(aGlobal),
      mOwningEventTarget(mGlobal->EventTargetFor(TaskCategory::Other)) {
  MOZ_ASSERT(aGlobal);
}

FetchStreamReader::~FetchStreamReader() {
  CloseAndRelease(nullptr, NS_BASE_STREAM_CLOSED);
}

// If a context is provided, an attempt will be made to cancel the reader.  The
// only situation where we don't expect to have a context is when closure is
// being triggered from the destructor, CC unlink, window destroy observer, or
// WorkerRef notification.
void FetchStreamReader::CloseAndRelease(JSContext* aCx, nsresult aStatus) {
  NS_ASSERT_OWNINGTHREAD(FetchStreamReader);

  if (mStreamClosed) {
    // Already closed.
    return;
  }

  RefPtr<FetchStreamReader> kungFuDeathGrip = this;
  if (aCx && mReader) {
    ErrorResult rv;
    if (aStatus == NS_ERROR_DOM_WRONG_TYPE_ERR) {
      rv.ThrowTypeError<MSG_FETCH_BODY_WRONG_TYPE>();
    } else {
      rv = aStatus;
    }
    JS::Rooted<JS::Value> errorValue(aCx);
    if (ToJSValue(aCx, std::move(rv), &errorValue)) {
      IgnoredErrorResult ignoredError;
      RefPtr<Promise> ignoredResultPromise =
          MOZ_KnownLive(mReader)->Cancel(aCx, errorValue, ignoredError);
      NS_WARNING_ASSERTION(!ignoredError.Failed(),
                           "Failed to cancel stream during close and release");
    }

    // We don't want to propagate exceptions during the cleanup.
    JS_ClearPendingException(aCx);
  }

  mStreamClosed = true;

  mGlobal = nullptr;

  if (mOutput) {
    mOutput->Shutdown(aStatus);
    mOutput = nullptr;
  }

  mReader = nullptr;
  mBuffer.Clear();
}

// https://fetch.spec.whatwg.org/#body-incrementally-read
void FetchStreamReader::StartConsuming(JSContext* aCx, ReadableStream* aStream,
                                       ReadableStreamDefaultReader** aReader,
                                       ErrorResult& aRv) {
  MOZ_DIAGNOSTIC_ASSERT(!mReader);
  MOZ_DIAGNOSTIC_ASSERT(aStream);

  if (mStreamClosed || !mOutput) {
    aRv.Throw(NS_ERROR_DOM_INVALID_STATE_ERR);
    return;
  }

  // Step 2: Let reader be the result of getting a reader for body’s stream.
  RefPtr<ReadableStreamDefaultReader> reader = aStream->GetReader(aRv);
  if (aRv.Failed()) {
    CloseAndRelease(aCx, NS_ERROR_DOM_INVALID_STATE_ERR);
    return;
  }

  mReader = reader;
  reader.forget(aReader);

  aRv = mOutput->AsyncWait(0, 0, mOwningEventTarget);
  if (NS_WARN_IF(aRv.Failed())) {
    return;
  }
}

struct FetchReadRequest : public ReadRequest {
 public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_CYCLE_COLLECTION_CLASS_INHERITED(FetchReadRequest, ReadRequest)

  explicit FetchReadRequest(FetchStreamReader* aReader)
      : mFetchStreamReader(aReader) {}

  void ChunkSteps(JSContext* aCx, JS::Handle<JS::Value> aChunk,
                  ErrorResult& aRv) override {
    mFetchStreamReader->ChunkSteps(aCx, aChunk, aRv);
  }

  void CloseSteps(JSContext* aCx, ErrorResult& aRv) override {
    mFetchStreamReader->CloseAndRelease(aCx, NS_BASE_STREAM_CLOSED);
  }

  void ErrorSteps(JSContext* aCx, JS::Handle<JS::Value> aError,
                  ErrorResult& aRv) override {
    mFetchStreamReader->ErrorSteps(aCx, aError, aRv);
  }

 protected:
  virtual ~FetchReadRequest() = default;

  RefPtr<FetchStreamReader> mFetchStreamReader;
};

NS_IMPL_CYCLE_COLLECTION_INHERITED(FetchReadRequest, ReadRequest,
                                   mFetchStreamReader)
NS_IMPL_ADDREF_INHERITED(FetchReadRequest, ReadRequest)
NS_IMPL_RELEASE_INHERITED(FetchReadRequest, ReadRequest)
NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(FetchReadRequest)
NS_INTERFACE_MAP_END_INHERITING(ReadRequest)

void FetchStreamReader::OnOutputStreamReady() {
  NS_ASSERT_OWNINGTHREAD(FetchStreamReader);
  if (mStreamClosed) {
    return;
  }

  MOZ_ASSERT(mReader);

  if (!mBuffer.IsEmpty()) {
    nsresult rv = WriteBuffer();
    if (NS_WARN_IF(NS_FAILED(rv))) {
      CloseAndRelease(nullptr, NS_ERROR_DOM_ABORT_ERR);
    }
    return;
  }

  // Here we can retrieve data from the reader using any global we want because
  // it is not observable. We want to use the reader's global, which is also the
  // Response's one.
  AutoEntryScript aes(mGlobal, "ReadableStreamReader.read", NS_IsMainThread());

  IgnoredErrorResult rv;

  // https://fetch.spec.whatwg.org/#incrementally-read-loop
  // The below very loosely tries to implement the incrementally-read-loop from
  // the fetch spec.
  // Step 2: Read a chunk from reader given readRequest.
  RefPtr<ReadRequest> readRequest = new FetchReadRequest(this);
  RefPtr<ReadableStreamDefaultReader> reader = mReader;
  reader->ReadChunk(aes.cx(), *readRequest, rv);

  if (NS_WARN_IF(rv.Failed())) {
    // Let's close the stream.
    CloseAndRelease(aes.cx(), NS_ERROR_DOM_INVALID_STATE_ERR);
  }
}

void FetchStreamReader::ChunkSteps(JSContext* aCx, JS::Handle<JS::Value> aChunk,
                                   ErrorResult& aRv) {
  // This roughly implements the chunk steps from
  // https://fetch.spec.whatwg.org/#incrementally-read-loop.

  // Step 2. If chunk is not a Uint8Array object, then set continueAlgorithm to
  // this step: run processBodyError given a TypeError.
  RootedSpiderMonkeyInterface<Uint8Array> chunk(aCx);
  if (!aChunk.isObject() || !chunk.Init(&aChunk.toObject())) {
    CloseAndRelease(aCx, NS_ERROR_DOM_WRONG_TYPE_ERR);
    return;
  }
  chunk.ComputeState();

  uint32_t len = chunk.Length();
  if (len == 0) {
    // If there is nothing to read, let's do another reading.
    OnOutputStreamReady();
    return;
  }

  MOZ_DIAGNOSTIC_ASSERT(mBuffer.IsEmpty());

  // Let's take a copy of the data.
  if (!mBuffer.AppendElements(chunk.Data(), len, fallible)) {
    CloseAndRelease(aCx, NS_ERROR_OUT_OF_MEMORY);
    return;
  }

  mBufferOffset = 0;
  mBufferRemaining = len;

  nsresult rv = WriteBuffer();
  if (NS_WARN_IF(NS_FAILED(rv))) {
    CloseAndRelease(aCx, NS_ERROR_DOM_ABORT_ERR);
  }
}

void FetchStreamReader::ErrorSteps(JSContext* aCx, JS::Handle<JS::Value> aError,
                                   ErrorResult& aRv) {
  ReportErrorToConsole(aCx, aError);
  CloseAndRelease(aCx, NS_ERROR_FAILURE);
}

nsresult FetchStreamReader::WriteBuffer() {
  MOZ_ASSERT(mBuffer.Length() == (mBufferOffset + mBufferRemaining));
  if (!mOutput) {
    return NS_BASE_STREAM_CLOSED;
  }

  char* data = reinterpret_cast<char*>(mBuffer.Elements());

  while (mBufferRemaining > 0) {
    uint32_t written = 0;
    nsresult rv =
        mOutput->Write(data + mBufferOffset, mBufferRemaining, &written);

    if (rv == NS_BASE_STREAM_WOULD_BLOCK) {
      break;
    }

    if (NS_WARN_IF(NS_FAILED(rv))) {
      return rv;
    }

    MOZ_ASSERT(written <= mBufferRemaining);
    mBufferRemaining -= written;
    mBufferOffset += written;

    if (mBufferRemaining == 0) {
      mBuffer.Clear();
      break;
    }
  }

  nsresult rv = mOutput->AsyncWait(0, 0, mOwningEventTarget);
  if (NS_WARN_IF(NS_FAILED(rv))) {
    return rv;
  }

  return NS_OK;
}

void FetchStreamReader::ReportErrorToConsole(JSContext* aCx,
                                             JS::Handle<JS::Value> aValue) {
  nsCString sourceSpec;
  uint32_t line = 0;
  uint32_t column = 0;
  nsString valueString;

  nsContentUtils::ExtractErrorValues(aCx, aValue, sourceSpec, &line, &column,
                                     valueString);

  nsTArray<nsString> params;
  params.AppendElement(valueString);

  RefPtr<ConsoleReportCollector> reporter = new ConsoleReportCollector();
  reporter->AddConsoleReport(nsIScriptError::errorFlag,
                             "ReadableStreamReader.read"_ns,
                             nsContentUtils::eDOM_PROPERTIES, sourceSpec, line,
                             column, "ReadableStreamReadingFailed"_ns, params);

  uint64_t innerWindowId = 0;

  if (NS_IsMainThread()) {
    nsCOMPtr<nsPIDOMWindowInner> window = do_QueryInterface(mGlobal);
    if (window) {
      innerWindowId = window->WindowID();
    }
    reporter->FlushReportsToConsole(innerWindowId);
    return;
  }

  WorkerPrivate* workerPrivate = GetWorkerPrivateFromContext(aCx);
  if (workerPrivate) {
    innerWindowId = workerPrivate->WindowID();
  }

  RefPtr<Runnable> r = NS_NewRunnableFunction(
      "FetchStreamReader::ReportErrorToConsole", [reporter, innerWindowId]() {
        reporter->FlushReportsToConsole(innerWindowId);
      });

  workerPrivate->DispatchToMainThread(r.forget());
}

}  // namespace mozilla::dom
