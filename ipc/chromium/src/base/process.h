// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PROCESS_H_
#define BASE_PROCESS_H_

#include "base/basictypes.h"

#include <sys/types.h>
#ifdef OS_WIN
#  include <windows.h>
#endif

namespace base {

// ProcessHandle is a platform specific type which represents the underlying OS
// handle to a process.
// ProcessId is a number which identifies the process in the OS.
#if defined(OS_WIN)
typedef HANDLE ProcessHandle;
typedef DWORD ProcessId;
#elif defined(OS_POSIX)
// On POSIX, our ProcessHandle will just be the PID.
typedef pid_t ProcessHandle;
typedef pid_t ProcessId;
#endif

}  // namespace base

#endif  // BASE_PROCESS_H_
