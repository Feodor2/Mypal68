/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

const permissionError =
  "error: NotAllowedError: The request is not allowed " +
  "by the user agent or the platform in the current context.";

const badDeviceError =
  "error: NotReadableError: Failed to allocate videosource";

var gTests = [
  {
    desc: "test queueing deny audio behind allow video",
    run: async function testQueuingDenyAudioBehindAllowVideo() {
      let promise = promisePopupNotificationShown("webRTC-shareDevices");
      await promiseRequestDevice(false, true);
      await promiseRequestDevice(true, false);
      await promise;
      promise = promisePopupNotificationShown("webRTC-shareDevices");
      checkDeviceSelectors(false, true);
      await expectObserverCalled("getUserMedia:request");
      let indicator = promiseIndicatorWindow();

      await promiseMessage("ok", () => {
        PopupNotifications.panel.firstElementChild.button.click();
      });

      await expectObserverCalled("getUserMedia:response:allow");
      await expectObserverCalled("recording-device-events");
      Assert.deepEqual(
        await getMediaCaptureState(),
        { video: true },
        "expected camera to be shared"
      );
      await indicator;
      await checkSharingUI({ audio: false, video: true });

      await promise;
      await expectObserverCalled("getUserMedia:request");
      checkDeviceSelectors(true, false);

      await promiseMessage(permissionError, () => {
        activateSecondaryAction(kActionDeny);
      });

      await expectObserverCalled("getUserMedia:response:deny");
      SitePermissions.removeFromPrincipal(
        null,
        "microphone",
        gBrowser.selectedBrowser
      );

      // close all streams
      await closeStream();
    },
  },

  {
    desc: "test queueing allow video behind deny audio",
    run: async function testQueuingAllowVideoBehindDenyAudio() {
      let promise = promisePopupNotificationShown("webRTC-shareDevices");
      await promiseRequestDevice(true, false);
      await promiseRequestDevice(false, true);
      await promise;
      promise = promisePopupNotificationShown("webRTC-shareDevices");
      await expectObserverCalled("getUserMedia:request");
      checkDeviceSelectors(true, false);

      await promiseMessage(permissionError, () => {
        activateSecondaryAction(kActionDeny);
      });

      await expectObserverCalled("getUserMedia:response:deny");

      await promise;
      checkDeviceSelectors(false, true);
      await expectObserverCalled("getUserMedia:request");

      let indicator = promiseIndicatorWindow();

      await promiseMessage("ok", () => {
        PopupNotifications.panel.firstElementChild.button.click();
      });

      await expectObserverCalled("getUserMedia:response:allow");
      await expectObserverCalled("recording-device-events");
      Assert.deepEqual(
        await getMediaCaptureState(),
        { video: true },
        "expected camera to be shared"
      );
      await indicator;
      await checkSharingUI({ audio: false, video: true });

      SitePermissions.removeFromPrincipal(
        null,
        "microphone",
        gBrowser.selectedBrowser
      );

      // close all streams
      await closeStream();
    },
  },

  {
    desc: "test queueing allow audio behind allow video with error",
    run: async function testQueuingAllowAudioBehindAllowVideoWithError() {
      let promise = promisePopupNotificationShown("webRTC-shareDevices");
      await promiseRequestDevice(
        false,
        true,
        null,
        null,
        gBrowser.selectedBrowser,
        true
      );
      await promiseRequestDevice(true, false);
      await promise;
      promise = promisePopupNotificationShown("webRTC-shareDevices");

      checkDeviceSelectors(false, true);

      await expectObserverCalled("getUserMedia:request");

      await promiseMessage(badDeviceError, () => {
        PopupNotifications.panel.firstElementChild.button.click();
      });

      await expectObserverCalled("getUserMedia:response:allow");

      await promise;
      checkDeviceSelectors(true, false);
      await expectObserverCalled("getUserMedia:request");
      let indicator = promiseIndicatorWindow();

      await promiseMessage("ok", () => {
        PopupNotifications.panel.firstElementChild.button.click();
      });

      await expectObserverCalled("getUserMedia:response:allow");
      await expectObserverCalled("recording-device-events");
      Assert.deepEqual(
        await getMediaCaptureState(),
        { audio: true },
        "expected microphone to be shared"
      );
      await indicator;
      await checkSharingUI({ audio: true, video: false });

      // close all streams
      await closeStream();
    },
  },

  {
    desc: "test queueing audio+video behind deny audio",
    run: async function testQueuingAllowVideoBehindDenyAudio() {
      let promise = promisePopupNotificationShown("webRTC-shareDevices");
      await promiseRequestDevice(true, false);
      await promiseRequestDevice(true, true);
      await promise;
      await expectObserverCalled("getUserMedia:request");
      checkDeviceSelectors(true, false);

      promise = promiseSpecificMessageReceived(permissionError, 2);
      activateSecondaryAction(kActionDeny);
      await promise;

      await expectObserverCalled("getUserMedia:request");
      await expectObserverCalled("getUserMedia:response:deny", 2);
      await expectObserverCalled("recording-window-ended");

      SitePermissions.removeFromPrincipal(
        null,
        "microphone",
        gBrowser.selectedBrowser
      );
    },
  },

  {
    desc: "test queueing audio, video behind reload after pending audio, video",
    run: async function testQueuingDenyAudioBehindAllowVideo() {
      let observerPromise = expectObserverCalled("getUserMedia:request");
      let promise = promisePopupNotificationShown("webRTC-shareDevices");
      await promiseRequestDevice(true, false);
      await promiseRequestDevice(false, true);
      await promise;
      await observerPromise;
      checkDeviceSelectors(true, false);

      await reloadAndAssertClosedStreams();

      observerPromise = expectObserverCalled("getUserMedia:request");
      // After the reload, gUM(audio) causes a prompt.
      promise = promisePopupNotificationShown("webRTC-shareDevices");
      await promiseRequestDevice(true, false);
      await promiseRequestDevice(false, true);
      await promise;
      await observerPromise;
      checkDeviceSelectors(true, false);

      let indicator = promiseIndicatorWindow();
      let observerPromise1 = expectObserverCalled(
        "getUserMedia:response:allow"
      );
      let observerPromise2 = expectObserverCalled("recording-device-events");

      // expect pending camera prompt to appear after ok'ing microphone one.
      observerPromise = expectObserverCalled("getUserMedia:request");
      promise = promisePopupNotificationShown("webRTC-shareDevices");

      await promiseMessage("ok", () => {
        PopupNotifications.panel.firstElementChild.button.click();
      });
      await observerPromise1;
      await observerPromise2;
      Assert.deepEqual(
        await getMediaCaptureState(),
        { audio: true },
        "expected microphone to be shared"
      );
      await indicator;
      await checkSharingUI({ video: false, audio: true });

      await promise;
      await observerPromise;
      checkDeviceSelectors(false, true);

      observerPromise1 = expectObserverCalled("getUserMedia:response:allow");
      observerPromise2 = expectObserverCalled("recording-device-events");
      await promiseMessage("ok", () => {
        PopupNotifications.panel.firstElementChild.button.click();
      });
      await observerPromise1;
      await observerPromise2;
      Assert.deepEqual(
        await getMediaCaptureState(),
        { audio: true, video: true },
        "expected microphone and camera to be shared"
      );

      // close all streams
      await closeStream();
    },
  },
];

add_task(async function test() {
  await runTests(gTests);
});
