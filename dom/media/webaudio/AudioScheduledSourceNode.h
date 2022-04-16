/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AudioScheduledSourceNode_h_
#define AudioScheduledSourceNode_h_

#include "AudioNode.h"
#include "mozilla/dom/AudioScheduledSourceNodeBinding.h"

namespace mozilla {
namespace dom {

class AudioContext;

class AudioScheduledSourceNode : public AudioNode {
 public:
  JSObject* WrapObject(JSContext* aCx,
                       JS::Handle<JSObject*> aGivenProto) override;

  virtual void Start(double aWhen, ErrorResult& aRv) = 0;
  virtual void Stop(double aWhen, ErrorResult& aRv) = 0;

  IMPL_EVENT_HANDLER(ended)

 protected:
  AudioScheduledSourceNode(AudioContext* aContext, uint32_t aChannelCount,
                           ChannelCountMode aChannelCountMode,
                           ChannelInterpretation aChannelInterpretation);
  virtual ~AudioScheduledSourceNode() = default;
};

}  // namespace dom
}  // namespace mozilla

#endif
