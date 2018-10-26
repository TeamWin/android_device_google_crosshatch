/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "pixelstats-uevent"

#include "UeventListener.h"

#include <android-base/logging.h>
#include <android-base/parseint.h>
#include <android-base/strings.h>
#include <cutils/uevent.h>
#include <hardware/google/pixelstats/1.0/IPixelStats.h>
#include <log/log.h>
#include <utils/StrongPointer.h>

#include <unistd.h>
#include <thread>

namespace device {
namespace google {
namespace crosshatch {

constexpr int32_t UEVENT_MSG_LEN = 2048;  // it's 2048 in all other users.

// Report connection & disconnection of devices into the USB-C connector.
void UeventListener::ReportUsbConnectorUevents(const char *power_supply_typec_mode) {
    using ::hardware::google::pixelstats::V1_0::IPixelStats;
    if (!power_supply_typec_mode) {
        // No mode reported -> No reporting.
        return;
    }

    // It's attached if the string *doesn't* match.
    int attached = !!strcmp(power_supply_typec_mode, "POWER_SUPPLY_TYPEC_MODE=Nothing attached");
    if (attached == is_usb_attached_) {
        return;
    }
    is_usb_attached_ = attached;

    android::sp<IPixelStats> client = IPixelStats::tryGetService();
    if (!client) {
        ALOGE("Unable to connect to PixelStats service");
        return;
    }

    if (attached) {
        client->reportUsbConnectorConnected();
        usb_connect_time_ = android::base::Timer();
    } else {
        client->reportUsbConnectorDisconnected(usb_connect_time_.duration().count());
    }
}

// Report connection & disconnection of USB audio devices.
void UeventListener::ReportUsbAudioUevents(const char *driver, const char *product,
                                           const char *action) {
    // driver is not provided on remove, so it can be NULL.  Check in remove branch.
    if (!product || !action) {
        return;
    }

    // The PRODUCT of a USB audio device is PRODUCT=VID/PID/VERSION
    std::vector<std::string> halves = android::base::Split(product, "=");
    if (halves.size() != 2) {
        return;
    }
    std::vector<std::string> vidPidVer = android::base::Split(halves[1], "/");
    if (vidPidVer.size() != 3) {
        return;
    }

    // Parse the VID/PID as hex values.
    const int kBaseHex = 16;
    int32_t vid, pid;
    char *vidEnd = NULL, *pidEnd = NULL;

    const char *vidC = vidPidVer[0].c_str();
    vid = strtol(vidC, &vidEnd, kBaseHex);
    if ((vidC == vidEnd) || !vidEnd || (*vidEnd != '\0')) {
        return;
    }

    const char *pidC = vidPidVer[1].c_str();
    pid = strtol(pidC, &pidEnd, kBaseHex);
    if ((pidC == pidEnd) || !pidEnd || (*pidEnd != '\0')) {
        return;
    }

    /* A uevent is generated for each audio interface - only report the first connected one
     * for each device by storing its PRODUCT= string in attached_product_, and clearing
     * it on disconnect.  This also means we will only report the first USB audio device attached
     * to the system. Only attempt to connect to the HAL when reporting an event.
     */
    using ::hardware::google::pixelstats::V1_0::IPixelStats;
    if (!attached_product_ && !strcmp(action, "ACTION=add")) {
        if (!driver || strcmp(driver, "DRIVER=snd-usb-audio")) {
            return;
        }
        usb_audio_connect_time_ = android::base::Timer();
        attached_product_ = strdup(product);

        android::sp<IPixelStats> client = IPixelStats::tryGetService();
        if (!client) {
            ALOGE("Couldn't connect to PixelStats service for audio connect");
            return;
        }
        client->reportUsbAudioConnected(vid, pid);
    } else if (attached_product_ && !strcmp(action, "ACTION=remove")) {
        if (strcmp(attached_product_, product)) {
            // Not the expected product detaching.
            return;
        }
        free(attached_product_);
        attached_product_ = NULL;

        android::sp<IPixelStats> client = IPixelStats::tryGetService();
        if (!client) {
            ALOGE("Couldn't connect to PixelStats service for audio disconnect");
            return;
        }
        client->reportUsbAudioDisconnected(vid, pid, usb_audio_connect_time_.duration().count());
    }
}

void UeventListener::ReportMicBroken(const char *devpath, const char *mic_break_status) {
    if (!devpath || !mic_break_status)
        return;
    if (!strcmp(devpath, ("DEVPATH=" + kAudioUevent).c_str()) &&
        !strcmp(mic_break_status, "MIC_BREAK_STATUS=true")) {
        using ::hardware::google::pixelstats::V1_0::IPixelStats;
        android::sp<IPixelStats> client = IPixelStats::tryGetService();
        if (!client) {
            ALOGE("Couldn't connect to PixelStats service for mic break");
            return;
        }
        client->reportHardwareFailed(IPixelStats::HardwareType::MICROPHONE, 0,
                                     IPixelStats::HardwareErrorCode::COMPLETE);
    }
}

bool UeventListener::ProcessUevent() {
    char msg[UEVENT_MSG_LEN + 2];
    char *cp;
    const char *action, *power_supply_typec_mode, *driver, *product;
    const char *mic_break_status;
    const char *devpath;
    int n;

    if (uevent_fd_ < 0) {
        uevent_fd_ = uevent_open_socket(64 * 1024, true);
        if (uevent_fd_ < 0) {
            ALOGE("uevent_init: uevent_open_socket failed\n");
            return false;
        }
    }

    n = uevent_kernel_multicast_recv(uevent_fd_, msg, UEVENT_MSG_LEN);
    if (n <= 0 || n >= UEVENT_MSG_LEN)
        return false;

    // Ensure double-null termination of msg.
    msg[n] = '\0';
    msg[n + 1] = '\0';

    action = power_supply_typec_mode = driver = product = NULL;
    mic_break_status = devpath = NULL;

    /**
     * msg is a sequence of null-terminated strings.
     * Iterate through and record positions of string/value pairs of interest.
     * Double null indicates end of the message. (enforced above).
     */
    cp = msg;
    while (*cp) {
        if (!strncmp(cp, "ACTION=", strlen("ACTION="))) {
            action = cp;
        } else if (!strncmp(cp, "POWER_SUPPLY_TYPEC_MODE=", strlen("POWER_SUPPLY_TYPEC_MODE="))) {
            power_supply_typec_mode = cp;
        } else if (!strncmp(cp, "DRIVER=", strlen("DRIVER="))) {
            driver = cp;
        } else if (!strncmp(cp, "PRODUCT=", strlen("PRODUCT="))) {
            product = cp;
        } else if (!strncmp(cp, "MIC_BREAK_STATUS=", strlen("MIC_BREAK_STATUS="))) {
            mic_break_status = cp;
        } else if (!strncmp(cp, "DEVPATH=", strlen("DEVPATH="))) {
            devpath = cp;
        }

        /* advance to after the next \0 */
        while (*cp++) {
        }
    }

    /* Process the strings recorded. */
    ReportUsbConnectorUevents(power_supply_typec_mode);
    ReportUsbAudioUevents(driver, product, action);
    ReportMicBroken(devpath, mic_break_status);

    return true;
}

UeventListener::UeventListener(const std::string audio_uevent)
    : kAudioUevent(audio_uevent),
      uevent_fd_(-1),
      is_usb_attached_(false),
      attached_product_(nullptr) {}

/* Thread function to continuously monitor uevents.
 * Exit after kMaxConsecutiveErrors to prevent spinning. */
void UeventListener::ListenForever() {
    constexpr int kMaxConsecutiveErrors = 10;
    int consecutive_errors = 0;
    while (1) {
        if (ProcessUevent()) {
            consecutive_errors = 0;
        } else {
            if (++consecutive_errors >= kMaxConsecutiveErrors) {
                ALOGE("Too many ProcessUevent errors; exiting UeventListener.");
                return;
            }
        }
    }
}

}  // namespace crosshatch
}  // namespace google
}  // namespace device
