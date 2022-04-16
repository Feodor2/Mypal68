/*
 * Copyright © 2018, VideoLAN and dav1d authors
 * Copyright © 2018, Two Orioles, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DAV1D_SRC_THREAD_H
#define DAV1D_SRC_THREAD_H

#if defined(__GNUC__) || defined(__clang__)
#    define dav_unused __attribute__((unused))
#else
#    define dav_unused
#endif


#if defined(_WIN32)

#include <windows.h>
#include <malloc.h>

#define PTHREAD_ONCE_INIT INIT_ONCE_STATIC_INIT

typedef struct {
    HANDLE h;
    void *(*func)(void*);
    void *arg;
} pthread_t;

typedef struct {
    unsigned stack_size;
} pthread_attr_t;

typedef CRITICAL_SECTION pthread_mutex_t;
typedef struct pthread_cond_t {
    void *Ptr;
} pthread_cond_t;
typedef union pthread_once_t  {
    void * Ptr;    ///< For the Windows 6.0+ native functions
    LONG state;    ///< For the pre-Windows 6.0 compat code
} pthread_once_t;

int dav1d_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                         void *(*func)(void*), void *arg);
int dav1d_pthread_join(pthread_t *thread, void **res);
int dav1d_pthread_once(pthread_once_t *once_control,
                       void (*init_routine)(void));

#define pthread_create dav1d_pthread_create
#define pthread_join(thread, res) dav1d_pthread_join(&(thread), res)
#define pthread_once   dav1d_pthread_once

static inline int pthread_attr_init(pthread_attr_t *const attr) {
    attr->stack_size = 0;
    return 0;
}

static inline int pthread_attr_destroy(pthread_attr_t *const attr) {
    return 0;
}

static inline int pthread_attr_setstacksize(pthread_attr_t *const attr,
                                            const unsigned stack_size)
{
    attr->stack_size = stack_size;
    return 0;
}

static inline int pthread_mutex_init(pthread_mutex_t *const mutex,
                                     const void *const attr)
{
    InitializeCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_destroy(pthread_mutex_t *const mutex) {
    DeleteCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_lock(pthread_mutex_t *const mutex) {
    EnterCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_unlock(pthread_mutex_t *const mutex) {
    LeaveCriticalSection(mutex);
    return 0;
}

typedef struct  win32_cond_t {
    pthread_mutex_t mtx_broadcast;
    pthread_mutex_t mtx_waiter_count;
    volatile int waiter_count;
    HANDLE semaphore;
    HANDLE waiters_done;
    volatile int is_broadcast;
} win32_cond_t;

static inline int pthread_cond_init(pthread_cond_t *const cond,
                                    const void *const attr)
{

    win32_cond_t *win32_cond = NULL;

    win32_cond = _aligned_malloc(sizeof(win32_cond_t), 16);
    if (win32_cond)
        memset(win32_cond, 0, sizeof(win32_cond_t));
    else
        return ENOMEM;
    cond->Ptr = win32_cond;
    win32_cond->semaphore = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
    if (!win32_cond->semaphore)
        return ENOMEM;
    win32_cond->waiters_done = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!win32_cond->waiters_done)
        return ENOMEM;

    pthread_mutex_init(&win32_cond->mtx_waiter_count, NULL);
    pthread_mutex_init(&win32_cond->mtx_broadcast, NULL);
    return 0;
}

static inline int pthread_cond_destroy(pthread_cond_t *const cond) {
    win32_cond_t *win32_cond = (win32_cond_t*)cond->Ptr;

    /* non native condition variables */
    CloseHandle(win32_cond->semaphore);
    CloseHandle(win32_cond->waiters_done);
    pthread_mutex_destroy(&win32_cond->mtx_waiter_count);
    pthread_mutex_destroy(&win32_cond->mtx_broadcast);

    void *val;

    memcpy(&val, &win32_cond, sizeof(val));
    memcpy(&win32_cond, &(void *){ NULL }, sizeof(val));
    _aligned_free(val);

    cond->Ptr = NULL;
    return 0;
}

static inline int pthread_cond_wait(pthread_cond_t *const cond,
                                    pthread_mutex_t *const mutex)
{
    win32_cond_t *win32_cond = (win32_cond_t*)cond->Ptr;
    int last_waiter;

    /* non native condition variables */
    pthread_mutex_lock(&win32_cond->mtx_broadcast);
    pthread_mutex_lock(&win32_cond->mtx_waiter_count);
    win32_cond->waiter_count++;
    pthread_mutex_unlock(&win32_cond->mtx_waiter_count);
    pthread_mutex_unlock(&win32_cond->mtx_broadcast);

    // unlock the external mutex
    pthread_mutex_unlock(mutex);
    WaitForSingleObject(win32_cond->semaphore, INFINITE);

    pthread_mutex_lock(&win32_cond->mtx_waiter_count);
    win32_cond->waiter_count--;
    last_waiter = !win32_cond->waiter_count || !win32_cond->is_broadcast;
    pthread_mutex_unlock(&win32_cond->mtx_waiter_count);

    if (last_waiter)
        SetEvent(win32_cond->waiters_done);

    // lock the external mutex
    return pthread_mutex_lock(mutex);
}

static inline int pthread_cond_signal(pthread_cond_t *const cond) {
    win32_cond_t *win32_cond = (win32_cond_t*)cond->Ptr;
    int have_waiter;

    pthread_mutex_lock(&win32_cond->mtx_broadcast);

    /* non-native condition variables */
    pthread_mutex_lock(&win32_cond->mtx_waiter_count);
    have_waiter = win32_cond->waiter_count;
    pthread_mutex_unlock(&win32_cond->mtx_waiter_count);

    if (have_waiter) {
        ReleaseSemaphore(win32_cond->semaphore, 1, NULL);
        WaitForSingleObject(win32_cond->waiters_done, INFINITE);
        ResetEvent(win32_cond->waiters_done);
    }

    pthread_mutex_unlock(&win32_cond->mtx_broadcast);
    return 0;
}

static inline int pthread_cond_broadcast(pthread_cond_t *const cond) {
    win32_cond_t *win32_cond = (win32_cond_t*)cond->Ptr;
    int have_waiter;

    /* non native condition variables */
    pthread_mutex_lock(&win32_cond->mtx_broadcast);
    pthread_mutex_lock(&win32_cond->mtx_waiter_count);
    have_waiter = 0;

    if (win32_cond->waiter_count) {
        win32_cond->is_broadcast = 1;
        have_waiter = 1;
    }

    if (have_waiter) {
        ReleaseSemaphore(win32_cond->semaphore, win32_cond->waiter_count, NULL);
        pthread_mutex_unlock(&win32_cond->mtx_waiter_count);
        WaitForSingleObject(win32_cond->waiters_done, INFINITE);
        ResetEvent(win32_cond->waiters_done);
        win32_cond->is_broadcast = 0;
    } else
        pthread_mutex_unlock(&win32_cond->mtx_waiter_count);
    pthread_mutex_unlock(&win32_cond->mtx_broadcast);
    return 0;
}

#else

#include <pthread.h>

#endif

#endif /* DAV1D_SRC_THREAD_H */
