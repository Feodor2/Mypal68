# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

browser-main-window =
  .data-title-default = { -brand-full-name }
  .data-title-private = { -brand-full-name } (Private Browsing)
  .data-content-title-default = { $content-title } - { -brand-full-name }
  .data-content-title-private = { $content-title } - { -brand-full-name } (Private Browsing)

browser-main-window-mac =
  .data-title-default = { -brand-full-name }
  .data-title-private = { -brand-full-name } - (Private Browsing)
  .data-content-title-default = { $content-title }
  .data-content-title-private = { $content-title } - (Private Browsing)

browser-main-window-title = { -brand-full-name }

appmenuitem-customize-mode =
    .label = Customize…

urlbar-identity-button =
    .aria-label = View site information

## Tooltips for images appearing in the address bar

urlbar-services-notification-anchor =
    .tooltiptext = Open install message panel
urlbar-web-notification-anchor =
    .tooltiptext = Change whether you can receive notifications from the site
urlbar-midi-notification-anchor =
    .tooltiptext = Open MIDI panel
urlbar-web-authn-anchor =
    .tooltiptext = Open Web Authentication panel
urlbar-canvas-notification-anchor =
    .tooltiptext = Manage canvas extraction permission
urlbar-web-rtc-share-microphone-notification-anchor =
    .tooltiptext = Manage sharing your microphone with the site
urlbar-default-notification-anchor =
    .tooltiptext = Open message panel
urlbar-geolocation-notification-anchor =
    .tooltiptext = Open location request panel
urlbar-storage-access-anchor =
    .tooltiptext = Open browsing activity permission panel
urlbar-translate-notification-anchor =
    .tooltiptext = Translate this page
urlbar-web-rtc-share-screen-notification-anchor =
    .tooltiptext = Manage sharing your windows or screen with the site
urlbar-indexed-db-notification-anchor =
    .tooltiptext = Open offline storage message panel
urlbar-password-notification-anchor =
    .tooltiptext = Open save password message panel
urlbar-translated-notification-anchor =
    .tooltiptext = Manage page translation
urlbar-plugins-notification-anchor =
    .tooltiptext = Manage plug-in use
urlbar-web-rtc-share-devices-notification-anchor =
    .tooltiptext = Manage sharing your camera and/or microphone with the site
urlbar-autoplay-notification-anchor =
    .tooltiptext = Open autoplay panel
urlbar-persistent-storage-notification-anchor =
    .tooltiptext = Store data in Persistent Storage
urlbar-addons-notification-anchor =
    .tooltiptext = Open add-on installation message panel

urlbar-geolocation-blocked =
    .tooltiptext = You have blocked location information for this website.
urlbar-web-notifications-blocked =
    .tooltiptext = You have blocked notifications for this website.
urlbar-camera-blocked =
    .tooltiptext = You have blocked your camera for this website.
urlbar-microphone-blocked =
    .tooltiptext = You have blocked your microphone for this website.
urlbar-screen-blocked =
    .tooltiptext = You have blocked this website from sharing your screen.
urlbar-persistent-storage-blocked =
    .tooltiptext = You have blocked persistent storage for this website.
urlbar-popup-blocked =
    .tooltiptext = You have blocked pop-ups for this website.
urlbar-autoplay-media-blocked =
    .tooltiptext = You have blocked autoplay media with sound for this website.
urlbar-canvas-blocked =
    .tooltiptext = You have blocked canvas data extraction for this website.
urlbar-flash-blocked =
    .tooltiptext = You have blocked this website from using the Adobe Flash plugin.
urlbar-midi-blocked =
    .tooltiptext = You have blocked MIDI access for this website.
urlbar-install-blocked =
    .tooltiptext = You have blocked add-on installation for this website.

# Variables
#   $shortcut (String) - A keyboard shortcut for the edit bookmark command.
urlbar-star-edit-bookmark =
    .tooltiptext = Edit this bookmark ({ $shortcut })

