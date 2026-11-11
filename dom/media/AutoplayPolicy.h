/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#if !defined(AutoplayPolicy_h_)
#  define AutoplayPolicy_h_

#  include "mozilla/NotNull.h"

class nsIPrincipal;

namespace mozilla {
namespace dom {

class HTMLMediaElement;
class AudioContext;
class Document;
enum class DocumentAutoplayPolicy : uint8_t;

/**
 * AutoplayPolicy is used to manage autoplay logic for all kinds of media,
 * including MediaElement, Web Audio and Web Speech.
 *
 * Autoplay could be disable by setting the pref "media.autoplay.default"
 * to anything but nsIAutoplay::Allowed. Once user disables autoplay, media
 * could only be played if one of following conditions is true.
 * 1) Owner document is activated by user gestures
 *    We restrict user gestures to "mouse click", "keyboard press" and "touch".
 * 2) Muted media content or video without audio content.
 * 3) Document's origin has the "autoplay-media" permission.
 */
class AutoplayPolicy {
 public:
  // Returns a DocumentAutoplayPolicy for given document.
  static DocumentAutoplayPolicy IsAllowedToPlay(const Document& aDocument);

  // Returns whether a given media element is allowed to play.
  static bool IsAllowedToPlay(const HTMLMediaElement& aElement);

  // Returns whether a given AudioContext is allowed to play.
  static bool IsAllowedToPlay(const AudioContext& aContext);

  // Return the value of the autoplay permissin for given principal. The return
  // value can be 0=unknown, 1=allow, 2=block audio, 5=block audio and video.
  static uint32_t GetSiteAutoplayPermission(nsIPrincipal* aPrincipal);
};

}  // namespace dom
}  // namespace mozilla

#endif
