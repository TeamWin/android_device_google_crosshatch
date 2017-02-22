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

/**
 * The class contains definitions for Qti specific carrier configs.
 */
/**
 * @hide
 */
public class QtiCarrierConfigs {

    /**
     * Private constructor. This class should not be instantiated.
     */
    private QtiCarrierConfigs() {
    }

    /* Flag specifying whether video calls are supported if device is in low battery or not */
    public static final String ALLOW_VIDEO_CALL_IN_LOW_BATTERY = "allow_video_call_in_low_battery";

    /**
     * Flag indicating whether preview video needs to be hidden during
     * video conference call.
     */
    public static final String HIDE_PREVIEW_IN_VT_CONFERENCE =
            "config_hide_preview_in_vt_confcall";

    /* Flag determining whether UI extensions for video calls should be used or not */
    public static final String USE_VIDEO_UI_EXTENSIONS = "video_call_use_ext";

    /* Flag determining whether customized video ui support is required or not */
    public static final String USE_CUSTOM_VIDEO_UI = "use_custom_video_ui";

    /* Flag specifying whether IMS to CS retry should be available for carrier
       false - hard disabled.
       true - then depends on user preference */
    public static final String CONFIG_CS_RETRY = "config_carrier_cs_retry_available";

    /* Controls modify call capabilities
       FALSE - default capabilities will be retained
       TRUE - remove modify call capabilities which will hide modify call button*/
    public static final String REMOVE_MODIFY_CALL_CAPABILITY = "remove_modify_call_capability";

    /* Config to show/hide Video quality toast */
    public static final String SHOW_VIDEO_QUALITY_TOAST = "show_video_quality_toast";
    /* Config to show/hide call session event toast like player start/stop */
    public static final String SHOW_CALL_SESSION_EVENT_TOAST = "show_call_session_event_toast";
    /* Config to show/hide data usage toast */
    public static final String SHOW_DATA_USAGE_TOAST = "show_data_usage_toast";
    /* Config that controls whether to stop video preview video and show static image
       in video preview or not
       true - enabled, false - disabled */
    public static final String TRANSMIT_STATIC_IMAGE = "transmit_static_image";
}
