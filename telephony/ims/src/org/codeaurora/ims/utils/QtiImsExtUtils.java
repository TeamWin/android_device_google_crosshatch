/**
 * Copyright (c) 2015,2016 The Linux Foundation. All rights reserved.
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

package org.codeaurora.ims.utils;

import android.content.ContentResolver;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.PersistableBundle;
import android.os.SystemProperties;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.util.Log;

import java.io.File;

import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.QtiCarrierConfigs;
import org.codeaurora.ims.QtiImsException;
import org.codeaurora.ims.QtiImsExtManager;

/**
 * This class contains QtiImsExt specific utiltity functions.
 */
public class QtiImsExtUtils {

    private static String LOG_TAG = "QtiImsExtUtils";

    public static final String QTI_IMS_CALL_DEFLECT_NUMBER =
            "ims_call_deflect_number";

    /* Call deflect setting name */
    public static final String QTI_IMS_DEFLECT_ENABLED = "qti.ims.call_deflect";

    /* Default success value */
    public static final int QTI_IMS_REQUEST_SUCCESS = 0;

    /* Default error value */
    public static final int QTI_IMS_REQUEST_ERROR = 1;

    /* name for carrier property */
    public static final String PROPERTY_RADIO_ATEL_CARRIER = "persist.radio.atel.carrier";

    /* Carrier one default mcc mnc */
    public static final String CARRIER_ONE_DEFAULT_MCC_MNC = "405854";

    public static final String QTI_IMS_STATIC_IMAGE_SETTING =
            "ims_vt_call_static_image";

    /**
     * Definitions for the call transfer type. For easier implementation,
     * the transfer type is defined as a bit mask value.
     */
    //Value representing blind call transfer type
    public static final int QTI_IMS_BLIND_TRANSFER = 0x01;
    //Value representing assured call transfer type
    public static final int QTI_IMS_ASSURED_TRANSFER = 0x02;
    //Value representing consultative call transfer type
    public static final int QTI_IMS_CONSULTATIVE_TRANSFER = 0x04;

    /* Call transfer extra key */
    public static final String QTI_IMS_TRANSFER_EXTRA_KEY = "transferType";

    /* Constants used for VOPS and SSAC feature */
    // Intent action
    public static final String ACTION_VOPS_SSAC_STATUS =
            "org.codeaurora.VOIP_VOPS_SSAC_STATUS";
    /* Intent extra
     * if true  : Voice is supported on LTE
     * if false : Voice is not supported on LTE
     */
    public static final String EXTRA_VOPS = "Vops";
    /* Intent extra
     * if true  : Access barring factor for voice calls is 0
     * if false : Access barring factor for voice calls is non-zero
     *            Range: 0 to 100. Value 100 is used when
     *            the UE goes to the LTE Connected state
     */
    public static final String EXTRA_SSAC = "Ssac";

    /**
     * Definitions for the volte preference values.
     */
    //Value representing volte preference is OFF
    public static final int QTI_IMS_VOLTE_PREF_OFF = 0;
    //Value representing volte preference is ON
    public static final int QTI_IMS_VOLTE_PREF_ON = 1;
    //Value representing volte preference is NOT known
    public static final int QTI_IMS_VOLTE_PREF_UNKNOWN = 2;


    /* Incoming conference call extra key */
    public static final String QTI_IMS_INCOMING_CONF_EXTRA_KEY = "incomingConference";

    /* Handover config params */
    public static final int QTI_IMS_HO_INVALID = 0x00;
    public static final int QTI_IMS_HO_ENABLE_ALL = 0x01;
    public static final int QTI_IMS_HO_DISABLE_ALL = 0x02;
    public static final int QTI_IMS_HO_ENABLED_WLAN_TO_WWAN_ONLY = 0x03;
    public static final int QTI_IMS_HO_ENABLED_WWAN_TO_WLAN_ONLY = 0x04;

    /**
     * Private constructor for QtiImsExtUtils as we don't want to instantiate this class
     */
    private QtiImsExtUtils() {
    }

