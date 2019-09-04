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

static constexpr uint32_t WAVEFORM_RINGTONE_EFFECT_INDEX = 65534;
static constexpr uint32_t WAVEFORM_RINGTONE_EFFECT_MS = 30000;

// The_big_adventure - RINGTONE_1
static constexpr char WAVEFORM_RINGTONE1_EFFECT_QUEUE[] = "160, 11.100, 1600, 1!";

// Copycat - RINGTONE_2
static constexpr char WAVEFORM_RINGTONE2_EFFECT_QUEUE[] = "260, 12.100, 660, 2!";

// Crackle - RINGTONE_3
static constexpr char WAVEFORM_RINGTONE3_EFFECT_QUEUE[] = "404, 13.100, 1440, 5!";

// Flutterby - RINGTONE_4
static constexpr char WAVEFORM_RINGTONE4_EFFECT_QUEUE[] = "140, 14.100, 6!";

// Hotline - RINGTONE_5
static constexpr char WAVEFORM_RINGTONE5_EFFECT_QUEUE[] = "140, 15.100, 4!";

// Leaps_and_bounds - RINGTONE_6
static constexpr char WAVEFORM_RINGTONE6_EFFECT_QUEUE[] = "140, 16.100, 1!";

// Lollipop - RINGTONE_7
static constexpr char WAVEFORM_RINGTONE7_EFFECT_QUEUE[] = "140, 17.100, 624, 1!";

// Lost_and_found - RINGTONE_8
static constexpr char WAVEFORM_RINGTONE8_EFFECT_QUEUE[] = "140, 18.100, 1020,496, 1!";

// Mash_up - RINGTONE_9
static constexpr char WAVEFORM_RINGTONE9_EFFECT_QUEUE[] = "140, 19.100, 8, 3!";

// Monkey_around - RINGTONE_10
static constexpr char WAVEFORM_RINGTONE10_EFFECT_QUEUE[] = "20.100, 23.100, 23.80, 23.60, 892, 4!";

// Schools_out - RINGTONE_11
static constexpr char WAVEFORM_RINGTONE11_EFFECT_QUEUE[] = "21.60, 21.80, 21.100, 1020, 564, 6!";

// Zen_too - RINGTONE_12
static constexpr char WAVEFORM_RINGTONE12_EFFECT_QUEUE[] = "140, 22.100, 972, 1!";

static constexpr float AMP_ATTENUATE_STEP_SIZE = 0.125f;

static constexpr int8_t MAX_TRIGGER_LATENCY_MS = 6;

static uint8_t amplitudeToScale(uint8_t amplitude, uint8_t maximum) {
    return std::round((-20 * std::log10(amplitude / static_cast<float>(maximum))) /
                      (AMP_ATTENUATE_STEP_SIZE));
}

Vibrator::Vibrator(std::ofstream&& activate, std::ofstream&& duration, std::ofstream&& effect,
        std::ofstream&& queue, std::ofstream&& scale) :
    mActivate(std::move(activate)),
    mDuration(std::move(duration)),
    mEffectIndex(std::move(effect)),
    mEffectQueue(std::move(queue)),
    mScale(std::move(scale))
{}

Return<Status> Vibrator::on(uint32_t timeoutMs, uint32_t effectIndex) {
    mEffectIndex << effectIndex << std::endl;
    mDuration << (timeoutMs + MAX_TRIGGER_LATENCY_MS) << std::endl;
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

    if (!amplitude) {
        return Status::BAD_VALUE;
    }

    int32_t scale = amplitudeToScale(amplitude, UINT8_MAX);

    mScale << scale << std::endl;
    if (!mScale) {
        ALOGE("Failed to set amplitude (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }

    return Status::OK;
}

Return<void> Vibrator::perform(V1_0::Effect effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    return performWrapper(effect, strength, _hidl_cb);
}

Return<void> Vibrator::perform_1_1(V1_1::Effect_1_1 effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    return performWrapper(effect, strength, _hidl_cb);
}

Return<void> Vibrator::perform_1_2(Effect effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    return performWrapper(effect, strength, _hidl_cb);
}

template <typename T>
Return<void> Vibrator::performWrapper(T effect, EffectStrength strength, perform_cb _hidl_cb) {
    auto validRange = hidl_enum_range<T>();
    if (effect < *validRange.begin() || effect > *std::prev(validRange.end())) {
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        return Void();
    }
    return performEffect(static_cast<Effect>(effect), strength, _hidl_cb);
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
    case Effect::RINGTONE_1:
        mEffectQueue << WAVEFORM_RINGTONE1_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_2:
        mEffectQueue << WAVEFORM_RINGTONE2_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_3:
        mEffectQueue << WAVEFORM_RINGTONE3_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_4:
        mEffectQueue << WAVEFORM_RINGTONE4_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_5:
        mEffectQueue << WAVEFORM_RINGTONE5_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_6:
        mEffectQueue << WAVEFORM_RINGTONE6_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_7:
        mEffectQueue << WAVEFORM_RINGTONE7_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_8:
        mEffectQueue << WAVEFORM_RINGTONE8_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_9:
        mEffectQueue << WAVEFORM_RINGTONE9_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_10:
        mEffectQueue << WAVEFORM_RINGTONE10_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_11:
        mEffectQueue << WAVEFORM_RINGTONE11_EFFECT_QUEUE << std::endl;
        break;
    case Effect::RINGTONE_12:
        mEffectQueue << WAVEFORM_RINGTONE12_EFFECT_QUEUE << std::endl;
        break;
    default:
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        return Void();
    }

    // EffectStrength needs to be handled differently for ringtone effects
    if (effect >= Effect::RINGTONE_1 && effect <= Effect::RINGTONE_15) {
        effectIndex = WAVEFORM_RINGTONE_EFFECT_INDEX;
        timeMs = WAVEFORM_RINGTONE_EFFECT_MS;
        switch (strength) {
        case EffectStrength::LIGHT:
            setAmplitude(UINT8_MAX / 3);
            break;
        case EffectStrength::MEDIUM:
            setAmplitude(UINT8_MAX / 2);
            break;
        case EffectStrength::STRONG:
            setAmplitude(UINT8_MAX);
            break;
        default:
            _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
            return Void();
        }
    } else {
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
        setAmplitude(UINT8_MAX); // Always set full-scale for non-ringtone constants
    }

    on(timeMs, effectIndex);
    _hidl_cb(status, timeMs);

    return Void();
}


} // namespace implementation
}  // namespace V1_2
}  // namespace vibrator
}  // namespace hardware
}  // namespace android
