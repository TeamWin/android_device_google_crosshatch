/*
 * Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


package org.codeaurora.ims;

import android.os.Parcel;
import android.os.Parcelable;
import android.telephony.Rlog;

import java.util.ArrayList;
import java.util.List;

/*
 * This file contains all the api's through which
 * information received in Vice Dialog can be
 * queried
 */

/**
 * Parcelable object to handle VICE Dialog Information
 * @hide
 */

public class QtiViceInfo implements Parcelable {

    private static final String TAG = "QtiViceInfo";
    private String mViceInfoAsString = null;
    public int dialogSize = 0;
    public static List<String[]> callInfo;

    public static final int INDEX_DIALOG_ID = 0;
    public static final int INDEX_NUMBER = 1;
    public static final int INDEX_ISPULLABLE = 2;
    public static final int INDEX_CALLTYPE = 3;
    public static final int INDEX_DIRECTION = 4;
    // INDEX Values listed above + 1 for the array to hold it
    public static final int INDEX_MAX = 5;

    // Strings/values that are part in DEP XML
    public static final String STATE_TERMINATED = "terminated";
    public static final String STATE_CONFIRMED = "confirmed";
    public static final String MEDIA_TYPE_AUDIO = "audio";
    public static final String MEDIA_TYPE_VIDEO = "video";
    public static final String MEDIA_DIRECTION_SENDRECV = "sendrecv";
    public static final String MEDIA_DIRECTION_SENDONLY = "sendonly";
    public static final String MEDIA_DIRECTION_RECVONLY = "recvonly";
    public static final String MEDIA_DIRECTION_INACTIVE = "inactive";

    // Translated values after processing the media attributes
    public static final String CALL_TYPE_VOICE_ACTIVE = "volteactive";
    public static final String CALL_TYPE_VOICE_HELD = "volteheld";
    public static final String CALL_TYPE_VIDEO_TX_RX = "vttxrx";
    public static final String CALL_TYPE_VIDEO_TX = "vttx";
    public static final String CALL_TYPE_VIDEO_RX = "vtrx";
    public static final String CALL_TYPE_VIDEO_HELD = "vtheld";


    public QtiViceInfo() {
    }

    public QtiViceInfo(Parcel in) {
        readFromParcel(in);
    }

    /**
     * dialogIds Will have following information -
     * dialogIds[INDEX_DIALOG_ID] - Holds Unique DialogId
     * dialogIds[INDEX_NUMBER] - Holds number/uri
     * dialogIds[INDEX_ISPULLABLE] - Pullable/NonPullable (true, false)
     * dialogIds[INDEX_CALLTYPE] - CallType
     *     CallType - volteactive, volteheld, vttxrx, vttx, vtrx, vtheld
     * dialogIds[INDEX_DIRECTION] - Direction of the call (Originator/recipent)
     */
    public QtiViceInfo(List<String[]> dialogIds) {
        if (dialogIds != null) {
            callInfo = new ArrayList<String[]>();
            dialogSize = dialogIds.size();
            callInfo = dialogIds;
            Rlog.d(TAG,"QtiViceInfo const = " + toString());
        }
    }

    public void setViceDialogInfoAsString(String value) {
        mViceInfoAsString = value;
        Rlog.d(TAG,"setViceDialogInfoAsString XML String = " + mViceInfoAsString);
    }

    public String getViceDialogInfoAsString() {
        return mViceInfoAsString;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    private void readFromParcel(Parcel in) {
       dialogSize = in.readInt();
       Rlog.d(TAG, "readFromParcel size = " + dialogSize);
       if (dialogSize < 0) {
           return;
       }
       callInfo = new ArrayList<String[]>();
       for (int i = 0; i < dialogSize; i++) {
           String[] info = new String[INDEX_MAX];
           info = in.createStringArray(); // read each string[]
           callInfo.add(info);
        }
        Rlog.d(TAG, "readFromParcel - " + toString());
    }

    public String toString() {
        if (callInfo != null) {
            Rlog.d(TAG, "mCallInfo size = " + dialogSize);
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < dialogSize; i++) {
                String[] callinfo = callInfo.get(i);
                sb.append("QtiViceInfo :");
                sb.append("DialogId - ");
                sb.append(callinfo[INDEX_DIALOG_ID]);
                sb.append("Number - ");
                sb.append(callinfo[INDEX_NUMBER]);
                sb.append("IsPullable - ");
                sb.append(callinfo[INDEX_ISPULLABLE]);
                sb.append("CallType - ");
                sb.append(callinfo[INDEX_CALLTYPE]);
                sb.append("Direction - ");
                sb.append(callinfo[INDEX_DIRECTION]);
            }
            return sb.toString();
        }
        return null;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        if (callInfo != null) {
            out.writeInt(dialogSize);
            for (int i = 0; i < dialogSize; i++) {
                String[] callPull = callInfo.get(i);
                out.writeStringArray(callPull);
            }
            out.setDataPosition(0);
        }
    }

    public static final Creator<QtiViceInfo> CREATOR =
            new Creator<QtiViceInfo>() {
        @Override
        public QtiViceInfo createFromParcel(Parcel in) {
            return new QtiViceInfo(in);
        }

        @Override
        public QtiViceInfo[] newArray(int size) {
            return new QtiViceInfo[size];
        }
    };
}
