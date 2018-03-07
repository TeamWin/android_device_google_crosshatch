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

#define LOG_TAG "VibratorService"

#include <log/log.h>

#include <hardware/hardware.h>
#include <hardware/vibrator.h>
#include <cutils/properties.h>

#include "Vibrator.h"

#include <cinttypes>
#include <cmath>
#include <iostream>
#include <fstream>


namespace android {
namespace hardware {
namespace vibrator {
namespace V1_2 {
namespace implementation {

using Status = ::android::hardware::vibrator::V1_0::Status;
using EffectStrength = ::android::hardware::vibrator::V1_0::EffectStrength;

Vibrator::Vibrator(std::ofstream&& activate, std::ofstream&& duration) :
    mActivate(std::move(activate)),
    mDuration(std::move(duration))
{}

// Methods from ::android::hardware::vibrator::V1_1::IVibrator follow.
Return<Status> Vibrator::on(uint32_t timeoutMs) {
    mDuration << timeoutMs << std::endl;
    mActivate << 1 << std::endl;

    return Status::OK;
}

Return<Status> Vibrator::off()  {
    mActivate << 0 << std::endl;
    if (!mActivate) {
        ALOGE("Failed to turn vibrator off (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
    return Status::OK;
}

Return<bool> Vibrator::supportsAmplitudeControl()  {
    return false;
}

Return<Status> Vibrator::setAmplitude(uint8_t) {
    return Status::UNSUPPORTED_OPERATION;
}

Return<void> Vibrator::perform(V1_0::Effect, EffectStrength, perform_cb _hidl_cb) {
    _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
    return Void();
}

Return<void> Vibrator::perform_1_1(V1_1::Effect_1_1, EffectStrength,
        perform_cb _hidl_cb) {
    _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
    return Void();
}

Return<void> Vibrator::perform_1_2(Effect, EffectStrength, perform_cb _hidl_cb) {
    _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
    return Void();
}

} // namespace implementation
}  // namespace V1_2
}  // namespace vibrator
}  // namespace hardware
}  // namespace android
