/*__kernel_entry NTSYSCALLAPI


typedef enum _FILE_INFORMATION_CLASS {
    FileDirectoryInformation                         = 1,
    FileFullDirectoryInformation,                   // 2
    FileBothDirectoryInformation,                   // 3
    FileBasicInformation,                           // 4
    FileStandardInformation,                        // 5
    FileInternalInformation,                        // 6
    FileEaInformation,                              // 7
    FileAccessInformation,                          // 8
    FileNameInformation,                            // 9
    FileRenameInformation,                          // 10
    FileLinkInformation,                            // 11
    FileNamesInformation,                           // 12
    FileDispositionInformation,                     // 13
    FilePositionInformation,                        // 14
    FileFullEaInformation,                          // 15
    FileModeInformation,                            // 16
    FileAlignmentInformation,                       // 17
    FileAllInformation,                             // 18
    FileAllocationInformation,                      // 19
    FileEndOfFileInformation,                       // 20
    FileAlternateNameInformation,                   // 21
    FileStreamInformation,                          // 22
    FilePipeInformation,                            // 23
    FilePipeLocalInformation,                       // 24
    FilePipeRemoteInformation,                      // 25
    FileMailslotQueryInformation,                   // 26
    FileMailslotSetInformation,                     // 27
    FileCompressionInformation,                     // 28
    FileObjectIdInformation,                        // 29
    FileCompletionInformation,                      // 30
    FileMoveClusterInformation,                     // 31
    FileQuotaInformation,                           // 32
    FileReparsePointInformation,                    // 33
    FileNetworkOpenInformation,                     // 34
    FileAttributeTagInformation,                    // 35
    FileTrackingInformation,                        // 36
    FileIdBothDirectoryInformation,                 // 37
    FileIdFullDirectoryInformation,                 // 38
    FileValidDataLengthInformation,                 // 39
    FileShortNameInformation,                       // 40
    FileIoCompletionNotificationInformation,        // 41
    FileIoStatusBlockRangeInformation,              // 42
    FileIoPriorityHintInformation,                  // 43
    FileSfioReserveInformation,                     // 44
    FileSfioVolumeInformation,                      // 45
    FileHardLinkInformation,                        // 46
    FileProcessIdsUsingFileInformation,             // 47
    FileNormalizedNameInformation,                  // 48
    FileNetworkPhysicalNameInformation,             // 49
    FileIdGlobalTxDirectoryInformation,             // 50
    FileIsRemoteDeviceInformation,                  // 51
    FileUnusedInformation,                          // 52
    FileNumaNodeInformation,                        // 53
    FileStandardLinkInformation,                    // 54
    FileRemoteProtocolInformation,                  // 55

        //
        //  These are special versions of these operations (defined earlier)
        //  which can be used by kernel mode drivers only to bypass security
        //  access checks for Rename and HardLink operations.  These operations
        //  are only recognized by the IOManager, a file system should never
        //  receive these.
        //

    FileRenameInformationBypassAccessCheck,         // 56
    FileLinkInformationBypassAccessCheck,           // 57

        //
        // End of special information classes reserved for IOManager.
        //

    FileVolumeNameInformation,                      // 58
    FileIdInformation,                              // 59
    FileIdExtdDirectoryInformation,                 // 60
    FileReplaceCompletionInformation,               // 61
    FileHardLinkFullIdInformation,                  // 62
    FileIdExtdBothDirectoryInformation,             // 63
    FileDispositionInformationEx,                   // 64
    FileRenameInformationEx,                        // 65
    FileRenameInformationExBypassAccessCheck,       // 66
    FileDesiredStorageClassInformation,             // 67
    FileStatInformation,                            // 68
    FileMemoryPartitionInformation,                 // 69
    FileStatLxInformation,                          // 70
    FileCaseSensitiveInformation,                   // 71
    FileLinkInformationEx,                          // 72
    FileLinkInformationExBypassAccessCheck,         // 73
    FileStorageReserveIdInformation,                // 74
    FileCaseSensitiveInformationForceAccessCheck,   // 75
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;


typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    } DUMMYUNIONNAME;

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


NTSTATUS
NTAPI
NtSetInformationFile (
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass
    );

__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationFile (
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass
    );

extern "system" {
    pub fn GetFileInformationByHandleEx(
        hFile: HANDLE,
        FileInformationClass: FILE_INFO_BY_HANDLE_CLASS,
        lpFileInformation: LPVOID,
        dwBufferSize: DWORD,
    ) -> BOOL;
}*/
use shared::basetsd::{DWORD_PTR, ULONG32, ULONG64, ULONG_PTR};
use shared::ntdef::NTSTATUS;
use shared::minwindef::{
    ATOM, BOOL, BYTE, DWORD, FARPROC, FILETIME, HFILE, HGLOBAL, HLOCAL, HMODULE, HRSRC, LPBOOL,
    LPBYTE, LPCVOID, LPDWORD, LPFILETIME, LPVOID, LPWORD, PBOOL, PDWORD, PUCHAR, PULONG, PUSHORT,
    UCHAR, UINT, ULONG, USHORT, WORD,
};
use um::winnt::{
    BOOLEAN, CHAR, DWORDLONG, EXECUTION_STATE, FILE_ID_128, HANDLE, HRESULT, INT, LANGID,
    LARGE_INTEGER, LATENCY_TIME, LONG, LPCCH, LPCH, LPCSTR, LPCWSTR, LPOSVERSIONINFOEXA,
    LPOSVERSIONINFOEXW, LPSTR, LPWSTR, MAXLONG, PBOOLEAN, PCONTEXT, PCWSTR, PFIRMWARE_TYPE,
    PHANDLE, PIO_COUNTERS, PJOB_SET_ARRAY, PLUID, POWER_REQUEST_TYPE, PPERFORMANCE_DATA,
    PPROCESSOR_NUMBER, PQUOTA_LIMITS, PRTL_UMS_SCHEDULER_ENTRY_POINT,
    PSECURE_MEMORY_CACHE_CALLBACK, PSID, PSID_NAME_USE, PULONGLONG, PVOID, PWOW64_CONTEXT,
    PWOW64_LDT_ENTRY, PWSTR, RTL_UMS_THREAD_INFO_CLASS, STATUS_ABANDONED_WAIT_0, STATUS_USER_APC,
    STATUS_WAIT_0, SecurityAnonymous, SecurityDelegation, SecurityIdentification,
    SecurityImpersonation, THREAD_BASE_PRIORITY_IDLE, THREAD_BASE_PRIORITY_LOWRT,
    THREAD_BASE_PRIORITY_MAX, THREAD_BASE_PRIORITY_MIN, ULARGE_INTEGER, VOID, WAITORTIMERCALLBACK,
    WCHAR, WOW64_CONTEXT,
};

pub type PIO_STATUS_BLOCK = *mut IO_STATUS_BLOCK;
UNION!{union IO_STATUS_BLOCK_u {
    [u64; 2],
    Status Status_mut: NTSTATUS,
    Pointer Pointer_mut: PVOID,
}}
STRUCT!{struct IO_STATUS_BLOCK {
    u: IO_STATUS_BLOCK_u,
    Information: ULONG_PTR,
}}

STRUCT!{struct FILE_IO_COMPLETION_NOTIFICATION_INFORMATION {
    Flags: ULONG,
}}

extern "system" {
    pub fn NtQueryInformationFile(
        hFile: HANDLE,
        IoStatusBlock: PIO_STATUS_BLOCK,
        FileInformation: PVOID,
        Length: ULONG,
        FileInformationClass: UINT,
    ) -> NTSTATUS;
    pub fn NtSetInformationFile(
        hFile: HANDLE,
        IoStatusBlock: PIO_STATUS_BLOCK,
        FileInformation: PVOID,
        Length: ULONG,
        FileInformationClass: UINT,
    ) -> NTSTATUS;
}
