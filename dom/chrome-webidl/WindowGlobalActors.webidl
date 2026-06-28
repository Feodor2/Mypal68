/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

interface Principal;
interface URI;
interface nsIDocShell;
interface RemoteTab;
interface nsITransportSecurityInfo;

[Exposed=Window, ChromeOnly]
interface WindowGlobalParent {
  readonly attribute boolean isClosed;
  readonly attribute boolean isInProcess;
  readonly attribute CanonicalBrowsingContext browsingContext;

  readonly attribute boolean isCurrentGlobal;

  readonly attribute unsigned long long innerWindowId;
  readonly attribute unsigned long long outerWindowId;

  readonly attribute FrameLoader? rootFrameLoader; // Embedded (browser) only

  readonly attribute WindowGlobalChild? childActor; // in-process only

  readonly attribute RemoteTab? remoteTab; // out-of-process only

  // Information about the currently loaded document.
  readonly attribute Principal documentPrincipal;
  readonly attribute URI? documentURI;

  static WindowGlobalParent? getByInnerWindowId(unsigned long long innerWindowId);

  /**
   * Get or create the JSWindowActor with the given name.
   *
   * See WindowActorOptions from JSWindowActor.webidl for details on how to
   * customize actor creation.
   */
  [Throws]
  JSWindowActorParent getActor(UTF8String name);

  [NewObject]
  Promise<RemoteTab> changeFrameRemoteness(
    BrowsingContext? bc, UTF8String remoteType,
    unsigned long long pendingSwitchId);

  /**
   * Fetches the securityInfo object for this window. This function will
   * look for failed and successful channels to find the security info,
   * thus it will work on regular HTTPS pages as well as certificate
   * error pages.
   *
   * This returns a Promise which resolves to an nsITransportSecurity
   * object with certificate data or undefined if no security info is available.
   */
  [NewObject]
  Promise<nsITransportSecurityInfo> getSecurityInfo();
};

[Exposed=Window, ChromeOnly]
interface WindowGlobalChild {
  readonly attribute boolean isClosed;
  readonly attribute boolean isInProcess;
  readonly attribute BrowsingContext browsingContext;

  readonly attribute boolean isCurrentGlobal;

  readonly attribute unsigned long long innerWindowId;
  readonly attribute unsigned long long outerWindowId;

  readonly attribute WindowGlobalParent? parentActor; // in-process only

  static WindowGlobalChild? getByInnerWindowId(unsigned long long innerWIndowId);

  /**
   * Get or create the JSWindowActor with the given name.
   *
   * See WindowActorOptions from JSWindowActor.webidl for details on how to
   * customize actor creation.
   */
  [Throws]
  JSWindowActorChild getActor(UTF8String name);
};
