/* -*- Mode: Java; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko;

import org.mozilla.gecko.GeckoDataPreferences;
import org.mozilla.gecko.util.ThreadUtils;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.app.Notification;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationCompat.Builder;
import android.util.Log;

public class DataReportingNotification extends BroadcastReceiver {

    private static final String LOGTAG = "DataReportNotification";

    public static final String INTENT_EXTRAS_NOTIFICATION_INTENT = "fromNotification";
    public static final String ALERT_NAME_DATAREPORTING_NOTIFICATION = "datareporting-notification";
    public static final String PREFS_POLICY_RESPONSE_TIME = "datareporting.policy.dataSubmissionPolicyResponseTime";

    private static final String PREFS_POLICY_NOTIFIED_TIME = "datareporting.policy.dataSubmissionPolicyNotifiedTime";
    private static final String PREFS_POLICY_VERSION = "datareporting.policy.dataSubmissionPolicyVersion";
    private static final int DATA_REPORTING_VERSION = 1;

    public static void checkAndNotifyPolicy(Context context) {
        SharedPreferences dataPrefs = context.getSharedPreferences(GeckoDataPreferences.PREFS_BRANCH, 0);

        // Notify if user has not responded, or if policy version has changed.
        if ((!dataPrefs.contains(PREFS_POLICY_RESPONSE_TIME)) ||
            (DATA_REPORTING_VERSION != dataPrefs.getInt(PREFS_POLICY_VERSION, -1))) {

            // Launch Data Choices preferences screen on notification click.
            Intent prefIntent = new Intent(context, GeckoDataPreferences.class);
            prefIntent.putExtra(INTENT_EXTRAS_NOTIFICATION_INTENT, true);
            PendingIntent contentIntent = PendingIntent.getActivity(context, 0, prefIntent, PendingIntent.FLAG_UPDATE_CURRENT);

            // Handle notification dismissal in this BroadcastReceiver.
            Intent broadcastIntent = new Intent(context, DataReportingNotification.class);
            PendingIntent deleteIntent = PendingIntent.getBroadcast(context, 0, broadcastIntent, 0);

            // Create and send notification.
            String notificationTitle = context.getResources().getString(R.string.datareporting_notification_title);
            String notificationSummary = context.getResources().getString(R.string.datareporting_notification_summary);
            Notification notification = new NotificationCompat.Builder(context)
                                        .setContentTitle(notificationTitle)
                                        .setContentText(notificationSummary)
                                        .setSmallIcon(R.drawable.ic_status_logo)
                                        .setAutoCancel(true)
                                        .setContentIntent(contentIntent)
                                        .setDeleteIntent(deleteIntent)
                                        .build();

            NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
            int notificationID = ALERT_NAME_DATAREPORTING_NOTIFICATION.hashCode();
            notificationManager.notify(notificationID, notification);

            // Record version and notification time.
            SharedPreferences.Editor editor = dataPrefs.edit();
            long now = System.currentTimeMillis();
            editor.putLong(PREFS_POLICY_NOTIFIED_TIME, now);
            editor.putInt(PREFS_POLICY_VERSION, DATA_REPORTING_VERSION);
            editor.commit();
        }
    }

    @Override
    public void onReceive(final Context context, Intent intent) {
        ThreadUtils.postToBackgroundThread(new Runnable() {
            @Override
            public void run() {
                // Record time of notification response/dismiss.
                SharedPreferences.Editor editor = context.getSharedPreferences(GeckoDataPreferences.PREFS_BRANCH, 0).edit();
                long now = System.currentTimeMillis();
                editor.putLong(PREFS_POLICY_RESPONSE_TIME, now);
                editor.commit();
            }
        });
    }
}
