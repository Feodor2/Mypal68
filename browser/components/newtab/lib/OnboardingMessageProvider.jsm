/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
"use strict";
const {Localization} = ChromeUtils.import("resource://gre/modules/Localization.jsm");
const {FxAccountsConfig} = ChromeUtils.import("resource://gre/modules/FxAccountsConfig.jsm");
const {AttributionCode} = ChromeUtils.import("resource:///modules/AttributionCode.jsm");
const {Services} = ChromeUtils.import("resource://gre/modules/Services.jsm");

const L10N = new Localization([
  "branding/brand.ftl",
  "browser/branding/brandings.ftl",
  "browser/branding/sync-brand.ftl",
  "browser/newtab/onboarding.ftl",
]);

const ONBOARDING_MESSAGES = async () => ([
  {
    id: "ONBOARDING_1",
    template: "onboarding",
    bundled: 3,
    order: 2,
    content: {
      title: {string_id: "onboarding-private-browsing-title"},
      text: {string_id: "onboarding-private-browsing-text"},
      icon: "privatebrowsing",
      primary_button: {
        label: {string_id: "onboarding-button-label-try-now"},
        action: {type: "OPEN_PRIVATE_BROWSER_WINDOW"},
      },
    },
    trigger: {id: "showOnboarding"},
  },
  {
    id: "ONBOARDING_2",
    template: "onboarding",
    bundled: 3,
    order: 3,
    content: {
      title: {string_id: "onboarding-screenshots-title"},
      text: {string_id: "onboarding-screenshots-text"},
      icon: "screenshots",
      primary_button: {
        label: {string_id: "onboarding-button-label-try-now"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://screenshots.firefox.com/#tour", where: "tabshifted"},
        },
      },
    },
    trigger: {id: "showOnboarding"},
  },
  {
    id: "ONBOARDING_3",
    template: "onboarding",
    bundled: 3,
    order: 1,
    content: {
      title: {string_id: "onboarding-addons-title"},
      text: {string_id: "onboarding-addons-text"},
      icon: "addons",
      primary_button: {
        label: {string_id: "onboarding-button-label-try-now"},
        action: {
          type: "OPEN_ABOUT_PAGE",
          data: {args: "addons"},
        },
      },
    },
    targeting: "trailheadInterrupt == 'control' && attributionData.campaign != 'non-fx-button' && attributionData.source != 'addons.mozilla.org'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "ONBOARDING_4",
    template: "onboarding",
    bundled: 3,
    order: 1,
    content: {
      title: {string_id: "onboarding-ghostery-title"},
      text: {string_id: "onboarding-ghostery-text"},
      icon: "gift",
      primary_button: {
        label: {string_id: "onboarding-button-label-try-now"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://addons.mozilla.org/en-US/firefox/addon/ghostery/", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadInterrupt == 'control' && providerCohorts.onboarding == 'ghostery'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "ONBOARDING_5",
    template: "onboarding",
    bundled: 3,
    order: 4,
    content: {
      title: {string_id: "onboarding-fxa-title"},
      text: {string_id: "onboarding-fxa-text"},
      icon: "sync",
      primary_button: {
        label: {string_id: "onboarding-button-label-get-started"},
        action: {
          type: "OPEN_URL",
          data: {args: await FxAccountsConfig.promiseEmailFirstURI("onboarding"), where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadInterrupt == 'control' && attributionData.campaign == 'non-fx-button' && attributionData.source == 'addons.mozilla.org'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_1",
    template: "trailhead",
    targeting: "trailheadInterrupt == 'join'",
    trigger: {id: "firstRun"},
    includeBundle: {length: 3, template: "onboarding", trigger: {id: "showOnboarding"}},
    utm_term: "trailhead-join",
    content: {
      className: "joinCohort",
      title: {string_id: "onboarding-welcome-body"},
      benefits: ["products", "knowledge", "privacy"].map(id => (
        {
          id,
          title: {string_id: `onboarding-benefit-${id}-title`},
          text: {string_id: `onboarding-benefit-${id}-text`},
        }
      )),
      learn: {
        text: {string_id: "onboarding-welcome-learn-more"},
        url: "https://www.mozilla.org/firefox/accounts/",
      },
      form: {
        title: {string_id: "onboarding-join-form-header"},
        text: {string_id: "onboarding-join-form-body"},
        email: {string_id: "onboarding-join-form-email"},
        button: {string_id: "onboarding-join-form-continue"},
      },
      skipButton: {string_id: "onboarding-start-browsing-button-label"},
    },
  },
  {
    id: "TRAILHEAD_2",
    template: "trailhead",
    targeting: "trailheadInterrupt == 'sync'",
    trigger: {id: "firstRun"},
    includeBundle: {length: 3, template: "onboarding", trigger: {id: "showOnboarding"}},
    utm_term: "trailhead-sync",
    content: {
      className: "syncCohort",
      title: {property_id: "firstrun_title"},
      subtitle: {property_id: "firstrun_content"},
      benefits: [],
      learn: {
        text: {property_id: "firstrun_learn_more_link"},
        url: "https://www.mozilla.org/firefox/accounts/",
      },
      form: {
        title: {property_id: "firstrun_form_header"},
        text: {property_id: "firstrun_form_sub_header"},
        email: {property_id: "firstrun_email_input_placeholder"},
        button: {property_id: "firstrun_continue_to_login"},
      },
      skipButton: {property_id: "firstrun_skip_login"},
    },
  },
  {
    id: "TRAILHEAD_3",
    template: "trailhead",
    targeting: "trailheadInterrupt == 'cards'",
    trigger: {id: "firstRun"},
    includeBundle: {length: 3, template: "onboarding", trigger: {id: "showOnboarding"}},
    utm_term: "trailhead-cards",
  },
  {
    id: "TRAILHEAD_4",
    template: "trailhead",
    targeting: "trailheadInterrupt == 'nofirstrun'",
    trigger: {id: "firstRun"},
  },
  {
    id: "TRAILHEAD_CARD_1",
    template: "onboarding",
    bundled: 3,
    order: 2,
    content: {
      title: {string_id: "onboarding-tracking-protection-title2"},
      text: {string_id: "onboarding-tracking-protection-text2"},
      icon: "tracking",
      primary_button: {
        label: {string_id: "onboarding-tracking-protection-button2"},
        action: Services.locale.appLocaleAsLangTag.substr(0, 2) === "en" ? {
          type: "OPEN_URL",
          data: {args: "https://mzl.la/ETPdefault", where: "tabshifted"},
        } : {
          type: "OPEN_PREFERENCES_PAGE",
          data: {category: "privacy-trackingprotection"},
        },
      },
    },
    targeting: "trailheadTriplet == 'privacy'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_2",
    template: "onboarding",
    bundled: 3,
    order: 2,
    content: {
      title: {string_id: "onboarding-data-sync-title"},
      text: {string_id: "onboarding-data-sync-text2"},
      icon: "devices",
      primary_button: {
        label: {string_id: "onboarding-data-sync-button2"},
        action: {
          type: "OPEN_URL",
          addFlowParams: true,
          data: {args: "https://accounts.firefox.com/?service=sync&action=email&context=fx_desktop_v3&entrypoint=activity-stream-firstrun&style=trailhead", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet == 'supercharge'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_3",
    template: "onboarding",
    bundled: 3,
    order: 3,
    content: {
      title: {string_id: "onboarding-firefox-monitor-title"},
      text: {string_id: "onboarding-firefox-monitor-text"},
      icon: "ffmonitor",
      primary_button: {
        label: {string_id: "onboarding-firefox-monitor-button"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://monitor.firefox.com/", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet in ['payoff', 'supercharge']",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_4",
    template: "onboarding",
    bundled: 3,
    order: 1,
    content: {
      title: {string_id: "onboarding-browse-privately-title"},
      text: {string_id: "onboarding-browse-privately-text"},
      icon: "private",
      primary_button: {
        label: {string_id: "onboarding-browse-privately-button"},
        action: {type: "OPEN_PRIVATE_BROWSER_WINDOW"},
      },
    },
    targeting: "trailheadTriplet == 'privacy'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_5",
    template: "onboarding",
    bundled: 3,
    order: 5,
    content: {
      title: {string_id: "onboarding-firefox-send-title"},
      text: {string_id: "onboarding-firefox-send-text2"},
      icon: "ffsend",
      primary_button: {
        label: {string_id: "onboarding-firefox-send-button"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://send.firefox.com/", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet == 'payoff'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_6",
    template: "onboarding",
    bundled: 3,
    order: 1,
    content: {
      title: {string_id: "onboarding-mobile-phone-title"},
      text: {string_id: "onboarding-mobile-phone-text"},
      icon: "mobile",
      primary_button: {
        label: {string_id: "onboarding-mobile-phone-button"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://www.mozilla.org/firefox/mobile/", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet in ['supercharge', 'multidevice']",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_7",
    template: "onboarding",
    bundled: 3,
    order: 3,
    content: {
      title: {string_id: "onboarding-send-tabs-title"},
      text: {string_id: "onboarding-send-tabs-text"},
      icon: "sendtab",
      primary_button: {
        label: {string_id: "onboarding-send-tabs-button"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://support.mozilla.org/kb/send-tab-firefox-desktop-other-devices", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet == 'multidevice'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_8",
    template: "onboarding",
    bundled: 3,
    order: 2,
    content: {
      title: {string_id: "onboarding-pocket-anywhere-title"},
      text: {string_id: "onboarding-pocket-anywhere-text2"},
      icon: "pocket",
      primary_button: {
        label: {string_id: "onboarding-pocket-anywhere-button"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://getpocket.com/firefox_learnmore", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet == 'multidevice'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_9",
    template: "onboarding",
    bundled: 3,
    order: 3,
    content: {
      title: {string_id: "onboarding-lockwise-passwords-title"},
      text: {string_id: "onboarding-lockwise-passwords-text2"},
      icon: "lockwise",
      primary_button: {
        label: {string_id: "onboarding-lockwise-passwords-button2"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://lockwise.firefox.com/", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet == 'privacy'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "TRAILHEAD_CARD_10",
    template: "onboarding",
    bundled: 3,
    order: 4,
    content: {
      title: {string_id: "onboarding-facebook-container-title"},
      text: {string_id: "onboarding-facebook-container-text2"},
      icon: "fbcont",
      primary_button: {
        label: {string_id: "onboarding-facebook-container-button"},
        action: {
          type: "OPEN_URL",
          data: {args: "https://addons.mozilla.org/firefox/addon/facebook-container/", where: "tabshifted"},
        },
      },
    },
    targeting: "trailheadTriplet == 'payoff'",
    trigger: {id: "showOnboarding"},
  },
  {
    id: "FXA_1",
    template: "fxa_overlay",
    trigger: {id: "firstRun"},
  },
]);

const OnboardingMessageProvider = {
  async getExtraAttributes() {
    const [header, button_label] = await L10N.formatMessages([
      {id: "onboarding-welcome-header"},
      {id: "onboarding-start-browsing-button-label"},
    ]);
    return {header: header.value, button_label: button_label.value};
  },
  async getMessages() {
    const messages = await this.translateMessages(await ONBOARDING_MESSAGES());
    return messages;
  },
  async getUntranslatedMessages() {
    // This is helpful for jsonSchema testing - since we are localizing in the provider
    const messages = await ONBOARDING_MESSAGES();
    return messages;
  },
  async translateMessages(messages) {
    let translatedMessages = [];
    for (const msg of messages) {
      let translatedMessage = {...msg};

      // If the message has no content, do not attempt to translate it
      if (!translatedMessage.content) {
        translatedMessages.push(translatedMessage);
        continue;
      }

      // Translate any secondary buttons separately
      if (msg.content.secondary_button) {
        const [secondary_button_string] = await L10N.formatMessages([{id: msg.content.secondary_button.label.string_id}]);
        translatedMessage.content.secondary_button.label = secondary_button_string.value;
      }
      if (msg.content.header) {
        const [header_string] = await L10N.formatMessages([{id: msg.content.header.string_id}]);
        translatedMessage.content.header = header_string.value;
      }
      translatedMessages.push(translatedMessage);
    }
    return translatedMessages;
  },
};
this.OnboardingMessageProvider = OnboardingMessageProvider;

const EXPORTED_SYMBOLS = ["OnboardingMessageProvider"];
