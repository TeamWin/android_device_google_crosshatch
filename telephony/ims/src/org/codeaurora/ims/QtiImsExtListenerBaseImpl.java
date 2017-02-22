/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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

import org.codeaurora.ims.internal.IQtiImsExtListener;
import android.telephony.Rlog;

/**
 * This class contains default implementation for IQtiImsExtListener.
 */
public class QtiImsExtListenerBaseImpl extends IQtiImsExtListener.Stub {

    @Override
    public void onSetCallForwardUncondTimer(int status) {
    }

    @Override
    public void onGetCallForwardUncondTimer(int startHour, int endHour, int startMinute,
            int endMinute, int reason, int status, String number, int service) {
    }

    @Override
    public void onUTReqFailed(int errCode, String errString) {
    }

    @Override
    public void onGetPacketCount(int status, long packetCount) {
    }

    @Override
    public void onGetPacketErrorCount(int status, long packetErrorCount) {
    }

    @Override
    public void receiveCallDeflectResponse(int result) {
    }

    @Override
    public void receiveCallTransferResponse(int result) {
    }

    @Override
    public void notifyVopsStatus(boolean vopsStatus) {
    }

    @Override
    public void notifySsacStatus(boolean ssacStatusResponse) {
    }

    @Override
    public void notifyRefreshViceInfo(QtiViceInfo viceInfo) {
    }

    @Override
    public void notifyParticipantStatusInfo(int operation, int sipStatus,
            String participantUri, boolean isEct) {
    }

    @Override
    public void onVoltePreferenceUpdated(int result) {
    }

    @Override
    public void onVoltePreferenceQueried(int result, int mode) {
    }

    @Override
    public void onSetHandoverConfig(int result) {
    }

    @Override
    public void onGetHandoverConfig(int result, int hoConfig) {
    }
}

