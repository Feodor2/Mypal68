/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko;

import org.mozilla.gecko.util.EventCallback;
import org.mozilla.gecko.util.GeckoBundle;

import com.google.android.gms.cast.CastDevice;
import com.google.android.gms.cast.CastRemoteDisplayLocalService;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.api.Status;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.support.v7.media.MediaRouter.RouteInfo;
import android.util.Log;

public class ChromeCastDisplay implements GeckoPresentationDisplay {

    static final String REMOTE_DISPLAY_APP_ID = "4574A331";

    private static final String LOGTAG = "GeckoChromeCastDisplay";
    private final Context context;
    private final RouteInfo route;
    private CastDevice castDevice;

    public ChromeCastDisplay(Context context, RouteInfo route) {
        int status =  GooglePlayServicesUtil.isGooglePlayServicesAvailable(context);
        if (status != ConnectionResult.SUCCESS) {
            throw new IllegalStateException("Play services are required for Chromecast support (got status code " + status + ")");
        }

        this.context = context;
        this.route = route;
        this.castDevice = CastDevice.getFromBundle(route.getExtras());
    }

    @Override // GeckoPresentationDisplay
    public GeckoBundle toBundle() {
        if (castDevice == null) {
            return null;
        }

        final GeckoBundle obj = new GeckoBundle(3);
        obj.putString("uuid", route.getId());
        obj.putString("friendlyName", castDevice.getFriendlyName());
        obj.putString("type", "chromecast");
        return obj;
    }

    @Override
    public void start(final EventCallback callback) {

        if (CastRemoteDisplayLocalService.getInstance() != null) {
            Log.d(LOGTAG, "CastRemoteDisplayLocalService already existed.");
            GeckoAppShell.notifyObservers("presentation-view-ready", route.getId());
            callback.sendSuccess("Succeed to start presentation.");
            return;
        }

        Intent intent = new Intent(context, RemotePresentationService.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        PendingIntent notificationPendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

        CastRemoteDisplayLocalService.NotificationSettings settings =
            new CastRemoteDisplayLocalService.NotificationSettings.Builder()
                .setNotificationPendingIntent(notificationPendingIntent).build();

        CastRemoteDisplayLocalService.startService(
            context,
            RemotePresentationService.class,
            REMOTE_DISPLAY_APP_ID,
            castDevice,
            settings,
            new CastRemoteDisplayLocalService.Callbacks() {
                @Override
                public void onServiceCreated(CastRemoteDisplayLocalService service) {
                    ((RemotePresentationService) service).setDeviceId(route.getId());
                }

                @Override
                public void onRemoteDisplaySessionStarted(CastRemoteDisplayLocalService service) {
                    Log.d(LOGTAG, "Remote presentation launched!");
                    callback.sendSuccess("Succeed to start presentation.");
                }

                @Override
                public void onRemoteDisplaySessionError(Status errorReason) {
                    int code = errorReason.getStatusCode();
                    callback.sendError("Fail to start presentation. Error code: " + code);
                }

                @Override
                public void onRemoteDisplaySessionEnded(CastRemoteDisplayLocalService castRemoteDisplayLocalService) {

                }
            });
    }

    @Override
    public void stop(EventCallback callback) {
        CastRemoteDisplayLocalService.stopService();
        callback.sendSuccess("Succeed to stop presentation.");
    }
}
