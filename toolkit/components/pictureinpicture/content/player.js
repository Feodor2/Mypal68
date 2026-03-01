/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

const { PictureInPicture } = ChromeUtils.import(
  "resource://gre/modules/PictureInPicture.jsm"
);
const { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");
const { AppConstants } = ChromeUtils.import(
  "resource://gre/modules/AppConstants.jsm"
);

const AUDIO_TOGGLE_ENABLED_PREF =
  "media.videocontrols.picture-in-picture.audio-toggle.enabled";
const KEYBOARD_CONTROLS_ENABLED_PREF =
  "media.videocontrols.picture-in-picture.keyboard-controls.enabled";

// Time to fade the Picture-in-Picture video controls after first opening.
const CONTROLS_FADE_TIMEOUT = 3000;

/**
 * Public function to be called from PictureInPicture.jsm. This is the main
 * entrypoint for initializing the player window.
 *
 * @param originatingBrowser (xul:browser)
 *   The <xul:browser> that the Picture-in-Picture video is coming from.
 */
function setupPlayer(originatingBrowser) {
  Player.init(originatingBrowser);
}

/**
 * Public function to be called from PictureInPicture.jsm. This update the
 * controls based on whether or not the video is playing.
 *
 * @param isPlaying (Boolean)
 *   True if the Picture-in-Picture video is playing.
 */
function setIsPlayingState(isPlaying) {
  Player.isPlaying = isPlaying;
}

/**
 * Public function to be called from PictureInPicture.jsm. This update the
 * controls based on whether or not the video is muted.
 *
 * @param isMuted (Boolean)
 *   True if the Picture-in-Picture video is muted.
 */
function setIsMutedState(isMuted) {
  Player.isMuted = isMuted;
}

/**
 * The Player object handles initializing the player, holds state, and handles
 * events for updating state.
 */
let Player = {
  WINDOW_EVENTS: [
    "click",
    "contextmenu",
    "dblclick",
    "keydown",
    "unload",
  ],
  mm: null,

  /**
   * When set to a non-null value, a timer is scheduled to hide the controls
   * after CONTROLS_FADE_TIMEOUT milliseconds.
   */
  showingTimeout: null,

  /**
   * Initializes the player browser, and sets up the initial state.
   *
   * @param originatingBrowser (xul:browser)
   *   The <xul:browser> that the Picture-in-Picture video is coming from.
   */
  init(originatingBrowser) {
    let holder = document.querySelector(".player-holder");
    let browser = document.getElementById("browser");
    browser.remove();

    browser.setAttribute("nodefaultsrc", "true");
    browser.sameProcessAsFrameLoader = originatingBrowser.frameLoader;
    holder.appendChild(browser);

    browser.loadURI("about:blank", {
      triggeringPrincipal: originatingBrowser.contentPrincipal,
    });

    this.mm = browser.frameLoader.messageManager;
    this.mm.sendAsyncMessage("PictureInPicture:SetupPlayer");
    for (let eventType of this.WINDOW_EVENTS) {
      addEventListener(eventType, this);
    }

    // If the content process hosting the video crashes, let's
    // just close the window for now.
    browser.addEventListener("oop-browser-crashed", this);

    this.revealControls(false);

    if (Services.prefs.getBoolPref(AUDIO_TOGGLE_ENABLED_PREF, false)) {
      const audioButton = document.getElementById("audio");
      audioButton.hidden = false;
      audioButton.previousElementSibling.hidden = false;
    }
    this.computeAndSetMinimumSize(window.outerWidth, window.outerHeight);
  },

  uninit() {
    PictureInPicture.unload();
  },

  handleEvent(event) {
    switch (event.type) {
      case "click": {
        this.onClick(event);
        this.controls.removeAttribute("keying");
        break;
      }

      case "contextmenu": {
        event.preventDefault();
        break;
      }

      case "dblclick": {
        this.onDblClick(event);
        break;
      }

      case "keydown": {
        if (event.keyCode == KeyEvent.DOM_VK_TAB) {
          this.controls.setAttribute("keying", true);
        } else if (
          event.keyCode == KeyEvent.DOM_VK_ESCAPE &&
          this.controls.hasAttribute("keying")
        ) {
          this.controls.removeAttribute("keying");

          // We preventDefault to avoid exiting fullscreen if we happen
          // to be in it.
          event.preventDefault();
        } else if (
          Services.prefs.getBoolPref(KEYBOARD_CONTROLS_ENABLED_PREF, false) &&
          !this.controls.hasAttribute("keying") &&
          (event.keyCode != KeyEvent.DOM_VK_SPACE || !event.target.id)
        ) {
          // Pressing "space" fires a "keydown" event which can also trigger a control
          // button's "click" event. Handle the "keydown" event only when the event did
          // not originate from a control button and it is not a "space" keypress.
          this.onKeyDown(event);
        }

        break;
      }

      case "oop-browser-crashed": {
        window.close();
        break;
      }

      case "unload": {
        this.uninit();
        break;
      }
    }
  },

  onDblClick(event) {
    if (event.target.id == "controls") {
      if (document.fullscreenElement == document.body) {
        document.exitFullscreen();
      } else {
        document.body.requestFullscreen();
      }
      event.preventDefault();
    }
  },

  onClick(event) {
    switch (event.target.id) {
      case "audio": {
        if (this.isMuted) {
          this.mm.sendAsyncMessage("PictureInPicture:Unmute");
        } else {
          this.mm.sendAsyncMessage("PictureInPicture:Mute");
        }
        break;
      }

      case "close": {
        this.mm.sendAsyncMessage("PictureInPicture:Pause");
        window.close();
        break;
      }

      case "playpause": {
        if (!this.isPlaying) {
          this.mm.sendAsyncMessage("PictureInPicture:Play");
          this.revealControls(false);
        } else {
          this.mm.sendAsyncMessage("PictureInPicture:Pause");
          this.revealControls(true);
        }

        break;
      }

      case "unpip": {
        PictureInPicture.focusTabAndClosePip();
        break;
      }
    }
  },

  onKeyDown(event) {
    this.mm.sendAsyncMessage("PictureInPicture:KeyDown", {
      altKey: event.altKey,
      shiftKey: event.shiftKey,
      metaKey: event.metaKey,
      ctrlKey: event.ctrlKey,
      keyCode: event.keyCode,
    });
  },

  get controls() {
    delete this.controls;
    return (this.controls = document.getElementById("controls"));
  },

  _isPlaying: false,
  /**
   * isPlaying returns true if the video is currently playing.
   *
   * @return Boolean
   */
  get isPlaying() {
    return this._isPlaying;
  },

  /**
   * Set isPlaying to true if the video is playing, false otherwise. This will
   * update the internal state and displayed controls.
   */
  set isPlaying(isPlaying) {
    this._isPlaying = isPlaying;
    this.controls.classList.toggle("playing", isPlaying);
  },

  _isMuted: false,
  /**
   * isMuted returns true if the video is currently muted.
   *
   * @return Boolean
   */
  get isMuted() {
    return this._isMuted;
  },

  /**
   * Set isMuted to true if the video is muted, false otherwise. This will
   * update the internal state and displayed controls.
   */
  set isMuted(isMuted) {
    this._isMuted = isMuted;
    this.controls.classList.toggle("muted", isMuted);
  },

  /**
   * Makes the player controls visible.
   *
   * @param revealIndefinitely (Boolean)
   *   If false, this will hide the controls again after
   *   CONTROLS_FADE_TIMEOUT_MS milliseconds has passed. If true, the controls
   *   will remain visible until revealControls is called again with
   *   revealIndefinitely set to false.
   */
  revealControls(revealIndefinitely) {
    clearTimeout(this.showingTimeout);
    this.showingTimeout = null;

    this.controls.setAttribute("showing", true);
    if (!revealIndefinitely) {
      this.showingTimeout = setTimeout(() => {
        this.controls.removeAttribute("showing");
      }, CONTROLS_FADE_TIMEOUT);
    }
  },

  computeAndSetMinimumSize(width, height) {
    if (!AppConstants.MOZ_WIDGET_GTK) {
      return;
    }

    // Using inspection, these seem to be the right minimums for each dimension
    // so that the controls don't get too crowded.
    const MIN_WIDTH = 120;
    const MIN_HEIGHT = 80;

    let resultWidth = width;
    let resultHeight = height;
    let aspectRatio = width / height;

    // Take the smaller of the two dimensions, and set it to the minimum.
    // Then calculate the other dimension using the aspect ratio to get
    // both minimums.
    if (width < height) {
      resultWidth = MIN_WIDTH;
      resultHeight = Math.round(MIN_WIDTH / aspectRatio);
    } else {
      resultHeight = MIN_HEIGHT;
      resultWidth = Math.round(MIN_HEIGHT * aspectRatio);
    }

    document.documentElement.style.minWidth = resultWidth + "px";
    document.documentElement.style.minHeight = resultHeight + "px";
  },
};
