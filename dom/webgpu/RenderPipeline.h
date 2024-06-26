/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GPU_RenderPipeline_H_
#define GPU_RenderPipeline_H_

#include "nsWrapperCache.h"
#include "ObjectModel.h"

namespace mozilla {
namespace webgpu {

class Device;

class RenderPipeline final : public ObjectBase, public ChildOf<Device> {
 public:
  GPU_DECL_CYCLE_COLLECTION(RenderPipeline)
  GPU_DECL_JS_WRAP(RenderPipeline)

 private:
  RenderPipeline() = delete;
  virtual ~RenderPipeline();
};

}  // namespace webgpu
}  // namespace mozilla

#endif  // GPU_RenderPipeline_H_
