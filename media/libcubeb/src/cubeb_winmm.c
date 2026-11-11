/*
 * Copyright © 2011 Mozilla Foundation
 *
 * This program is made available under an ISC-style license.  See the
 * accompanying file LICENSE for details.
 */
#undef WINVER
#define WINVER 0x0501
#undef WIN32_LEAN_AND_MEAN

#include <malloc.h>
#include <windows.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <IntSafe.h>
#include "cubeb/cubeb.h"
#include "cubeb-internal.h"

/* This is missing from the MinGW headers. Use a safe fallback. */
#if !defined(MEMORY_ALLOCATION_ALIGNMENT)
#define MEMORY_ALLOCATION_ALIGNMENT 16
#endif

#ifndef STACK_SIZE_PARAM_IS_A_RESERVATION
#define STACK_SIZE_PARAM_IS_A_RESERVATION 0x00010000
#endif

#define CUBEB_STREAM_MAX 32
#define NBUFS 4
#define WINMM_STREAM_MAGIC 0x43554253u /* "CUBS" */
#define WINMM_INPUT_RING_MIN_SAMPLES 0x2000u
#define WINMM_INPUT_SCRATCH_SLOP_FRAMES 0x400u

const GUID KSDATAFORMAT_SUBTYPE_PCM =
{ 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
const GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT =
{ 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

struct cubeb_stream_item {
  SLIST_ENTRY head;
  cubeb_stream * stream;
};

struct winmm_float_ring {
  CRITICAL_SECTION lock;
  float * data;
  uint32_t capacity;
  uint32_t count;
  uint32_t read_pos;
  uint32_t write_pos;
};

static struct cubeb_ops const winmm_ops;

struct cubeb {
  struct cubeb_ops const * ops;
  HANDLE event;
  HANDLE thread;
  int shutdown;
  PSLIST_HEADER work;
  CRITICAL_SECTION lock;
  unsigned int active_streams;
  unsigned int minimum_latency_ms;
};

struct cubeb_stream {
  /* Must match cubeb_stream's prefix in cubeb.c. */
  cubeb * context;
  void * user_ptr;

  cubeb_stream_params params;
  cubeb_data_callback data_callback;
  cubeb_state_callback state_callback;

  WAVEHDR buffers[NBUFS];
  size_t buffer_size;
  int next_buffer;
  int free_buffers;
  int shutdown;
  int draining;
  HANDLE event;
  HWAVEOUT waveout;
  CRITICAL_SECTION lock;
  uint64_t written;
  float soft_volume;
  size_t frame_size;
  DWORD prev_pos_lo_dword;
  DWORD pos_hi_dword;

  cubeb_stream_params input_params;
  HWAVEIN wavein;
  WAVEHDR input_buffers[NBUFS];
  size_t input_buffer_size;
  struct winmm_float_ring * input_ring;
  float * input_convert_buffer;
  float * input_callback_buffer;
  uint32_t output_buffer_frames;
  int input_active;
  uint32_t magic;
  uint32_t input_debug_count;
};

static uint32_t output_debug_count;

/* Debug logging is disabled.  Keep this non-static no-op because the
   reconstructed cubeb.c wrapper and the WinMM backend reference it. */
void
cubeb_winmm_patch_log(char const * fmt, ...)
{
  (void) fmt;
}

/* Source-build counterpart of the injected section's one-time import setup.
   The linked source does not need GetProcAddress, but the scan and diagnostic
   preserve the observable base/delta behavior of the injected implementation. */
static int patch_initialized;

static void
winmm_patch_initialize(void)
{
  uintptr_t base;
  uintptr_t delta;

  if (patch_initialized) {
    return;
  }

  base = (uintptr_t) winmm_patch_initialize & ~(uintptr_t) 0xffff;
  while (base > 0x10000) {
    IMAGE_DOS_HEADER const * dos = (IMAGE_DOS_HEADER const *) base;
    if (dos->e_magic == IMAGE_DOS_SIGNATURE && dos->e_lfanew <= 0xfff &&
        *(DWORD const *) (base + dos->e_lfanew) == IMAGE_NT_SIGNATURE) {
      break;
    }
    base -= 0x10000;
  }
  if (base <= 0x10000) {
    base = 0x62020000;
  }

  delta = base - 0x62020000;
  patch_initialized = 1;
  cubeb_winmm_patch_log(
    "[cubeb_winmm] imports initialized (base=0x%x, delta=0x%x)\n",
    (unsigned int) base, (unsigned int) delta);
}

static size_t
bytes_per_frame(cubeb_stream_params params)
{
  size_t bytes;

  switch (params.format) {
  case CUBEB_SAMPLE_S16LE:
  case CUBEB_SAMPLE_S16BE:
    bytes = sizeof(short);
    break;
  case CUBEB_SAMPLE_FLOAT32LE:
  case CUBEB_SAMPLE_FLOAT32BE:
    bytes = sizeof(float);
    break;
  default:
    XASSERT(0);
    bytes = sizeof(float);
  }

  return bytes * params.channels;
}

static struct winmm_float_ring *
ring_create(uint32_t capacity)
{
  struct winmm_float_ring * ring = calloc(1, sizeof(*ring));
  if (!ring) {
    return NULL;
  }
  ring->data = malloc((size_t) capacity * sizeof(float));
  if (!ring->data) {
    free(ring);
    return NULL;
  }
  ring->capacity = capacity;
  ring->count = 0;
  ring->read_pos = 0;
  ring->write_pos = 0;
  InitializeCriticalSection(&ring->lock);
  return ring;
}

static void
ring_destroy(struct winmm_float_ring * ring)
{
  if (!ring) {
    return;
  }
  DeleteCriticalSection(&ring->lock);
  free(ring->data);
  free(ring);
}

/* Add samples, retaining the newest samples if either the input itself or the
   accumulated contents exceed the ring capacity. */
static void
ring_write(struct winmm_float_ring * ring, float const * input, uint32_t count)
{
  uint32_t i;
  uint32_t overflow;

  EnterCriticalSection(&ring->lock);

  if (count > ring->capacity) {
    input += count - ring->capacity;
    count = ring->capacity;
  }

  if (ring->capacity - ring->count < count) {
    overflow = count - (ring->capacity - ring->count);
    ring->count -= overflow;
    ring->read_pos = (ring->read_pos + overflow) % ring->capacity;
  }

  for (i = 0; i < count; ++i) {
    ring->data[ring->write_pos] = input[i];
    ring->write_pos = (ring->write_pos + 1) % ring->capacity;
  }
  ring->count += count;

  LeaveCriticalSection(&ring->lock);
}

static uint32_t
ring_count(struct winmm_float_ring * ring)
{
  uint32_t count;
  EnterCriticalSection(&ring->lock);
  count = ring->count;
  LeaveCriticalSection(&ring->lock);
  return count;
}

static uint32_t
ring_read(struct winmm_float_ring * ring, float * output, uint32_t count)
{
  uint32_t available;
  uint32_t i;

  EnterCriticalSection(&ring->lock);
  available = count < ring->count ? count : ring->count;
  for (i = 0; i < available; ++i) {
    output[i] = ring->data[(ring->read_pos + i) % ring->capacity];
  }
  ring->read_pos = (ring->read_pos + available) % ring->capacity;
  ring->count -= available;
  LeaveCriticalSection(&ring->lock);
  return available;
}

static WAVEHDR *
winmm_get_next_buffer(cubeb_stream * stm)
{
  WAVEHDR * hdr = &stm->buffers[stm->next_buffer];
  stm->next_buffer = (stm->next_buffer + 1) % NBUFS;
  stm->free_buffers -= 1;
  return hdr;
}

static void
winmm_refill_stream(cubeb_stream * stm)
{
  WAVEHDR * hdr;
  long got;
  long wanted;
  MMRESULT r;
  float * input_buffer = NULL;
  uint32_t input_channels;
  uint32_t available_frames;
  uint32_t read_frames;
  uint32_t samples;
  uint32_t i;
  int peak_milli = 0;
  int signal = 0;

  EnterCriticalSection(&stm->lock);
  stm->free_buffers += 1;

  if (stm->draining) {
    LeaveCriticalSection(&stm->lock);
    if (stm->free_buffers == NBUFS) {
      stm->state_callback(stm, stm->user_ptr, CUBEB_STATE_DRAINED);
    }
    if (stm->event) {
      SetEvent(stm->event);
    }
    return;
  }

  if (stm->shutdown) {
    LeaveCriticalSection(&stm->lock);
    if (stm->event) {
      SetEvent(stm->event);
    }
    return;
  }

  hdr = winmm_get_next_buffer(stm);
  wanted = (long) (stm->buffer_size / bytes_per_frame(stm->params));

  if (stm->input_active && stm->input_ring) {
    input_channels = stm->input_params.channels;
    available_frames = ring_count(stm->input_ring) / input_channels;
    read_frames = available_frames < (uint32_t) wanted ?
                  available_frames : (uint32_t) wanted;
    input_buffer = stm->input_callback_buffer;
    if (read_frames) {
      ring_read(stm->input_ring, input_buffer, read_frames * input_channels);
    }
    if (available_frames < (uint32_t) wanted) {
      memset(input_buffer + read_frames * input_channels, 0,
             ((uint32_t) wanted - read_frames) * input_channels * sizeof(float));
    }
  }

  LeaveCriticalSection(&stm->lock);
  got = stm->data_callback(stm, stm->user_ptr, input_buffer,
                           hdr->lpData, wanted);
  EnterCriticalSection(&stm->lock);

  if (got < 0) {
    cubeb_winmm_patch_log("[cubeb_winmm] data_callback returned error %ld\n", got);
    LeaveCriticalSection(&stm->lock);
    stm->state_callback(stm, stm->user_ptr, CUBEB_STATE_ERROR);
    return;
  }
  if (got < wanted) {
    cubeb_winmm_patch_log("[cubeb_winmm] stream draining: got %ld < wanted %ld\n",
                          got, wanted);
    stm->draining = 1;
  }

  stm->written += got;
  samples = (uint32_t) got * stm->params.channels;
  hdr->dwBufferLength = (DWORD) got * (DWORD) bytes_per_frame(stm->params);

  if (stm->params.format == CUBEB_SAMPLE_S16LE) {
    short * b = (short *) hdr->lpData;
    int peak = 0;
    if (stm->soft_volume != -1.0f) {
      for (i = 0; i < samples; ++i) {
        b[i] = (short) (b[i] * stm->soft_volume);
      }
    }
    for (i = 0; i < samples; ++i) {
      int value = b[i] < 0 ? -(int) b[i] : (int) b[i];
      if (value > peak) {
        peak = value;
      }
    }
    peak_milli = (int) ((float) peak * (1.0f / 32768.0f) * 1000.0f);
    signal = peak_milli > 5;
  } else if (stm->params.format == CUBEB_SAMPLE_FLOAT32LE) {
    float * b = (float *) hdr->lpData;
    float peak = 0.0f;
    if (stm->soft_volume != -1.0f) {
      for (i = 0; i < samples; ++i) {
        b[i] *= stm->soft_volume;
      }
    }
    for (i = 0; i < samples; ++i) {
      float value = b[i] < 0.0f ? -b[i] : b[i];
      if (value > peak) {
        peak = value;
      }
    }
    peak_milli = (int) (peak * 1000.0f);
    signal = peak_milli > 5;
  }

  output_debug_count += 1;
  if ((output_debug_count % 20) == 1 || signal) {
    cubeb_winmm_patch_log(
      "[cubeb_winmm] SPEAKER PLAYBACK SIGNAL: stream=%p, frames=%ld, ch=%u, peak=%d/1000, count=%u\n",
      stm, got, stm->params.channels, peak_milli, output_debug_count);
  }

  r = waveOutWrite(stm->waveout, hdr, sizeof(*hdr));
  LeaveCriticalSection(&stm->lock);
  if (r != MMSYSERR_NOERROR) {
    cubeb_winmm_patch_log("[cubeb_winmm] waveOutWrite failed (r=%u)\n", r);
    stm->state_callback(stm, stm->user_ptr, CUBEB_STATE_ERROR);
  }
}

static void CALLBACK
winmm_buffer_callback(HWAVEOUT waveout, UINT msg, DWORD_PTR user_ptr,
                      DWORD_PTR p1, DWORD_PTR p2)
{
  cubeb_stream * stm = (cubeb_stream *) user_ptr;
  struct cubeb_stream_item * item;
  (void) waveout;
  (void) p1;
  (void) p2;

  if (msg != WOM_DONE || !stm || stm->magic != WINMM_STREAM_MAGIC) {
    return;
  }

  item = _aligned_malloc(sizeof(*item), MEMORY_ALLOCATION_ALIGNMENT);
  if (item) {
    item->stream = stm;
    InterlockedPushEntrySList(stm->context->work, &item->head);
  }
  SetEvent(stm->context->event);
}

static void CALLBACK
winmm_input_callback(HWAVEIN wavein, UINT msg, DWORD_PTR user_ptr,
                     DWORD_PTR p1, DWORD_PTR p2)
{
  cubeb_stream * stm = (cubeb_stream *) user_ptr;
  WAVEHDR * hdr = (WAVEHDR *) p1;
  uint32_t bytes_per_input_frame;
  uint32_t bytes_recorded;
  uint32_t frames;
  uint32_t sample_count;
  uint32_t i;
  int peak = 0;
  MMRESULT r;
  (void) wavein;
  (void) p2;

  if (msg != WIM_DATA || !stm || stm->magic != WINMM_STREAM_MAGIC) {
    return;
  }

  EnterCriticalSection(&stm->lock);
  bytes_per_input_frame =
    ((stm->input_params.format == CUBEB_SAMPLE_FLOAT32LE ||
      stm->input_params.format == CUBEB_SAMPLE_FLOAT32BE) ? 4u : 2u) *
    stm->input_params.channels;
  bytes_recorded = hdr->dwBytesRecorded ? hdr->dwBytesRecorded : hdr->dwBufferLength;

  if (stm->input_active && stm->input_ring &&
      bytes_recorded >= bytes_per_input_frame && stm->input_convert_buffer) {
    short const * source = (short const *) hdr->lpData;
    frames = bytes_recorded / bytes_per_input_frame;
    sample_count = frames * (stm->input_params.channels ?
                             stm->input_params.channels : 1u);
    if (source) {
      for (i = 0; i < sample_count; ++i) {
        int value = source[i];
        int magnitude = value < 0 ? -value : value;
        stm->input_convert_buffer[i] = (float) value * (1.0f / 32768.0f);
        if (magnitude > peak) {
          peak = magnitude;
        }
      }
    } else {
      memset(stm->input_convert_buffer, 0, sample_count * sizeof(float));
    }
    ring_write(stm->input_ring, stm->input_convert_buffer, sample_count);
    stm->input_debug_count += 1;
    if ((stm->input_debug_count % 100) == 1) {
      cubeb_winmm_patch_log(
        "[cubeb_winmm] mic capture: raw_peak=%d/32767, nsamples=%u, count=%u\n",
        peak, sample_count, stm->input_debug_count);
    }
  }
  LeaveCriticalSection(&stm->lock);

  r = waveInAddBuffer(stm->wavein, hdr, sizeof(*hdr));
  if (r != MMSYSERR_NOERROR) {
    cubeb_winmm_patch_log("[cubeb_winmm] input_cb: waveInAddBuffer failed (r=%u)\n", r);
  }
}

static DWORD WINAPI
winmm_buffer_thread(void * user_ptr)
{
  cubeb * ctx = (cubeb *) user_ptr;

  for (;;) {
    PSLIST_ENTRY item;
    if (WaitForSingleObject(ctx->event, INFINITE) != WAIT_OBJECT_0 || ctx->shutdown) {
      break;
    }
    item = InterlockedFlushSList(ctx->work);
    while (item) {
      PSLIST_ENTRY next = item->Next;
      struct cubeb_stream_item * work = (struct cubeb_stream_item *) item;
      if (work->stream && work->stream->magic == WINMM_STREAM_MAGIC) {
        winmm_refill_stream(work->stream);
      }
      _aligned_free(work);
      item = next;
    }
  }
  return 0;
}

static void winmm_destroy(cubeb * ctx);

/*static*/ int
winmm_init(cubeb ** context, char const * context_name)
{
  cubeb * ctx;
  UINT out_devs;
  UINT in_devs;
  (void) context_name;

  winmm_patch_initialize();
  out_devs = waveOutGetNumDevs();
  in_devs = waveInGetNumDevs();
  cubeb_winmm_patch_log(
    "[cubeb_winmm] new_winmm_init called (out_devs=%u, in_devs=%u)\n",
    out_devs, in_devs);
  *context = NULL;

  if (waveOutGetNumDevs() == 0 && waveInGetNumDevs() == 0) {
    cubeb_winmm_patch_log("[cubeb_winmm] no waveOut or waveIn devices\n");
    return CUBEB_ERROR;
  }

  ctx = calloc(1, sizeof(*ctx));
  if (!ctx) {
    return CUBEB_ERROR;
  }
  ctx->ops = &winmm_ops;
  ctx->minimum_latency_ms = 100;
  InitializeCriticalSection(&ctx->lock);

  ctx->work = _aligned_malloc(sizeof(*ctx->work), MEMORY_ALLOCATION_ALIGNMENT);
  if (!ctx->work) {
    DeleteCriticalSection(&ctx->lock);
    free(ctx);
    return CUBEB_ERROR;
  }
  InitializeSListHead(ctx->work);

  ctx->event = CreateEventA(NULL, FALSE, FALSE, NULL);
  if (!ctx->event) {
    _aligned_free(ctx->work);
    DeleteCriticalSection(&ctx->lock);
    free(ctx);
    return CUBEB_ERROR;
  }

  ctx->thread = CreateThread(NULL, 256 * 1024, winmm_buffer_thread, ctx,
                             STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
  if (!ctx->thread) {
    CloseHandle(ctx->event);
    _aligned_free(ctx->work);
    DeleteCriticalSection(&ctx->lock);
    free(ctx);
    return CUBEB_ERROR;
  }
  SetThreadPriority(ctx->thread, THREAD_PRIORITY_TIME_CRITICAL);
  *context = ctx;
  cubeb_winmm_patch_log("[cubeb_winmm] winmm_init completed successfully\n");
  return CUBEB_OK;
}

static char const *
winmm_get_backend_id(cubeb * ctx)
{
  (void) ctx;
  return "winmm";
}

static void
winmm_destroy(cubeb * ctx)
{
  DWORD r;

  XASSERT(ctx->active_streams == 0);
  XASSERT(!InterlockedPopEntrySList(ctx->work));
  DeleteCriticalSection(&ctx->lock);

  if (ctx->thread) {
    ctx->shutdown = 1;
    SetEvent(ctx->event);
    r = WaitForSingleObject(ctx->thread, INFINITE);
    XASSERT(r == WAIT_OBJECT_0);
    CloseHandle(ctx->thread);
  }
  if (ctx->event) {
    CloseHandle(ctx->event);
  }
  _aligned_free(ctx->work);
  free(ctx);
}

static void winmm_stream_destroy_internal(cubeb_stream * stm);

static int
winmm_stream_init(cubeb * context, cubeb_stream ** stream, char const * stream_name,
                  cubeb_devid input_device,
                  cubeb_stream_params * input_stream_params,
                  cubeb_devid output_device,
                  cubeb_stream_params * output_stream_params,
                  unsigned int latency_frames,
                  cubeb_data_callback data_callback,
                  cubeb_state_callback state_callback,
                  void * user_ptr)
{
  cubeb_stream_params synthetic_output_params;
  cubeb_stream_params * out = output_stream_params;
  WAVEFORMATEXTENSIBLE output_wfx;
  WAVEFORMATEX input_wfx;
  cubeb_stream * stm;
  uintptr_t output_devid;
  uintptr_t input_devid;
  MMRESULT r;
  size_t bufsz;
  uint32_t latency_ms;
  uint32_t input_latency_ms;
  uint32_t input_frame_size;
  uint32_t input_buffer_frames;
  uint32_t ring_capacity;
  uint32_t input_channels;
  int specified_output;
  int i;

  cubeb_winmm_patch_log(
    "[cubeb_winmm] new_winmm_stream_init called: name='%s' (in_params=%p, out_params=%p, out_dev=%p, in_dev=%p, lat_frames=%u)\n",
    stream_name ? stream_name : "null", input_stream_params,
    output_stream_params, output_device, input_device, latency_frames);

  if (!context || !stream || (!output_stream_params && !input_stream_params)) {
    cubeb_winmm_patch_log("[cubeb_winmm] invalid parameter in stream_init\n");
    return CUBEB_ERROR_INVALID_PARAMETER;
  }

  if (!out) {
    memset(&synthetic_output_params, 0, sizeof(synthetic_output_params));
    synthetic_output_params.format = CUBEB_SAMPLE_FLOAT32LE;
    synthetic_output_params.rate = input_stream_params->rate;
    synthetic_output_params.channels = 2;
    synthetic_output_params.layout = CUBEB_LAYOUT_STEREO;
    synthetic_output_params.prefs = CUBEB_STREAM_PREF_NONE;
    out = &synthetic_output_params;
  }

  /* This is the literal mask tested by the modified DLL. */
  if (out->prefs & 0x10) {
    cubeb_winmm_patch_log("[cubeb_winmm] loopback not supported\n");
    return CUBEB_ERROR_NOT_SUPPORTED;
  }

  *stream = NULL;
  output_devid = (uintptr_t) output_device - 1;
  input_devid = (uintptr_t) input_device - 1;

  memset(&output_wfx, 0, sizeof(output_wfx));
  output_wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  output_wfx.Format.nChannels = (WORD) out->channels;
  output_wfx.Format.nSamplesPerSec = out->rate;
  output_wfx.Format.cbSize = sizeof(output_wfx) - sizeof(output_wfx.Format);
  if (out->channels == 1) {
    output_wfx.dwChannelMask = SPEAKER_FRONT_CENTER;
  } else if (out->channels == 2) {
    output_wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
  } else {
    output_wfx.dwChannelMask = out->layout;
  }

  switch (out->format) {
  case CUBEB_SAMPLE_S16LE:
    output_wfx.Format.wBitsPerSample = 16;
    output_wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    break;
  case CUBEB_SAMPLE_FLOAT32LE:
    output_wfx.Format.wBitsPerSample = 32;
    output_wfx.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    break;
  default:
    cubeb_winmm_patch_log("[cubeb_winmm] invalid format for output: %d\n", out->format);
    return CUBEB_ERROR_INVALID_FORMAT;
  }
  output_wfx.Format.nBlockAlign =
    (output_wfx.Format.wBitsPerSample * output_wfx.Format.nChannels) / 8;
  output_wfx.Format.nAvgBytesPerSec =
    output_wfx.Format.nSamplesPerSec * output_wfx.Format.nBlockAlign;
  output_wfx.Samples.wValidBitsPerSample = output_wfx.Format.wBitsPerSample;

  EnterCriticalSection(&context->lock);
  if (context->active_streams >= CUBEB_STREAM_MAX) {
    LeaveCriticalSection(&context->lock);
    cubeb_winmm_patch_log("[cubeb_winmm] too many active streams (%u)\n",
                          context->active_streams);
    return CUBEB_ERROR_NOT_SUPPORTED;
  }
  context->active_streams += 1;
  LeaveCriticalSection(&context->lock);

  stm = calloc(1, sizeof(*stm));
  if (!stm) {
    EnterCriticalSection(&context->lock);
    context->active_streams -= 1;
    LeaveCriticalSection(&context->lock);
    return CUBEB_ERROR;
  }

  stm->magic = WINMM_STREAM_MAGIC;
  stm->context = context;
  stm->user_ptr = user_ptr;
  stm->params = *out;
  stm->data_callback = data_callback;
  stm->state_callback = state_callback;
  stm->written = 0;
  stm->soft_volume = -1.0f;
  stm->frame_size = bytes_per_frame(stm->params);
  stm->prev_pos_lo_dword = 0;
  stm->pos_hi_dword = 0;

  latency_ms = latency_frames * 1000 / out->rate;
  if (latency_ms < context->minimum_latency_ms) {
    latency_ms = context->minimum_latency_ms;
  }
  bufsz = (size_t) (out->rate / 1000.0 * latency_ms *
                    bytes_per_frame(*out) / NBUFS);
  if (bufsz % bytes_per_frame(*out)) {
    bufsz += bytes_per_frame(*out) - bufsz % bytes_per_frame(*out);
  }
  stm->buffer_size = bufsz;

  InitializeCriticalSection(&stm->lock);
  stm->event = CreateEventA(NULL, FALSE, FALSE, NULL);
  if (!stm->event) {
    winmm_stream_destroy_internal(stm);
    return CUBEB_ERROR;
  }

  specified_output = output_devid != (uintptr_t) WAVE_MAPPER;
  r = waveOutOpen(&stm->waveout, (UINT) output_devid, &output_wfx.Format,
                  (DWORD_PTR) winmm_buffer_callback, (DWORD_PTR) stm,
                  CALLBACK_FUNCTION);
  if (r != MMSYSERR_NOERROR && specified_output) {
    cubeb_winmm_patch_log(
      "[cubeb_winmm] waveOutOpen failed with dev %u (r=%u), trying WAVE_MAPPER\n",
      (UINT) output_devid, r);
    r = waveOutOpen(&stm->waveout, WAVE_MAPPER, &output_wfx.Format,
                    (DWORD_PTR) winmm_buffer_callback, (DWORD_PTR) stm,
                    CALLBACK_FUNCTION);
  }
  if (r != MMSYSERR_NOERROR && out->channels <= 2) {
    cubeb_winmm_patch_log(
      "[cubeb_winmm] waveOutOpen extensible failed (r=%u), trying legacy tags\n", r);
    output_wfx.Format.cbSize = 0;
    output_wfx.Format.wFormatTag =
      out->format == CUBEB_SAMPLE_FLOAT32LE ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
    r = waveOutOpen(&stm->waveout, (UINT) output_devid, &output_wfx.Format,
                    (DWORD_PTR) winmm_buffer_callback, (DWORD_PTR) stm,
                    CALLBACK_FUNCTION);
    if (r != MMSYSERR_NOERROR && specified_output) {
      r = waveOutOpen(&stm->waveout, WAVE_MAPPER, &output_wfx.Format,
                      (DWORD_PTR) winmm_buffer_callback, (DWORD_PTR) stm,
                      CALLBACK_FUNCTION);
    }
  }
  if (r != MMSYSERR_NOERROR) {
    cubeb_winmm_patch_log("[cubeb_winmm] waveOutOpen failed completely (r=%u)\n", r);
    winmm_stream_destroy_internal(stm);
    return CUBEB_ERROR;
  }

  r = waveOutPause(stm->waveout);
  if (r != MMSYSERR_NOERROR) {
    cubeb_winmm_patch_log("[cubeb_winmm] waveOutPause failed (r=%u)\n", r);
    winmm_stream_destroy_internal(stm);
    return CUBEB_ERROR;
  }

  if (input_stream_params) {
    input_channels = input_stream_params->channels == 2 ? 2 : 1;
    cubeb_winmm_patch_log(
      "[cubeb_winmm] setting up input stream capture (rate=%u, req_ch=%u, clamped_ch=%u, fmt=%u)\n",
      input_stream_params->rate, input_stream_params->channels,
      input_channels, input_stream_params->format);

    memset(&input_wfx, 0, sizeof(input_wfx));
    input_wfx.wFormatTag = WAVE_FORMAT_PCM;
    input_wfx.nChannels = (WORD) input_channels;
    input_wfx.nSamplesPerSec = input_stream_params->rate;
    input_wfx.wBitsPerSample = 16;
    input_wfx.nBlockAlign = (WORD) (input_channels * sizeof(short));
    input_wfx.nAvgBytesPerSec = input_wfx.nSamplesPerSec * input_wfx.nBlockAlign;
    input_wfx.cbSize = 0;

    if (input_stream_params->format != CUBEB_SAMPLE_S16LE &&
        input_stream_params->format != CUBEB_SAMPLE_FLOAT32LE) {
      cubeb_winmm_patch_log("[cubeb_winmm] invalid format for input: %d\n",
                            input_stream_params->format);
      winmm_stream_destroy_internal(stm);
      return CUBEB_ERROR_INVALID_FORMAT;
    }

    stm->input_params = *input_stream_params;
    stm->input_params.format = CUBEB_SAMPLE_S16LE;
    stm->input_params.channels = input_channels;

    input_frame_size = input_channels * sizeof(short);
    input_latency_ms = latency_frames * 1000 / input_stream_params->rate;
    if (input_latency_ms < context->minimum_latency_ms) {
      input_latency_ms = context->minimum_latency_ms;
    }
    stm->input_buffer_size = (size_t)
      (input_stream_params->rate / 1000.0 * input_latency_ms *
       input_frame_size / NBUFS);
    if (stm->input_buffer_size % input_frame_size) {
      stm->input_buffer_size += input_frame_size -
                                stm->input_buffer_size % input_frame_size;
    }
    input_buffer_frames = (uint32_t) (stm->input_buffer_size / input_frame_size);
    stm->output_buffer_frames =
      (uint32_t) (stm->buffer_size / bytes_per_frame(stm->params));

    ring_capacity = input_stream_params->rate * input_channels / 2;
    if (ring_capacity < WINMM_INPUT_RING_MIN_SAMPLES) {
      ring_capacity = WINMM_INPUT_RING_MIN_SAMPLES;
    }
    stm->input_ring = ring_create(ring_capacity);
    stm->input_convert_buffer = malloc((size_t) input_channels *
      (input_buffer_frames + WINMM_INPUT_SCRATCH_SLOP_FRAMES) * 16);
    stm->input_callback_buffer = malloc((size_t) input_channels *
      (stm->output_buffer_frames + WINMM_INPUT_SCRATCH_SLOP_FRAMES) * 16);
    if (!stm->input_ring || !stm->input_convert_buffer ||
        !stm->input_callback_buffer) {
      cubeb_winmm_patch_log("[cubeb_winmm] failed to allocate ring/scratch buffers\n");
      winmm_stream_destroy_internal(stm);
      return CUBEB_ERROR;
    }

    r = waveInOpen(&stm->wavein, (UINT) input_devid, &input_wfx,
                   (DWORD_PTR) winmm_input_callback, (DWORD_PTR) stm,
                   CALLBACK_FUNCTION);
    if (r != MMSYSERR_NOERROR && input_devid != (uintptr_t) WAVE_MAPPER) {
      cubeb_winmm_patch_log(
        "[cubeb_winmm] waveInOpen failed with dev %u (r=%u), trying WAVE_MAPPER\n",
        (UINT) input_devid, r);
      input_devid = (uintptr_t) WAVE_MAPPER;
      r = waveInOpen(&stm->wavein, WAVE_MAPPER, &input_wfx,
                     (DWORD_PTR) winmm_input_callback, (DWORD_PTR) stm,
                     CALLBACK_FUNCTION);
    }
    if (r != MMSYSERR_NOERROR) {
      cubeb_winmm_patch_log(
        "[cubeb_winmm] waveInOpen failed (r=%u); degrading to output-only\n", r);
      stm->wavein = NULL;
    } else {
      for (i = 0; i < NBUFS; ++i) {
        WAVEHDR * hdr = &stm->input_buffers[i];
        hdr->lpData = calloc(1, stm->input_buffer_size);
        hdr->dwBufferLength = (DWORD) stm->input_buffer_size;
        hdr->dwBytesRecorded = 0;
        memset(&hdr->dwUser, 0, sizeof(*hdr) - offsetof(WAVEHDR, dwUser));
        if (!hdr->lpData ||
            waveInPrepareHeader(stm->wavein, hdr, sizeof(*hdr)) != MMSYSERR_NOERROR ||
            waveInAddBuffer(stm->wavein, hdr, sizeof(*hdr)) != MMSYSERR_NOERROR) {
          cubeb_winmm_patch_log(
            "[cubeb_winmm] waveInPrepareHeader/AddBuffer failed on buf %d\n", i);
          winmm_stream_destroy_internal(stm);
          return CUBEB_ERROR;
        }
      }
      stm->input_active = 1;
      cubeb_winmm_patch_log(
        "[cubeb_winmm] waveInOpen succeeded, input_active = 1, dev=%u, rate=%u, ch=%u\n",
        (UINT) input_devid, stm->input_params.rate, stm->input_params.channels);
    }
  }

  for (i = 0; i < NBUFS; ++i) {
    WAVEHDR * hdr = &stm->buffers[i];
    hdr->lpData = calloc(1, stm->buffer_size);
    hdr->dwBufferLength = (DWORD) stm->buffer_size;
    hdr->dwFlags = 0;
    if (!hdr->lpData ||
        waveOutPrepareHeader(stm->waveout, hdr, sizeof(*hdr)) != MMSYSERR_NOERROR) {
      cubeb_winmm_patch_log(
        "[cubeb_winmm] waveOutPrepareHeader failed on buf %d\n", i);
      winmm_stream_destroy_internal(stm);
      return CUBEB_ERROR;
    }
    winmm_refill_stream(stm);
  }

  *stream = stm;
  cubeb_winmm_patch_log("[cubeb_winmm] stream_init succeeded (%p)\n", stm);
  return CUBEB_OK;
}

static void
winmm_stream_destroy_internal(cubeb_stream * stm)
{
  int i;
  int retries;

  if (stm->waveout) {
    EnterCriticalSection(&stm->lock);
    stm->shutdown = 1;
    waveOutReset(stm->waveout);
    retries = 10;
    while (stm->free_buffers <= 3 && retries-- > 0) {
      LeaveCriticalSection(&stm->lock);
      if (stm->event) {
        WaitForSingleObject(stm->event, 50);
      }
      EnterCriticalSection(&stm->lock);
    }
    for (i = 0; i < NBUFS; ++i) {
      if (stm->buffers[i].dwFlags & WHDR_PREPARED) {
        waveOutUnprepareHeader(stm->waveout, &stm->buffers[i],
                               sizeof(stm->buffers[i]));
      }
    }
    waveOutClose(stm->waveout);
    LeaveCriticalSection(&stm->lock);
  }

  /* Invalidate the stream only after the waveOut reset completions have been
     accounted for.  winmm_buffer_callback() and winmm_buffer_thread() both drop
     work while magic is invalid, so clearing it before waveOutReset() stops the
     completions from ever being counted and the wait loop above burns its full
     10 x 50 ms budget on every stream destruction. */
  stm->magic = 0;

  if (stm->wavein) {
    EnterCriticalSection(&stm->lock);
    stm->input_active = 0;
    LeaveCriticalSection(&stm->lock);
    waveInStop(stm->wavein);
    waveInReset(stm->wavein);
    EnterCriticalSection(&stm->lock);
    for (i = 0; i < NBUFS; ++i) {
      if (stm->input_buffers[i].dwFlags & WHDR_PREPARED) {
        waveInUnprepareHeader(stm->wavein, &stm->input_buffers[i],
                              sizeof(stm->input_buffers[i]));
      }
    }
    waveInClose(stm->wavein);
    ring_destroy(stm->input_ring);
    stm->input_ring = NULL;
    free(stm->input_convert_buffer);
    stm->input_convert_buffer = NULL;
    free(stm->input_callback_buffer);
    stm->input_callback_buffer = NULL;
    LeaveCriticalSection(&stm->lock);
    stm->wavein = NULL;
  } else {
    EnterCriticalSection(&stm->lock);
    stm->input_active = 0;
    ring_destroy(stm->input_ring);
    stm->input_ring = NULL;
    free(stm->input_convert_buffer);
    stm->input_convert_buffer = NULL;
    free(stm->input_callback_buffer);
    stm->input_callback_buffer = NULL;
    LeaveCriticalSection(&stm->lock);
  }

  /* The injected implementation frees the output blocks here but, notably,
     does not free input_buffers[i].lpData. */
  for (i = 0; i < NBUFS; ++i) {
    free(stm->buffers[i].lpData);
    stm->buffers[i].lpData = NULL;
  }
  if (stm->event) {
    CloseHandle(stm->event);
  }
  DeleteCriticalSection(&stm->lock);

  if (stm->context) {
    EnterCriticalSection(&stm->context->lock);
    if (stm->context->active_streams) {
      stm->context->active_streams -= 1;
    }
    LeaveCriticalSection(&stm->context->lock);
  }
  free(stm);
}

static void
winmm_stream_destroy(cubeb_stream * stm)
{
  cubeb_winmm_patch_log("[cubeb_winmm] new_winmm_stream_destroy called (%p)\n", stm);
  if (!stm || stm->magic != WINMM_STREAM_MAGIC) {
    return;
  }
  winmm_stream_destroy_internal(stm);
}

static int
winmm_get_max_channel_count(cubeb * ctx, uint32_t * max_channels)
{
  XASSERT(ctx && max_channels);
  *max_channels = 2;
  return CUBEB_OK;
}

static int
winmm_get_min_latency(cubeb * ctx, cubeb_stream_params params, uint32_t * latency)
{
  *latency = ctx->minimum_latency_ms * params.rate / 1000;
  return CUBEB_OK;
}

static int
winmm_get_preferred_sample_rate(cubeb * ctx, uint32_t * rate)
{
  WAVEOUTCAPS woc;
  MMRESULT r;
  (void) ctx;

  r = waveOutGetDevCaps(WAVE_MAPPER, &woc, sizeof(woc));
  if (r != MMSYSERR_NOERROR) {
    return CUBEB_ERROR;
  }
  if (!(woc.dwFormats & WAVE_FORMAT_4S16) &&
      (woc.dwFormats & WAVE_FORMAT_48S16)) {
    *rate = 48000;
    return CUBEB_OK;
  }
  *rate = 44100;
  return CUBEB_OK;
}

static int
winmm_stream_start(cubeb_stream * stm)
{
  MMRESULT r;

  if (!stm) {
    cubeb_winmm_patch_log(
      "[cubeb_winmm] new_winmm_stream_start called (%p, wavein=%p)\n",
      NULL, NULL);
    return CUBEB_ERROR;
  }
  cubeb_winmm_patch_log(
    "[cubeb_winmm] new_winmm_stream_start called (%p, wavein=%p)\n",
    stm, stm->wavein);
  if (stm->magic != WINMM_STREAM_MAGIC) {
    return CUBEB_ERROR;
  }

  EnterCriticalSection(&stm->lock);
  r = waveOutRestart(stm->waveout);
  if (r == MMSYSERR_NOERROR && stm->wavein) {
    if (stm->input_ring && stm->input_callback_buffer) {
      uint32_t channels = stm->input_params.channels ? stm->input_params.channels : 1;
      uint32_t frames = stm->output_buffer_frames ? stm->output_buffer_frames : 441;
      uint32_t samples = channels * frames;
      memset(stm->input_callback_buffer, 0, samples * sizeof(float));
      ring_write(stm->input_ring, stm->input_callback_buffer, samples);
    }
    r = waveInStart(stm->wavein);
  }
  LeaveCriticalSection(&stm->lock);

  if (r != MMSYSERR_NOERROR) {
    cubeb_winmm_patch_log("[cubeb_winmm] stream_start failed (r=%u)\n", r);
    return CUBEB_ERROR;
  }
  stm->state_callback(stm, stm->user_ptr, CUBEB_STATE_STARTED);
  cubeb_winmm_patch_log("[cubeb_winmm] stream_start succeeded\n");
  return CUBEB_OK;
}

static int
winmm_stream_stop(cubeb_stream * stm)
{
  MMRESULT r;

  cubeb_winmm_patch_log("[cubeb_winmm] new_winmm_stream_stop called (%p)\n", stm);
  if (!stm || stm->magic != WINMM_STREAM_MAGIC) {
    return CUBEB_OK;
  }

  EnterCriticalSection(&stm->lock);
  r = waveOutPause(stm->waveout);
  if (r != MMSYSERR_NOERROR) {
    LeaveCriticalSection(&stm->lock);
    cubeb_winmm_patch_log("[cubeb_winmm] stream_stop failed (r=%u)\n", r);
    return CUBEB_ERROR;
  }
  if (stm->wavein) {
    waveInStop(stm->wavein);
  }
  LeaveCriticalSection(&stm->lock);
  stm->state_callback(stm, stm->user_ptr, CUBEB_STATE_STOPPED);
  cubeb_winmm_patch_log("[cubeb_winmm] stream_stop succeeded\n");
  return CUBEB_OK;
}

static size_t
winmm_output_frame_size(cubeb_stream * stm)
{
  size_t frame_size = stm->frame_size;
  if (!frame_size) {
    if (stm->params.format <= CUBEB_SAMPLE_S16BE) {
      frame_size = stm->params.channels * 2;
    } else if (stm->params.format <= CUBEB_SAMPLE_FLOAT32BE) {
      frame_size = stm->params.channels * 4;
    }
    if (!frame_size) {
      frame_size = 4;
    }
  }
  return frame_size;
}

static uint64_t
winmm_position_bytes(cubeb_stream * stm, DWORD low)
{
  if (low < stm->prev_pos_lo_dword) {
    stm->pos_hi_dword += 1;
  }
  stm->prev_pos_lo_dword = low;
  return ((uint64_t) stm->pos_hi_dword << 32) | low;
}

static int
winmm_stream_get_position(cubeb_stream * stm, uint64_t * position)
{
  MMTIME time;
  MMRESULT r;

  if (!stm || !position || stm->magic != WINMM_STREAM_MAGIC) {
    return CUBEB_ERROR_INVALID_PARAMETER;
  }
  EnterCriticalSection(&stm->lock);
  time.wType = TIME_BYTES;
  r = waveOutGetPosition(stm->waveout, &time, sizeof(time));
  if (r != MMSYSERR_NOERROR || time.wType != TIME_BYTES) {
    LeaveCriticalSection(&stm->lock);
    return CUBEB_ERROR;
  }
  *position = winmm_position_bytes(stm, time.u.cb) /
              winmm_output_frame_size(stm);
  LeaveCriticalSection(&stm->lock);
  return CUBEB_OK;
}

static int
winmm_stream_get_latency(cubeb_stream * stm, uint32_t * latency)
{
  MMTIME time;
  MMRESULT r;
  uint64_t position;
  uint64_t written;

  if (!stm || !latency || stm->magic != WINMM_STREAM_MAGIC) {
    return CUBEB_ERROR_INVALID_PARAMETER;
  }
  EnterCriticalSection(&stm->lock);
  time.wType = TIME_BYTES;
  r = waveOutGetPosition(stm->waveout, &time, sizeof(time));
  if (r != MMSYSERR_NOERROR || time.wType != TIME_BYTES) {
    LeaveCriticalSection(&stm->lock);
    return CUBEB_ERROR;
  }
  position = winmm_position_bytes(stm, time.u.cb) /
             winmm_output_frame_size(stm);
  written = stm->written;
  *latency = written >= position ? (uint32_t) (written - position) : 0;
  LeaveCriticalSection(&stm->lock);
  return CUBEB_OK;
}

static int
winmm_stream_get_input_latency(cubeb_stream * stm, uint32_t * latency)
{
  uint32_t samples;

  if (!stm || !latency || !stm->input_ring || !stm->input_active ||
      stm->magic != WINMM_STREAM_MAGIC || !stm->input_params.channels) {
    return CUBEB_ERROR_NOT_SUPPORTED;
  }
  samples = ring_count(stm->input_ring);
  *latency = samples / stm->input_params.channels;
  return CUBEB_OK;
}

static int
winmm_stream_set_volume(cubeb_stream * stm, float volume)
{
  if (!stm || volume < 0.0f || volume > 1.0f ||
      stm->magic != WINMM_STREAM_MAGIC) {
    return CUBEB_ERROR_INVALID_PARAMETER;
  }
  EnterCriticalSection(&stm->lock);
  stm->soft_volume = volume;
  LeaveCriticalSection(&stm->lock);
  return CUBEB_OK;
}

static char *
patch_device_id(UINT devid)
{
  char * id = malloc(16);
  if (id) {
    id[0] = (char) ('0' + (devid % 10));
    id[1] = '\0';
  }
  return id;
}

static char *
patch_strdup(char const * source)
{
  size_t length = strlen(source) + 1;
  char * result = malloc(length);
  if (result) {
    memcpy(result, source, length);
  }
  return result;
}

static void
patch_fill_common_device_info(cubeb_device_info * info, UINT index,
                              char const * name, cubeb_device_type type,
                              uint32_t channels)
{
  info->devid = (cubeb_devid) (uintptr_t) (index + 1);
  info->device_id = patch_device_id(index);
  info->friendly_name = patch_strdup(name);
  info->group_id = NULL;
  info->vendor_name = NULL;
  info->type = type;
  info->state = CUBEB_DEVICE_STATE_ENABLED;
  info->preferred = index == 0 ? CUBEB_DEVICE_PREF_ALL : CUBEB_DEVICE_PREF_NONE;
  info->format = CUBEB_DEVICE_FMT_S16LE | CUBEB_DEVICE_FMT_F32LE;
  info->default_format = CUBEB_DEVICE_FMT_F32LE;
  info->max_channels = channels;
  info->default_rate = 44100;
  info->max_rate = 48000;
  info->min_rate = 8000;
  info->latency_lo = 441;
  info->latency_hi = 4410;
}

static int
winmm_enumerate_devices(cubeb * context, cubeb_device_type type,
                        cubeb_device_collection * collection)
{
  UINT outcount = waveOutGetNumDevs();
  UINT incount = waveInGetNumDevs();
  UINT total = outcount + incount;
  UINT i;
  cubeb_device_info * devices;
  (void) context;

  devices = calloc(total ? total : 1, sizeof(*devices));
  if (!devices) {
    return CUBEB_ERROR;
  }
  collection->count = 0;

  if (type & CUBEB_DEVICE_TYPE_OUTPUT) {
    for (i = 0; i < outcount; ++i) {
      WAVEOUTCAPSA caps;
      MMRESULT r;
      cubeb_device_info * info;
      memset(&caps, 0, sizeof(caps));
      r = waveOutGetDevCapsA(i, &caps, sizeof(caps));
      if (r == MMSYSERR_NOERROR) {
        info = &devices[collection->count];
        patch_fill_common_device_info(info, i, caps.szPname,
                                      CUBEB_DEVICE_TYPE_OUTPUT,
                                      caps.wChannels == 1 ? 1 : 2);
        /* For successful nonzero devices the DLL stores r (zero). */
        info->preferred = i == 0 ? CUBEB_DEVICE_PREF_ALL : (cubeb_device_pref) r;
        collection->count += 1;
        cubeb_winmm_patch_log(
          "[cubeb_winmm] enumerated out device: idx=%u, devid=%p, name=%s\n",
          i, info->devid, info->friendly_name);
      }
    }
  }

  if (type & CUBEB_DEVICE_TYPE_INPUT) {
    for (i = 0; i < incount; ++i) {
      WAVEINCAPSA caps;
      MMRESULT r;
      cubeb_device_info * info;
      memset(&caps, 0, sizeof(caps));
      r = waveInGetDevCapsA(i, &caps, sizeof(caps));
      if (r == MMSYSERR_NOERROR) {
        info = &devices[collection->count];
        patch_fill_common_device_info(info, i, caps.szPname,
                                      CUBEB_DEVICE_TYPE_INPUT,
                                      caps.wChannels == 2 ? 2 : 1);
        info->preferred = i == 0 ? CUBEB_DEVICE_PREF_ALL : (cubeb_device_pref) r;
        collection->count += 1;
        cubeb_winmm_patch_log(
          "[cubeb_winmm] enumerated in device: idx=%u, devid=%p, ch=%u, name=%s\n",
          i, info->devid, info->max_channels, info->friendly_name);
      }
    }
  }

  collection->device = devices;
  cubeb_winmm_patch_log("[cubeb_winmm] enumerate_devices total count: %u\n",
                        collection->count);
  return CUBEB_OK;
}

static int
winmm_device_collection_destroy(cubeb * context,
                                 cubeb_device_collection * collection)
{
  size_t i;
  (void) context;
  if (!collection) {
    return CUBEB_OK;
  }
  for (i = 0; i < collection->count; ++i) {
    free((void *) collection->device[i].device_id);
    free((void *) collection->device[i].friendly_name);
    free((void *) collection->device[i].group_id);
    free((void *) collection->device[i].vendor_name);
  }
  free(collection->device);
  return CUBEB_OK;
}

static struct cubeb_ops const winmm_ops = {
  /*.init =*/ winmm_init,
  /*.get_backend_id =*/ winmm_get_backend_id,
  /*.get_max_channel_count=*/ winmm_get_max_channel_count,
  /*.get_min_latency=*/ winmm_get_min_latency,
  /*.get_preferred_sample_rate =*/ winmm_get_preferred_sample_rate,
  /*.enumerate_devices =*/ winmm_enumerate_devices,
  /*.device_collection_destroy =*/ winmm_device_collection_destroy,
  /*.destroy =*/ winmm_destroy,
  /*.stream_init =*/ winmm_stream_init,
  /*.stream_destroy =*/ winmm_stream_destroy,
  /*.stream_start =*/ winmm_stream_start,
  /*.stream_stop =*/ winmm_stream_stop,
  /*.stream_reset_default_device =*/ NULL,
  /*.stream_get_position =*/ winmm_stream_get_position,
  /*.stream_get_latency = */ winmm_stream_get_latency,
  /*.stream_get_input_latency = */ winmm_stream_get_input_latency,
  /*.stream_set_volume =*/ winmm_stream_set_volume,
  /*.stream_get_current_device =*/ NULL,
  /*.stream_device_destroy =*/ NULL,
  /*.stream_register_device_changed_callback=*/ NULL,
  /*.register_device_collection_changed =*/ NULL
};
