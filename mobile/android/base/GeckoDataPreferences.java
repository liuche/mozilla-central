/* -*- Mode: Java; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko;

import org.mozilla.gecko.GeckoPreferencesActivity;

import android.app.Activity;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;

public class GeckoDataPreferences
    extends GeckoPreferencesActivity
{
    public static final String PREFS_BRANCH = "datareporting";
    private static final String LOGTAG = "GeckoDataPreferences";
    private static final String PREF_HEALTHREPORT_ENABLED = NON_PREF_PREFIX + "healthreport.uploadEnabled";

    @Override
    protected int getPreferencesResource() {
        return R.xml.datachoices_preferences;
    }

    @Override
    protected void setupPrefsBranch() {
        this.getPreferenceManager().setSharedPreferencesName(PREFS_BRANCH);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        final String prefName = preference.getKey();
        if (!TextUtils.isEmpty(prefName)) {
            if (PREF_HEALTHREPORT_ENABLED.equals(prefName)) {
                // Healthreport pref is not mirrored to Gecko.
                return true;
            }
            // Set pref value in Gecko.
            PrefsHelper.setPref(prefName, newValue);
        }
        return true;
    }
}
