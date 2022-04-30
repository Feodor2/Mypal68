#ifndef InitOnceExecOnceXP_h

#include <windows.h>
#include <ntstatus.h>
#include <winternl.h>

extern "C" DECLSPEC_IMPORT NTSTATUS WINAPI NtWaitForKeyedEvent(HANDLE, const void*, BOOLEAN, const LARGE_INTEGER*);
extern "C" DECLSPEC_IMPORT NTSTATUS WINAPI NtReleaseKeyedEvent(HANDLE, const void*, BOOLEAN, const LARGE_INTEGER*);

class WinxpStuff final {
 public:
  static DWORD InitOnceExecOnceXP(INIT_ONCE* once, PINIT_ONCE_FN func, void* param, void ** context) {
    DWORD ret = 0;
    for (;;) {
for_begin:
      ULONG_PTR next, val = (ULONG_PTR)&once->Ptr;

      switch (val & 3) {
        case 0:  /* first time */
          if (!::InterlockedCompareExchangePointer(&once->Ptr, (void *)1, 0 ))
            ret = STATUS_PENDING;
          break;

        case 1:  /* in progress, wait */
          next = val & ~3;
          if (::InterlockedCompareExchangePointer(&once->Ptr, (void *)((ULONG_PTR)&next | 1), (void *)val ) == (void *)val)
            NtWaitForKeyedEvent( 0, &next, FALSE, NULL );
          goto for_begin;

        case 2:  /* done */
          ret = STATUS_SUCCESS;
          break;

        case 3:  /* in progress, async */
          ret = STATUS_INVALID_PARAMETER;
          break;
       }
      break;
    }

    if (ret != STATUS_PENDING) return !ret;

    if (!func(once, param, context)) {
      RunOnceCompleteXP(once, RTL_RUN_ONCE_INIT_FAILED, NULL);
      return !STATUS_UNSUCCESSFUL;
    }

  return !RunOnceCompleteXP(once, 0, context ? *context : NULL);
  }

 private:
  static DWORD RunOnceCompleteXP(INIT_ONCE* once, ULONG flags, void* context) {
    if ((ULONG_PTR)context & 3) return STATUS_INVALID_PARAMETER;

    if (flags & RTL_RUN_ONCE_INIT_FAILED) {
      if (context) return STATUS_INVALID_PARAMETER;
      if (flags & RTL_RUN_ONCE_ASYNC) return STATUS_INVALID_PARAMETER;
    }
    else context = (void *)((ULONG_PTR)context | 2);

    for (;;) {
      ULONG_PTR val = (ULONG_PTR)once->Ptr;
      switch (val & 3) {
        case 1:  /* in progress */
          if (::InterlockedCompareExchangePointer( &once->Ptr, context, (void *)val ) != (void *)val) break;
          val &= ~3;
          while (val) {
            ULONG_PTR next = *(ULONG_PTR *)val;
            NtReleaseKeyedEvent( 0, (void *)val, FALSE, NULL );
            val = next;
            }
            return STATUS_SUCCESS;

        case 3:  /* in progress, async */
          if (!(flags & RTL_RUN_ONCE_ASYNC)) return STATUS_INVALID_PARAMETER;
          if (::InterlockedCompareExchangePointer( &once->Ptr, context, (void *)val ) != (void *)val) break;
          return STATUS_SUCCESS;

        default:
          return STATUS_UNSUCCESSFUL;
      }
    }
  }
};

#endif
