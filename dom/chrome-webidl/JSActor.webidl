/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


interface mixin JSActor {
    [Throws]
    undefined sendAsyncMessage(DOMString messageName,
                               optional any obj);

    [NewObject]
    Promise<any> sendQuery(DOMString messageName,
                           optional any obj);
  };

/**
 * WebIDL callback interface version of the nsIObserver interface for use when
 * calling the observe method on JSActors.
 *
 * NOTE: This isn't marked as ChromeOnly, as it has no interface object, and
 * thus cannot be conditionally exposed.
 */
[Exposed=Window]
callback interface MozObserverCallback {
  undefined observe(nsISupports subject, ByteString topic, DOMString? data);
};

/**
 * WebIDL callback interface calling the `willDestroy`, `didDestroy`, and
 * `actorCreated` methods on JSActors.
 */
[MOZ_CAN_RUN_SCRIPT_BOUNDARY]
callback MozJSActorCallback = undefined();

/**
 * The willDestroy method, if present, will be called at the last opportunity
 * to send messages to the remote side, giving implementers the chance to clean
 * up and send final messages.
 * The didDestroy method, if present, will be called after the actor is no
 * longer able to receive any more messages.
 * The actorCreated method, if present, will be called immediately after the
 * actor has been created and initialized.
 *
 * NOTE: Messages may be received between willDestroy and didDestroy, but they
 * may not be s ent.
 */
[GenerateInit]
dictionary MozJSActorCallbacks {
  [ChromeOnly] MozJSActorCallback willDestroy;
  [ChromeOnly] MozJSActorCallback didDestroy;
  [ChromeOnly] MozJSActorCallback actorCreated;
};
