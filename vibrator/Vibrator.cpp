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

static constexpr uint32_t WAVEFORM_TICK_EFFECT_INDEX = 2;
static constexpr uint32_t WAVEFORM_TICK_EFFECT_MS = 9;

static constexpr uint32_t WAVEFORM_CLICK_EFFECT_INDEX = 3;
static constexpr uint32_t WAVEFORM_CLICK_EFFECT_MS = 9;

static constexpr uint32_t WAVEFORM_HEAVY_CLICK_EFFECT_INDEX = 4;
static constexpr uint32_t WAVEFORM_HEAVY_CLICK_EFFECT_MS = 9;
static constexpr uint32_t WAVEFORM_STRONG_HEAVY_CLICK_EFFECT_MS = 12;

static constexpr uint32_t WAVEFORM_DOUBLE_CLICK_EFFECT_INDEX = 7;
static constexpr uint32_t WAVEFORM_DOUBLE_CLICK_EFFECT_MS = 130;

static constexpr int8_t MAX_SCALE_INPUT = 112;

static constexpr int8_t MAX_TRIGGER_LATENCY_MS = 5;

Vibrator::Vibrator(std::ofstream&& activate, std::ofstream&& duration, std::ofstream&& effect,
        std::ofstream&& scale) :
    mActivate(std::move(activate)),
    mDuration(std::move(duration)),
    mEffectIndex(std::move(effect)),
    mScale(std::move(scale))
{}

Return<Status> Vibrator::on(uint32_t timeoutMs, uint32_t effectIndex) {
    mEffectIndex << effectIndex << std::endl;
    mDuration << timeoutMs << std::endl;
    mActivate << 1 << std::endl;

    return Status::OK;
}


// Methods from ::android::hardware::vibrator::V1_1::IVibrator follow.
Return<Status> Vibrator::on(uint32_t timeoutMs) {
    return on(timeoutMs, 0);
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
    return (mScale ? true : false);
}

Return<Status> Vibrator::setAmplitude(uint8_t amplitude) {
    int32_t scale = MAX_SCALE_INPUT - std::round((amplitude - 1) / 254.0 * MAX_SCALE_INPUT);

    mScale << scale << std::endl;
    if (!mScale) {
        ALOGE("Failed to set amplitude (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }

    return Status::OK;
}

Return<void> Vibrator::perform(V1_0::Effect effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    return performEffect(static_cast<Effect>(effect), strength, _hidl_cb);
}

Return<void> Vibrator::perform_1_1(V1_1::Effect_1_1 effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    return performEffect(static_cast<Effect>(effect), strength, _hidl_cb);
}

Return<void> Vibrator::perform_1_2(Effect effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    return performEffect(effect, strength, _hidl_cb);
}

Return<void> Vibrator::performEffect(Effect effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    Status status = Status::OK;
    uint32_t timeMs;
    uint32_t effectIndex;

    switch (effect) {
    case Effect::TICK:
        effectIndex = WAVEFORM_TICK_EFFECT_INDEX;
        timeMs = WAVEFORM_TICK_EFFECT_MS;
        break;
    case Effect::CLICK:
        effectIndex = WAVEFORM_CLICK_EFFECT_INDEX;
        timeMs = WAVEFORM_CLICK_EFFECT_MS;
        break;
    case Effect::HEAVY_CLICK:
        effectIndex = WAVEFORM_HEAVY_CLICK_EFFECT_INDEX;
        if (strength == EffectStrength::STRONG) {
            timeMs = WAVEFORM_STRONG_HEAVY_CLICK_EFFECT_MS;
        } else {
            timeMs = WAVEFORM_HEAVY_CLICK_EFFECT_MS;
        }
        break;
    case Effect::DOUBLE_CLICK:
        effectIndex = WAVEFORM_DOUBLE_CLICK_EFFECT_INDEX;
        timeMs = WAVEFORM_DOUBLE_CLICK_EFFECT_MS;
        break;
    default:
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        return Void();
    }

    switch (strength) {
    case EffectStrength::LIGHT:
        effectIndex -= 1;
        break;
    case EffectStrength::MEDIUM:
        break;
    case EffectStrength::STRONG:
        effectIndex += 1;
        break;
    default:
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        return Void();
    }

    timeMs += MAX_TRIGGER_LATENCY_MS; // Add expected cold-start latency

    setAmplitude(UINT8_MAX); // Always set full-scale for pre-defined constants
    on(timeMs, effectIndex);
    _hidl_cb(status, timeMs);

    return Void();
}


} // namespace implementation
}  // namespace V1_2
}  // namespace vibrator
}  // namespace hardware
}  // namespace android
