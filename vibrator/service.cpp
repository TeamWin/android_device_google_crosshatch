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
#define LOG_TAG "android.hardware.vibrator@1.2-service.crosshatch"

#include <android/hardware/vibrator/1.2/IVibrator.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>
#include <utils/StrongPointer.h>

#include "Vibrator.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::vibrator::V1_2::IVibrator;
using android::hardware::vibrator::V1_2::implementation::Vibrator;
using namespace android;

static constexpr char ACTIVATE_PATH[] = "/sys/class/leds/vibrator/activate";
static constexpr char DURATION_PATH[] = "/sys/class/leds/vibrator/duration";
static constexpr char STATE_PATH[] = "/sys/class/leds/vibrator/state";

status_t registerVibratorService() {
    // ostreams below are required
    std::ofstream activate{ACTIVATE_PATH};
    if (!activate) {
        ALOGE("Failed to open %s (%d): %s", ACTIVATE_PATH, errno, strerror(errno));
    }

    std::ofstream duration{DURATION_PATH};
    if (!duration) {
        ALOGE("Failed to open %s (%d): %s", DURATION_PATH, errno, strerror(errno));
    }

    std::ofstream state{STATE_PATH};
    if (!state) {
        ALOGE("Failed to open %s (%d): %s", STATE_PATH, errno, strerror(errno));
    }

    state << 1 << std::endl;
    if (!state) {
        ALOGE("Failed to set state (%d): %s", errno, strerror(errno));
    }

    sp<IVibrator> vibrator = new Vibrator(std::move(activate), std::move(duration));

    return vibrator->registerAsService();
}

int main() {
    configureRpcThreadpool(1, true);
    status_t status = registerVibratorService();

    if (status != OK) {
        return status;
    }

    joinRpcThreadpool();
}
