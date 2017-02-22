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

import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;

import org.codeaurora.ims.internal.IQtiImsExt;
import org.codeaurora.ims.internal.IQtiImsExtListener;
import org.codeaurora.ims.QtiCallConstants;

/**
 * Provides API's for IQtiImsExt Binder such as sending call deflect, call transfer etc
 * This class is starting point for all the QtiImsExt actions.
 * You can acquire an instance of it by calling {@link getInstance getInstance()}
 *
 * Note: The implementation of QtiImsExtManager is not synchronized hence this is
 * not a thread safe class, Assuming all the users will call the API's from the same thread
 */
public class QtiImsExtManager {

    /**
     * Key to retrieve service form the ServiceManager
     */
    public static final String SERVICE_ID = "qti.ims.ext";

    /**
     * Singleton instance of the {@link QtiImsExtManager}
     */
    private static QtiImsExtManager sInstance;

    /**
     * All the QtiImsExt actions are performed using this interface,
     * this interface/binder provides API's such as sending call deflect,
     * call transfer etc
     */
    private IQtiImsExt mQtiImsExt;

    private QtiImsExtManager() {
        // Empty Constructor
    }

    public static QtiImsExtManager getInstance() {
        if(sInstance == null) {
            sInstance = new QtiImsExtManager();
        }
        return sInstance;
    }

    public void setCallForwardUncondTimer(int startHour, int startMinute, int endHour,
            int endMinute, int action, int condition, int serviceClass, String number,
            IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.setCallForwardUncondTimer(startHour, startMinute, endHour, endMinute, action,
                condition, serviceClass, number, listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService setCallForwardUncondTimer : " + e);
        }
    }

    public void getCallForwardUncondTimer(int reason, int serviceClass,
            IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.getCallForwardUncondTimer(reason, serviceClass, listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService getCallForwardUncondTimer : " + e);
        }
    }

    public void getPacketCount(IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.getPacketCount(listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService getPacketCount : " + e);
        }
    }

    public void getPacketErrorCount(IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.getPacketErrorCount(listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService getPacketErrorCount : " + e);
        }
    }

    public void sendCallDeflectRequest(int phoneId, String deflectNumber,
            IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.sendCallDeflectRequest(phoneId, deflectNumber, listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService sendCallDeflectRequestCount : " + e);
        }
    }

    public void resumePendingCall(int videoState) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.resumePendingCall(videoState);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService resumePendingCall : " + e);
        }
    }

    public void sendCallTransferRequest(int phoneId, int type, String number,
            IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.sendCallTransferRequest(phoneId, type, number, listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService sendCallTransferRequest : " + e);
        }
    }

    public void queryVopsStatus(IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.queryVopsStatus(listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService queryVopsStatus : " + e);
        }
    }

    public void querySsacStatus(IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.querySsacStatus(listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService querySsacStatus : " + e);
        }
    }

    public int getImsPhoneId() throws QtiImsException {
        obtainBinder();
        int imsPhoneId = QtiCallConstants.INVALID_PHONE_ID;
        try {
            imsPhoneId = mQtiImsExt.getImsPhoneId();
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService getImsPhoneId : " + e);
        }
        return imsPhoneId;
    }

    public void registerForViceRefreshInfo(IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.registerForViceRefreshInfo(listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService registerForViceRefreshInfo : " + e);
        }
    }

    public void registerForParticipantStatusInfo(IQtiImsExtListener listener)
            throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.registerForParticipantStatusInfo(listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService registerForParticipantStatusInfo : " + e);
        }
    }

    public void updateVoltePreference(int phoneId, int preference,
            IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.updateVoltePreference(phoneId, preference, listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService updateVoltePreference : " + e);
        }
    }

    public void queryVoltePreference(int phoneId,
            IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.queryVoltePreference(phoneId, listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService queryVoltePreference : " + e);
        }
    }

    public void getHandoverConfig(IQtiImsExtListener listener) throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.getHandoverConfig(listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService getHandoverConfig : " + e);
        }
    }

    public void setHandoverConfig(int hoConfig, IQtiImsExtListener listener)
           throws QtiImsException {
        obtainBinder();
        try {
            mQtiImsExt.setHandoverConfig(hoConfig, listener);
        } catch(RemoteException e) {
            throw new QtiImsException("Remote ImsService setHandoverConfig : " + e);
        }
    }

    /**
     * Check if binder is available, else try to retrieve it from ServiceManager
     * if binder still doesn't exists throw {@link QtiImsException}
     */
    private IQtiImsExt obtainBinder() throws QtiImsException {
        if (mQtiImsExt == null) {
            IBinder b = ServiceManager.getService(SERVICE_ID);
            mQtiImsExt = IQtiImsExt.Stub.asInterface(b);

            if (mQtiImsExt == null) {
                throw new QtiImsException("ImsService is not running");
            }
            return mQtiImsExt;
        }
        return mQtiImsExt;
    }
}