    /**
     * Retrieves the call deflection stored by the user
     * Returns stored number, or null otherwise.
     */
    public static String getCallDeflectNumber(ContentResolver contentResolver) {
        String deflectcall = android.provider.Settings.Global.getString(contentResolver,
                                     QTI_IMS_CALL_DEFLECT_NUMBER);

        /* Consider being null or empty as "Not Set" */
        if ((deflectcall != null) && (deflectcall.isEmpty())) {
            deflectcall = null;
        }

        return deflectcall;
    }

    /* Stores the call deflection provided by the user */
    public static void setCallDeflectNumber(ContentResolver contentResolver, String value) {
        String deflectNum = value;

        if (value == null || value.isEmpty()) {
            deflectNum = "";
        }

        android.provider.Settings.Global.putString(contentResolver,
                QTI_IMS_CALL_DEFLECT_NUMBER, deflectNum);
    }

   /**
     * Retrieves the static image stored by the user
     * Returns stored static image file path, or null otherwise.
     */
    public static String getStaticImageUriStr(ContentResolver contentResolver) {
        return android.provider.Settings.Global.getString(contentResolver,
                                     QTI_IMS_STATIC_IMAGE_SETTING);
    }

    private static boolean isValidUriStr(String uri) {
        /* uri is not valid if
         * 1. uri is null
         * 2. uri is empty
         * 3. uri doesn't exist in UE
         */
        return uri != null && !uri.isEmpty() && (new File(uri)).exists();
    }

    /**
     * Calculate an inSampleSize for use in a {@link android.graphics.BitmapFactory.Options} object
     * when decoding bitmaps using the decode* methods from {@link android.graphics.BitmapFactory}.
     * This implementation calculates the closest inSampleSize that is a power of 2 and will result
     * in the final decoded bitmap having a width and height equal to or larger than the requested
     * width and height.
     *
     * @param options An options object with out* params already populated (run through a decode*
     *            method with inJustDecodeBounds==true
     * @param reqWidth The requested width of the resulting bitmap
     * @param reqHeight The requested height of the resulting bitmap
     * @return The value to be used for inSampleSize
     */
    private static int calculateInSampleSize(
            BitmapFactory.Options options, int reqWidth, int reqHeight) {
        // Raw height and width of image
        final int height = options.outHeight;
        final int width = options.outWidth;
        int inSampleSize = 1;

        Log.d(LOG_TAG, "calculateInSampleSize: reqWidth = " + reqWidth + " reqHeight = " + reqHeight
                + " raw width = " + width + " raw height = " + height);

        if (height > reqHeight || width > reqWidth) {
            final int halfHeight = height / 2;
            final int halfWidth = width / 2;

            // Calculate the largest inSampleSize value that is a power of 2 and keeps both
            // height and width larger than the requested height and width.
            while ((halfHeight / inSampleSize) > reqHeight
                    && (halfWidth / inSampleSize) > reqWidth) {
                inSampleSize *= 2;
            }
        }
        Log.d(LOG_TAG, "calculateInSampleSize: inSampleSize = " + inSampleSize);
        return inSampleSize;
    }

    /**
     * Decodes an image pointed to by uri as per requested Width and requested Height
     * and returns a bitmap
     */
    public static Bitmap decodeImage(String uri, int reqWidth, int reqHeight) {
        if (uri == null) {
            return null;
        }

        BitmapFactory.Options options = new BitmapFactory.Options();
        // Each pixel is stored on 4 bytes
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        /* If set to true, the decoder will return null (no bitmap),
           but the out... fields (i.e. outWidth, outHeight and outMimeType)
           will still be set, allowing the caller to query the bitmap
           without having to allocate the memory for its pixels */
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(uri, options);

        // Calculate inSampleSize
        options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight);

