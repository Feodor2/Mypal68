/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GPU_DeviceLostInfo_H_
#define GPU_DeviceLostInfo_H_

#include "nsWrapperCache.h"
#include "ObjectModel.h"

namespace mozilla {
namespace webgpu {
class Device;

class DeviceLostInfo final : public nsWrapperCache, public ChildOf<Device> {
 public:
  GPU_DECL_CYCLE_COLLECTION(DeviceLostInfo)
  GPU_DECL_JS_WRAP(DeviceLostInfo)

 private:
  DeviceLostInfo() = delete;
  virtual ~DeviceLostInfo();

 public:
  void GetMessage(nsAString& aValue) const {}
};

}  // namespace webgpu
}  // namespace mozilla

#endif  // GPU_DeviceLostInfo_H_
