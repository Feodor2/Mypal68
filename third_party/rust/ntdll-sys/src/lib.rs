//! FFI bindings to ntdll.
#![cfg(windows)]
extern crate winapi;
use winapi::*;
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
