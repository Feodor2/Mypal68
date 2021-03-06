/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef _mozilla_dom_ClientManagerActors_h
#define _mozilla_dom_ClientManagerActors_h

namespace mozilla {
namespace dom {

class PClientManagerChild;
class PClientManagerParent;

PClientManagerChild* AllocClientManagerChild();

bool DeallocClientManagerChild(PClientManagerChild* aActor);

PClientManagerParent* AllocClientManagerParent();

bool DeallocClientManagerParent(PClientManagerParent* aActor);

void InitClientManagerParent(PClientManagerParent* aActor);

}  // namespace dom
}  // namespace mozilla

#endif  // _mozilla_dom_ClientManagerActors_h
