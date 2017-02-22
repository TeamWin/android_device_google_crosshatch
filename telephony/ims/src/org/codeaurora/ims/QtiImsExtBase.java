/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
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

import org.codeaurora.ims.internal.IQtiImsExt;
import org.codeaurora.ims.internal.IQtiImsExtListener;
import org.codeaurora.ims.QtiCallConstants;

/**
 * Base implementation for IQtiImsExt.
 */
public abstract class QtiImsExtBase {

    /*
     * Implement the methods of the IQtiImsExt interface in this stub
     */
    public final class QtiImsExtBinder extends IQtiImsExt.Stub {

        @Override
        public void setCallForwardUncondTimer(int startHour, int startMinute, int endHour,
                int endMinute, int action, int condition, int serviceClass, String number,
                IQtiImsExtListener listener) {
            onSetCallForwardUncondTimer(startHour, startMinute, endHour, endMinute, action,
                condition, serviceClass, number, listener);
        }

        @Override
        public void getCallForwardUncondTimer(int reason, int serviceClass,
                IQtiImsExtListener listener) {
            onGetCallForwardUncondTimer(reason, serviceClass, listener);
        }

        @Override
        public void getPacketCount(IQtiImsExtListener listener) {
            onGetPacketCount(listener);
        }

        @Override
        public void getPacketErrorCount(IQtiImsExtListener listener) {
            onGetPacketErrorCount(listener);
        }

        @Override
        public void sendCallDeflectRequest(int phoneId, String deflectNumber,
                IQtiImsExtListener listener) {
            onSendCallDeflectRequest(phoneId, deflectNumber, listener);
        }

        @Override
        public void resumePendingCall(int videoState) {
            onResumePendingCall(videoState);
        }

        @Override
        public void sendCallTransferRequest(int phoneId, int type, String number,
                IQtiImsExtListener listener) {
            onSendCallTransferRequest(phoneId, type, number, listener);
        }

        @Override
        public void queryVopsStatus(IQtiImsExtListener listener) {
            onQueryVopsStatus(listener);
        }

        @Override
        public void querySsacStatus(IQtiImsExtListener listener) {
            onQuerySsacStatus(listener);
        }

        @Override
        public int getImsPhoneId() {
            return onGetImsPhoneId();
        }

        @Override
        public void registerForViceRefreshInfo(IQtiImsExtListener listener) {
            onRegisterForViceRefreshInfo(listener);
        }

        @Override
        public void registerForParticipantStatusInfo(IQtiImsExtListener listener) {
            onRegisterForParticipantStatusInfo(listener);
        }

        @Override
        public void updateVoltePreference(int phoneId, int preference,
                IQtiImsExtListener listener) {
            onUpdateVoltePreference(phoneId, preference, listener);
        }

        @Override
        public void queryVoltePreference(int phoneId, IQtiImsExtListener listener) {
            onQueryVoltePreference(phoneId, listener);
        }

        @Override
        public void getHandoverConfig(IQtiImsExtListener listener) {
            onGetHandoverConfig(listener);
        }

        @Override
        public void setHandoverConfig(int hoConfig,
                IQtiImsExtListener listener) {
            onSetHandoverConfig(hoConfig, listener);
        }
    };

    private QtiImsExtBinder mQtiImsExtBinder;

    public QtiImsExtBinder getBinder() {
        if (mQtiImsExtBinder == null) {
            mQtiImsExtBinder = new QtiImsExtBinder();
        }
        return mQtiImsExtBinder;
    }

    protected void onSetCallForwardUncondTimer(int startHour, int startMinute, int endHour,
            int endMinute, int action, int condition, int serviceClass, String number,
            IQtiImsExtListener listener) {
        // no-op
    }
    protected void onGetCallForwardUncondTimer(int reason, int serviceClass,
            IQtiImsExtListener listener) {
        // no-op
    }
    protected void onGetPacketCount(IQtiImsExtListener listener) {
        // no-op
    }
    protected void onGetPacketErrorCount(IQtiImsExtListener listener) {
        // no-op
    }
    protected void onSendCallDeflectRequest(int phoneId, String deflectNumber,
            IQtiImsExtListener listener) {
        // no-op
    }
    protected void onResumePendingCall(int videoState) {
        // no-op
    }
    protected void onSendCallTransferRequest(int phoneId, int type, String number,
            IQtiImsExtListener listener) {
        // no-op
    }
    protected void onQueryVopsStatus(IQtiImsExtListener listener) {
        // no-op
    }
    protected void onQuerySsacStatus(IQtiImsExtListener listener) {
        // no-op
    }
    protected int onGetImsPhoneId() {
        return QtiCallConstants.INVALID_PHONE_ID;
    }
    protected void onRegisterForViceRefreshInfo(IQtiImsExtListener listener) {
        // no-op
    }
    protected void onRegisterForParticipantStatusInfo(IQtiImsExtListener listener) {
        // no-op
    }
    protected void onUpdateVoltePreference(int phoneId, int preference,
            IQtiImsExtListener listener) {
        // no-op
    }
    protected void onQueryVoltePreference(int phoneId, IQtiImsExtListener listener) {
        // no-op
    }
    protected void onGetHandoverConfig(IQtiImsExtListener listener) {
        // no-op
    }
    protected void onSetHandoverConfig(int hoConfig,
            IQtiImsExtListener listener) {
        // no-op
    }
}
