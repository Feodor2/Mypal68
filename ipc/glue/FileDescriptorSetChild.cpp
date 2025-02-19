/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FileDescriptorSetChild.h"

namespace mozilla {
namespace ipc {

FileDescriptorSetChild::FileDescriptorSetChild(
    const FileDescriptor& aFileDescriptor) {
  mFileDescriptors.AppendElement(aFileDescriptor);
}

FileDescriptorSetChild::~FileDescriptorSetChild() {
  MOZ_ASSERT(mFileDescriptors.IsEmpty());
}

void FileDescriptorSetChild::ForgetFileDescriptors(
    nsTArray<FileDescriptor>& aFileDescriptors) {
  aFileDescriptors = std::move(mFileDescriptors);
}

mozilla::ipc::IPCResult FileDescriptorSetChild::RecvAddFileDescriptor(
    const FileDescriptor& aFileDescriptor) {
  mFileDescriptors.AppendElement(aFileDescriptor);
  return IPC_OK();
}

}  // namespace ipc
}  // namespace mozilla