# Variables
#   $shortcut (String) - A keyboard shortcut for the add bookmark command.
urlbar-star-add-bookmark =
    .tooltiptext = Bookmark this page ({ $shortcut })

## Page Action Context Menu

page-action-add-to-urlbar =
    .label = Add to Address Bar
page-action-manage-extension =
    .label = Manage Extension…
page-action-remove-from-urlbar =
    .label = Remove from Address Bar

## Page Action menu

# Variables
# $tabCount (integer) - Number of tabs selected
page-action-send-tabs-panel =
  .label = { $tabCount ->
      [1] Send Tab to Device
     *[other] Send { $tabCount } Tabs to Device
  }
page-action-send-tabs-urlbar =
  .tooltiptext = { $tabCount ->
      [1] Send Tab to Device
     *[other] Send { $tabCount } Tabs to Device
  }
page-action-copy-url-panel =
  .label = Copy Link
page-action-copy-url-urlbar =
  .tooltiptext = Copy Link
page-action-email-link-panel =
  .label = Email Link…
page-action-email-link-urlbar =
  .tooltiptext = Email Link…
page-action-send-tab-not-ready =
  .label = Syncing Devices…
# "Pin" is being used as a metaphor for expressing the fact that these tabs
# are "pinned" to the left edge of the tabstrip. Really we just want the
# string to express the idea that this is a lightweight and reversible
# action that keeps your tab where you can reach it easily.
page-action-pin-tab-panel =
  .label = Pin Tab
page-action-pin-tab-urlbar =
  .tooltiptext = Pin Tab
page-action-unpin-tab-panel =
  .label = Unpin Tab
page-action-unpin-tab-urlbar =
  .tooltiptext = Unpin Tab

## Auto-hide Context Menu

full-screen-autohide =
    .label = Hide Toolbars
    .accesskey = H
full-screen-exit =
    .label = Exit Full Screen Mode
    .accesskey = F

## Search Engine selection buttons (one-offs)

# This string prompts the user to use the list of one-click search engines in
# the Urlbar and searchbar.
search-one-offs-with-title = This time, search with:

# This string won't wrap, so if the translated string is longer,
# consider translating it as if it said only "Search Settings".
search-one-offs-change-settings-button =
    .label = Change Search Settings
search-one-offs-change-settings-compact-button =
    .tooltiptext = Change search settings

search-one-offs-context-open-new-tab =
    .label = Search in New Tab
    .accesskey = T
search-one-offs-context-set-as-default =
    .label = Set as Default Search Engine
    .accesskey = D

## Bookmark Panel

bookmark-panel-show-editor-checkbox =
    .label = Show editor when saving
    .accesskey = S

bookmark-panel-done-button =
    .label = Done

# Width of the bookmark panel.
# Should be large enough to fully display the Done and
# Cancel/Remove Bookmark buttons.
bookmark-panel =
    .style = min-width: 23em

## WebRTC Pop-up notifications

popup-select-camera =
.value = Camera to share:
.accesskey = C
popup-select-microphone =
.value = Microphone to share:
.accesskey = M
popup-all-windows-shared = All visible windows on your screen will be shared.

## URL Bar

urlbar-placeholder =
  .placeholder = Search or enter address

# Variables
#  $name (String): the name of the user's default search engine
urlbar-placeholder-with-name =
  .placeholder = Search with { $name } or enter address
urlbar-remote-control-notification-anchor =
  .tooltiptext = Browser is under remote control
urlbar-permissions-granted =
  .tooltiptext = You have granted this website additional permissions.
urlbar-switch-to-tab =
  .value = Switch to tab:

# Used to indicate that a selected autocomplete entry is provided by an extension.
urlbar-extension =
  .value = Extension:

urlbar-go-end-cap =
  .tooltiptext = Go to the address in the Location Bar
urlbar-page-action-button =
  .tooltiptext = Page actions
