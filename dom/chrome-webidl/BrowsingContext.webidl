/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

interface nsIDocShell;

[Exposed=Window, ChromeOnly]
interface BrowsingContext {
  static BrowsingContext? get(unsigned long long aId);

  BrowsingContext? findChildWithName(DOMString name);
  BrowsingContext? findWithName(DOMString name);

  readonly attribute DOMString name;

  readonly attribute BrowsingContext? parent;

  readonly attribute BrowsingContext top;

  sequence<BrowsingContext> getChildren();

  readonly attribute nsIDocShell? docShell;

  readonly attribute Element? embedderElement;

  readonly attribute unsigned long long id;

  readonly attribute BrowsingContext? opener;

  readonly attribute BrowsingContextGroup group;

  // The inRDMPane flag indicates whether or not Responsive Design Mode is
  // active for the browsing context.
  attribute boolean inRDMPane;

  // Extension to give chrome JS the ability to set the window screen
  // orientation while in RDM.
  undefined setRDMPaneOrientation(OrientationType type, float rotationAngle);
};

[Exposed=Window, ChromeOnly]
interface CanonicalBrowsingContext : BrowsingContext {
  sequence<WindowGlobalParent> getWindowGlobals();

  readonly attribute WindowGlobalParent? currentWindowGlobal;

  // XXX(nika): This feels kinda hacky, but will do for now while we don't
  // synchronously create WindowGlobalParent. It can throw if somehow the
  // content process has died.
  [Throws]
  readonly attribute UTF8String? currentRemoteType;

  readonly attribute WindowGlobalParent? embedderWindowGlobal;

  undefined notifyStartDelayedAutoplayMedia();
  undefined notifyMediaMutedChanged(boolean muted);
};

[Exposed=Window, ChromeOnly]
interface BrowsingContextGroup {
  sequence<BrowsingContext> getToplevels();
};
