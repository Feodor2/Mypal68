# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

aboutDialog-title =
    .title = About { -brand-full-name }

releaseNotes-link = What’s new

update-checkForUpdatesButton =
    .label = Check for updates
    .accesskey = C

update-updateButton =
    .label = Restart to update { -brand-shorter-name }
    .accesskey = R

update-checkingForUpdates = Checking for updates…
update-downloading = <img data-l10n-name="icon"/>Downloading update — <label data-l10n-name="download-status"/>
update-applying = Applying update…

update-failed = Update failed. <label data-l10n-name="failed-link">Download the latest version</label>
update-failed-main =
    Update failed. <a data-l10n-name="failed-link-main">Download the latest version</a>

update-adminDisabled = Updates disabled by your system administrator
update-noUpdatesFound = { -brand-short-name } is up to date
update-otherInstanceHandlingUpdates = { -brand-short-name } is being updated by another instance

update-manual = Updates available at <label data-l10n-name="manual-link"/>

update-restarting = Restarting…

community-2 = { -brand-short-name } is based on Mozilla Firefox adapted for winxp\win2003.

bottomLinks-license = Licensing Information
bottomLinks-rights = End-User Rights
bottomLinks-privacy = Build Configuration

# Example of resulting string: 66.0.1 (64-bit)
# Variables:
#   $version (String): version of Firefox, e.g. 66.0.1
#   $bits (Number): bits of the architecture (32 or 64)
aboutDialog-version = { $version } ({ $bits }-bit)

# Example of resulting string: 66.0a1 (2019-01-16) (64-bit)
# Variables:
#   $version (String): version of Firefox for Nightly builds, e.g. 66.0a1
#   $isodate (String): date in ISO format, e.g. 2019-01-16
#   $bits (Number): bits of the architecture (32 or 64)
aboutDialog-version-nightly = { $version } ({ $isodate }) ({ $bits }-bit)