        // Decode bitmap with inSampleSize set
        options.inJustDecodeBounds = false;
        Bitmap bitmap = BitmapFactory.decodeFile(uri, options);
        return scaleImage(bitmap, reqWidth, reqHeight);
    }

    // scales the image using reqWidth/reqHeight and returns a scaled bitmap
    private static Bitmap scaleImage(Bitmap bitmap, int reqWidth, int reqHeight) {
        if (bitmap == null) {
            return null;
        }

        int w = bitmap.getWidth();
        int h = bitmap.getHeight();
        float scaleWidth = ((float) reqWidth) / w;
        float scaleHeight = ((float) reqHeight) / h;
        Log.d(LOG_TAG, "scaleImage bitmap w = " + w + " bitmap h = " + h);

        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleHeight);
        Bitmap resizedBitmap = Bitmap.createBitmap(
               bitmap, 0, 0, w, h, matrix, false);
        return resizedBitmap;
    }

    /**
     * Decode and sample down a bitmap from a resource to the requested width and height.
     *
     * @param res The resources containing the resId
     * @param resId The resource id
     * @param reqWidth The requested width of the resulting bitmap
     * @param reqHeight The requested height of the resulting bitmap
     * @return A down sampled bitmap
     */
    public static Bitmap decodeImage(Resources res, int resId,
            int reqWidth, int reqHeight) {

        // First decode with inJustDecodeBounds=true to check dimensions
        final BitmapFactory.Options options = new BitmapFactory.Options();
        // Each pixel is stored on 4 bytes
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        /* If set to true, the decoder will return null (no bitmap),
           but the out... fields (i.e. outWidth, outHeight and outMimeType)
           will still be set, allowing the caller to query the bitmap
           without having to allocate the memory for its pixels */
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeResource(res, resId, options);

        // Calculate inSampleSize
        options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight);

        // Decode bitmap with inSampleSize set
        options.inJustDecodeBounds = false;
        Bitmap bitmap = BitmapFactory.decodeResource(res, resId, options);
        return scaleImage(bitmap, reqWidth, reqHeight);
    }

    /*
     * This API:
     * 1. Checks to see if user has preconfigured any image
     * 2. Throws an INVALID_FILE_PATH exception if image file path is invalid
     * 3. Throws an IMAGE_DECODING_FAILURE exception if image decoding failed
     * 4. Returns a downsampled bitmap in all other cases.
     */
    public static Bitmap getStaticImage(Context context, int reqWidth, int reqHeight)
            throws QtiImsException {
        // Query database for user selected image
        String uriStr = getStaticImageUriStr(context.getContentResolver());
        Log.d(LOG_TAG, "getStaticImage: uriStr = " + uriStr + " reqWidth = " + reqWidth +
                " reqHeight = " + reqHeight);

        if (!isValidUriStr(uriStr)) {
            throw new QtiImsException("invalid file path");
        }

        Bitmap imageBitmap = decodeImage(uriStr, reqWidth, reqHeight);
        if (imageBitmap == null) {
            throw new QtiImsException("image decoding error");
        }
        return imageBitmap;
    }

    /***
     * Checks if the IMS Video call hide me feature is enabled or not.
     * Returns true if enabled, or false otherwise.
     */
    public static boolean shallTransmitStaticImage(Context context) {
        return isCarrierConfigEnabled(context, QtiCarrierConfigs.TRANSMIT_STATIC_IMAGE);

    }

    /***
     * Checks if the IMS call transfer property is enabled or not.
     * Returns true if enabled, or false otherwise.
     */
    public static boolean isCallTransferEnabled(Context context) {
        return SystemProperties.getBoolean("persist.radio.ims_call_transfer", false);
    }

   /**
     * This API checks to see whether we are going to use ui extension for video call or not.
     * @param context context for getting video call ui ext configuration value
     * Returns true if enabled, or false otherwise.
     */
    public static boolean useExt(Context context) {
        return isCarrierConfigEnabled(context, QtiCarrierConfigs.USE_VIDEO_UI_EXTENSIONS);
    }

   /**
     * This API checks to see whether custom video ui is enabled or not.
     * @param context context for getting custom video ui configuration value
     * Returns true if enabled, or false otherwise.
     */
    public static boolean useCustomVideoUi(Context context) {
        return isCarrierConfigEnabled(context, QtiCarrierConfigs.USE_CUSTOM_VIDEO_UI);
    }

    /**
     * This API checks to see whether IMS to CS retry is enabled or not.
     * @param context context for getting the CS retry configuration value
     * Returns true if enabled, or false otherwise.
     */
    public static boolean isCsRetryConfigEnabled(Context context) {
        return isCarrierConfigEnabled(context, QtiCarrierConfigs.CONFIG_CS_RETRY);
    }

    /**
     * Check is carrier one supported or not
     */
    public static boolean isCarrierOneSupported() {
        return CARRIER_ONE_DEFAULT_MCC_MNC.equals(SystemProperties.get(
                PROPERTY_RADIO_ATEL_CARRIER));
    }

    /**
     * Returns true if config flag is enabled.
     */
    public static boolean isCarrierConfigEnabled(Context context, String carrierConfig) {

        PersistableBundle b = getConfigForDefaultImsPhoneId(context);

        if (b == null) {
            Log.e(LOG_TAG, "isCarrierConfigEnabled bundle is null");
            return false;
        }

        return b.getBoolean(carrierConfig, false);
    }

    public static boolean allowVideoCallsInLowBattery(Context context) {
        return isCarrierConfigEnabled(context, QtiCarrierConfigs.ALLOW_VIDEO_CALL_IN_LOW_BATTERY);
    }

    public static boolean shallHidePreviewInVtConference(Context context) {
        return isCarrierConfigEnabled(context,
                QtiCarrierConfigs.HIDE_PREVIEW_IN_VT_CONFERENCE);
    }

    public static boolean shallRemoveModifyCallCapability(Context context) {
        return isCarrierConfigEnabled(context, QtiCarrierConfigs.REMOVE_MODIFY_CALL_CAPABILITY);
    }

    private static PersistableBundle getConfigForDefaultImsPhoneId(Context context) {
        return getConfigForPhoneId(context, getImsPhoneId());
    }

    private static PersistableBundle getConfigForPhoneId(Context context, int phoneId) {
        if (context == null) {
            Log.e(LOG_TAG, "getConfigForPhoneId context is null");
            return null;
        }

        CarrierConfigManager configManager = (CarrierConfigManager) context.getSystemService(
                Context.CARRIER_CONFIG_SERVICE);
        if (configManager == null) {
            Log.e(LOG_TAG, "getConfigForPhoneId configManager is null");
            return null;
        }

        if (phoneId == QtiCallConstants.INVALID_PHONE_ID) {
            Log.e(LOG_TAG, "getConfigForPhoneId phoneId is invalid");
            return null;
        }

        int subId = getSubscriptionIdFromPhoneId(context, phoneId);
        if (!SubscriptionManager.isValidSubscriptionId(subId)) {
            Log.e(LOG_TAG, "getConfigForPhoneId subId is invalid");
            return null;
        }

        return configManager.getConfigForSubId(subId);
    }

    /**
     * Returns IMS phone id.
     */
    private static int getImsPhoneId() {
        int phoneId = QtiCallConstants.INVALID_PHONE_ID;
        try {
            phoneId = QtiImsExtManager.getInstance().getImsPhoneId();
        } catch (QtiImsException e) {
            Log.e(LOG_TAG, "getImsPhoneId failed. Exception = " + e);
        }
        return phoneId;
    }

    /**
     * Returns subscription id for given phone id.
     */
    private static int getSubscriptionIdFromPhoneId(Context context, int phoneId) {
        SubscriptionManager subscriptionManager = SubscriptionManager.from(context);
        if (subscriptionManager == null) {
            return subscriptionManager.INVALID_SUBSCRIPTION_ID;
        }

        SubscriptionInfo subInfo = subscriptionManager.
                getActiveSubscriptionInfoForSimSlotIndex(phoneId);
        if (subInfo == null) {
            return subscriptionManager.INVALID_SUBSCRIPTION_ID;
        }
        return subInfo.getSubscriptionId();
    }
}
