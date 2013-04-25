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
    extends GeckoPreferenceFragment {
{
    private static final String PREFS_BRANCH = "datareporting";

    @Override
    protected void setupPrefsBranch() {
        this.getPreferenceManager().setSharedPreferencesName(PREFS_BRANCH);
    }
}
