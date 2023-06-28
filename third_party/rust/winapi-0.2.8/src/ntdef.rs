// Copyright © 2015, skdltmxn
// Licensed under the MIT License <LICENSE.md>
//! Type definitions for the basic types.
//909
pub type NTSTATUS = ::LONG;
pub type PNTSTATUS = *mut NTSTATUS;
pub type PCNTSTATUS = *const NTSTATUS;

pub type PIO_STATUS_BLOCK = *mut IO_STATUS_BLOCK;
UNION!(IO_STATUS_BLOCK, u, Status, Status_mut, NTSTATUS);
UNION!(IO_STATUS_BLOCK, u, Pointer, Pointer_mut, ::PVOID);
#[cfg(target_arch="x86_64")]
STRUCT!{struct IO_STATUS_BLOCK {
    u: [u32; 8],
    Information: ::ULONG_PTR,
}}
#[cfg(target_arch="x86")]
STRUCT!{struct IO_STATUS_BLOCK {
    u: [u32; 4],
    Information: ::ULONG_PTR,
}}

STRUCT!{struct FILE_IO_COMPLETION_NOTIFICATION_INFORMATION {
    Flags: ::ULONG,
}}
