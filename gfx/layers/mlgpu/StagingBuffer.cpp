/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "StagingBuffer.h"
#include "MLGDevice.h"
#include "ShaderDefinitionsMLGPU.h"

namespace mozilla {
namespace layers {

ConstantStagingBuffer::ConstantStagingBuffer(MLGDevice* aDevice)
    : StagingBuffer(mlg::kMaxConstantBufferSize) {}

}  // namespace layers
}  // namespace mozilla
