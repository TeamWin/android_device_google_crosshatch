/*
 * Copyright (C) 2018 The Android Open Source Project
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
#include "DropDetect.h"

#include <chre/util/nanoapp/app_id.h>
#include <chre_host/host_protocol_host.h>
#include <chre_host/socket_client.h>

#include <hardware/google/pixelstats/1.0/IPixelStats.h>
#define LOG_TAG "pixelstats-vendor"
#include <log/log.h>

#include <inttypes.h>

using android::sp;
using android::chre::HostProtocolHost;
using android::chre::IChreMessageHandlers;
using android::chre::SocketClient;

// following convention of CHRE code.
namespace fbs = ::chre::fbs;

namespace device {
namespace google {
namespace crosshatch {

namespace {  // anonymous namespace for file-local definitions

// This struct is defined in nanoapps/drop/messaging.h
// by the DropDetect nanoapp.
struct __attribute__((__packed__)) DropEventPayload {
    float confidence;
    float accel_magnitude_peak;
    int32_t free_fall_duration_ns;
};

// This enum is defined in nanoapps/drop/messaging.h
// by the DropDetect nanoapp.
enum DropConstants {
    kDropEnableRequest = 1,
    kDropEnableNotification = 2,
    kDropDisableRequest = 3,
    kDropDisableNotification = 4,
    kDropEventDetection = 5,
};

void requestNanoappList(SocketClient &client) {
    flatbuffers::FlatBufferBuilder builder(64);
    HostProtocolHost::encodeNanoappListRequest(builder);

    if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
        ALOGE("Failed to send NanoappList request");
    }
}

}  // namespace

DropDetect::DropDetect(const uint64_t drop_detect_app_id) : kDropDetectAppId(drop_detect_app_id) {}

sp<DropDetect> DropDetect::start(const uint64_t drop_detect_app_id, const char *const chre_socket) {
    sp<DropDetect> dropDetect = new DropDetect(drop_detect_app_id);
    if (!dropDetect->connectInBackground(chre_socket, dropDetect)) {
        ALOGE("Couldn't connect to CHRE socket");
        return nullptr;
    }
    return dropDetect;
}

void DropDetect::onConnected() {
    requestNanoappList(*this);
}

/**
 * Decode unix socket msgs to CHRE messages, and call the appropriate
 * callback depending on the CHRE message.
 */
void DropDetect::onMessageReceived(const void *data, size_t length) {
    if (!HostProtocolHost::decodeMessageFromChre(data, length, *this)) {
        ALOGE("Failed to decode message");
    }
}

/**
 * Handle the response of a NanoappList request.
 * Ensure that the Drop Detect nanoapp is running.
 */
void DropDetect::handleNanoappListResponse(const fbs::NanoappListResponseT &response) {
    for (const std::unique_ptr<fbs::NanoappListEntryT> &nanoapp : response.nanoapps) {
        if (nanoapp->app_id == kDropDetectAppId) {
            if (!nanoapp->enabled)
                ALOGE("Drop Detect app not enabled");
            else
                ALOGI("Drop Detect enabled");
            return;
        }
    }
    ALOGE("Drop Detect app not found");
}

/**
 * listen for messages from the DropDetect nanoapp and report them to
 * PixelStats.
 */
void DropDetect::handleNanoappMessage(const fbs::NanoappMessageT &message) {
    struct DropEventPayload *message_struct;

    if (message.app_id != kDropDetectAppId || message.message_type != kDropEventDetection ||
        message.message.size() < sizeof(struct DropEventPayload))
        return;
    message_struct = (struct DropEventPayload *)&message.message[0];
    /*
     * ALOGI("Received drop detect message! Confidence %f Peak %f Duration %g",
     *     message_struct->confidence, message_struct->accel_magnitude_peak,
     *     message_struct->free_fall_duration_ns / 1e9);
     */
    int32_t confidence = message_struct->confidence * 100;
    confidence = std::min(confidence, 100);
    confidence = std::max(0, confidence);

    int32_t accel_magnitude_peak_1000ths_g = message_struct->accel_magnitude_peak * 1000.0;
    int32_t free_fall_duration_ms = message_struct->free_fall_duration_ns / 1000000;

    using ::hardware::google::pixelstats::V1_0::IPixelStats;
    sp<IPixelStats> client = IPixelStats::tryGetService();
    if (!client) {
        ALOGE("Unable to connect to PixelStats service");
        return;
    }
    client->reportPhysicalDropDetected(confidence, accel_magnitude_peak_1000ths_g,
                                       free_fall_duration_ms);
}

}  // namespace crosshatch
}  // namespace google
}  // namespace device
