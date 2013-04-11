/* -*- Mode: Java; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko;

import org.mozilla.gecko.GeckoPreferencesActivity;

import android.preference.Preference;
import android.preference.PreferenceManager;
import android.text.TextUtils;

public class GeckoDataPreferences
    extends GeckoPreferencesActivity
{
    private static final String PREFS_HEALTHREPORT_UPLOAD_ENABLED = NON_PREF_PREFIX + "healthreport.uploadEnabled";
    private static final String PREFS_BRANCH = "datareporting";

    @Override
    protected int getPreferencesResource() {
        return R.xml.datareporting_preferences;
    }

    @Override
    protected void setupPrefsBranch() {
        this.getPreferenceManager().setSharedPreferencesName(PREFS_BRANCH);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        String prefName = preference.getKey();
        if (!TextUtils.isEmpty(prefName)) {
            // Healthreport pref only lives in Android. Do not persist to Gecko.
            if (!PREFS_HEALTHREPORT_UPLOAD_ENABLED.equals(prefName)) {
                PrefsHelper.setPref(prefName, newValue);
            }
        }
        return true;
    }
}
