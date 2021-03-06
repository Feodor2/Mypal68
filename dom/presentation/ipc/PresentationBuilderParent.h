/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_PresentationBuilderParent_h__
#define mozilla_dom_PresentationBuilderParent_h__

#include "mozilla/dom/PPresentationBuilderParent.h"
#include "PresentationParent.h"
#include "nsIPresentationSessionTransportBuilder.h"

namespace mozilla {
namespace dom {

class PresentationBuilderParent final
    : public PPresentationBuilderParent,
      public nsIPresentationDataChannelSessionTransportBuilder {
 public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPRESENTATIONSESSIONTRANSPORTBUILDER
  NS_DECL_NSIPRESENTATIONDATACHANNELSESSIONTRANSPORTBUILDER

  explicit PresentationBuilderParent(PresentationParent* aParent);

  mozilla::ipc::IPCResult RecvSendOffer(const nsString& aSDP);

  mozilla::ipc::IPCResult RecvSendAnswer(const nsString& aSDP);

  mozilla::ipc::IPCResult RecvSendIceCandidate(const nsString& aCandidate);

  mozilla::ipc::IPCResult RecvClose(const nsresult& aReason);

  virtual void ActorDestroy(ActorDestroyReason aWhy) override;

  mozilla::ipc::IPCResult RecvOnSessionTransport();

  mozilla::ipc::IPCResult RecvOnSessionTransportError(const nsresult& aReason);

 private:
  virtual ~PresentationBuilderParent();
  bool mNeedDestroyActor = false;
  RefPtr<PresentationParent> mParent;
  nsCOMPtr<nsIPresentationSessionTransportBuilderListener> mBuilderListener;
  nsCOMPtr<nsIPresentationSessionTransport> mIPCSessionTransport;
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_PresentationBuilderParent_h__
