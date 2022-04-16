/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_ipc_IPCStreamAlloc_h
#define mozilla_ipc_IPCStreamAlloc_h

namespace mozilla {
namespace ipc {

class PChildToParentStreamParent;
class PParentToChildStreamChild;

PChildToParentStreamParent* AllocPChildToParentStreamParent();

PParentToChildStreamChild* AllocPParentToChildStreamChild();

}  // namespace ipc
}  // namespace mozilla

#endif  // mozilla_ipc_IPCStreamAlloc_h
