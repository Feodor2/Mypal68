/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.widget;

import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;
import org.mozilla.gecko.R;
import org.mozilla.gecko.util.GeckoBundle;

import android.content.Context;
import android.view.View;

import org.mozilla.gecko.Telemetry;
import org.mozilla.gecko.TelemetryContract;
import org.mozilla.gecko.toolbar.SiteIdentityPopup;
import org.mozilla.gecko.util.GeckoBundle;

import java.util.Locale;

public class ContentSecurityDoorHanger extends DoorHanger {
    private static final String LOGTAG = "GeckoSecurityDoorHanger";

    private final TextView mTitle;
    private final TextView mSecurityState;
    private final TextView mMessage;

    public ContentSecurityDoorHanger(Context context, DoorhangerConfig config, Type type) {
        super(context, config, type);

        mTitle = (TextView) findViewById(R.id.security_title);
        mSecurityState = (TextView) findViewById(R.id.security_state);
        mMessage = (TextView) findViewById(R.id.security_message);

        loadConfig(config);
    }

    @Override
    protected void loadConfig(DoorhangerConfig config) {
        final String message = config.getMessage();
        if (message != null) {
            mMessage.setText(message);
        }

        final GeckoBundle options = config.getOptions();
        if (options != null) {
            setOptions(options);
        }

        final DoorhangerConfig.Link link = config.getLink();
        if (link != null) {
            addLink(link.label, link.url);
        }

        addButtonsToLayout(config);
    }

    @Override
    protected int getContentResource() {
        return R.layout.doorhanger_security;
    }

    @Override
    public void setOptions(final GeckoBundle options) {
        super.setOptions(options);

        final GeckoBundle link = options.getBundle("link");
        if (link != null) {
            final String linkLabel = link.getString("label");
            final String linkUrl = link.getString("url");
            addLink(linkLabel, linkUrl);
        }

        final GeckoBundle trackingProtection = options.getBundle("tracking_protection");
        if (trackingProtection != null) {
            mTitle.setVisibility(VISIBLE);
            mTitle.setText(R.string.doorhanger_tracking_title);

            final boolean enabled = trackingProtection.getBoolean("enabled");
            if (enabled) {
                mMessage.setText(R.string.doorhanger_tracking_message_enabled);
                mSecurityState.setText(R.string.doorhanger_tracking_state_enabled);
                mSecurityState.setTextColor(ContextCompat.getColor(getContext(),
                                            R.color.affirmative_green));
            } else {
                mMessage.setText(R.string.doorhanger_tracking_message_disabled);
                mSecurityState.setText(R.string.doorhanger_tracking_state_disabled);
                mSecurityState.setTextColor(ContextCompat.getColor(getContext(),
                                            R.color.rejection_red));
            }
            mMessage.setVisibility(VISIBLE);
            mSecurityState.setVisibility(VISIBLE);
        }
    }

    @Override
    protected OnClickListener makeOnButtonClickListener(final int id, final String telemetryExtra) {
        return new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                final String expandedExtra = mType.toString().toLowerCase(Locale.US) + "-" + telemetryExtra;
                Telemetry.sendUIEvent(TelemetryContract.Event.ACTION, TelemetryContract.Method.DOORHANGER, expandedExtra);

                final GeckoBundle response = new GeckoBundle(2);
                if (mType == Type.TRACKING) {
                    response.putBoolean("allowContent",
                                        id == SiteIdentityPopup.ButtonType.DISABLE.ordinal());
                    response.putString("contentType", "tracking");
                } else {
                    Log.w(LOGTAG, "Unknown doorhanger type " + mType.toString());
                }

                mOnButtonClickListener.onButtonClick(
                        response, ContentSecurityDoorHanger.this);
            }
        };
    }
}
