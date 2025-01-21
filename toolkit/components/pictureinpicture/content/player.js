/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

const { PictureInPicture } = ChromeUtils.import(
  "resource://gre/modules/PictureInPicture.jsm"
);

// Time to fade the Picture-in-Picture video controls after first opening.
const CONTROLS_FADE_TIMEOUT = 3000;

/**
 * Public function to be called from PictureInPicture.jsm. This is the main
 * entrypoint for initializing the player window.
 *
 * @param id (Number)
 *   A unique numeric ID for the window, used for Telemetry Events.
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
 * The Player object handles initializing the player, holds state, and handles
 * events for updating state.
 */
let Player = {
  WINDOW_EVENTS: ["click", "keydown", "unload"],
  mm: null,
  /**
   * Used for window movement Telemetry to determine if the player window has
   * moved since the last time we checked.
   */
  lastScreenX: -1,
  lastScreenY: -1,
  id: -1,

  /**
   * When set to a non-null value, a timer is scheduled to hide the controls
   * after CONTROLS_FADE_TIMEOUT milliseconds.
   */
  showingTimeout: null,

  /**
   * Initializes the player browser, and sets up the initial state.
   *
   * @param id (Number)
   *   A unique numeric ID for the window, used for Telemetry Events.
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

      case "keydown": {
        if (event.keyCode == KeyEvent.DOM_VK_TAB) {
          this.controls.setAttribute("keying", true);
        } else if (event.keyCode == KeyEvent.DOM_VK_ESCAPE) {
          this.controls.removeAttribute("keying");
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

  onClick(event) {
    switch (event.target.id) {
      case "close": {
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
};
