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

static constexpr char ACTIVATE_PATH[] = "/sys/class/leds/vibrator/activate";
static constexpr char DURATION_PATH[] = "/sys/class/leds/vibrator/duration";
static constexpr char STATE_PATH[] = "/sys/class/leds/vibrator/state";
static constexpr char EFFECT_INDEX_PATH[] = "/sys/class/leds/vibrator/device/cp_trigger_index";
static constexpr char EFFECT_QUEUE_PATH[] = "/sys/class/leds/vibrator/device/cp_trigger_queue";
static constexpr char DIGI_SCALE_PATH[] = "/sys/class/leds/vibrator/device/dig_scale";

// File path to the calibration file
static constexpr char CALIBRATION_FILEPATH[] = "/persist/haptics/cs40l20.cal";

// Kernel ABIs for updating the calibration data
static constexpr char F0_CONFIG[] = "f0_measured";
static constexpr char REDC_CONFIG[] = "redc_measured";
static constexpr char F0_FILEPATH[] = "/sys/class/leds/vibrator/device/f0_stored";
static constexpr char REDC_FILEPATH[] = "/sys/class/leds/vibrator/device/redc_stored";

static std::string trim(const std::string &str, const std::string &whitespace = " \t") {
    const auto str_begin = str.find_first_not_of(whitespace);
    if (str_begin == std::string::npos) {
        return "";
    }

    const auto str_end = str.find_last_not_of(whitespace);
    const auto str_range = str_end - str_begin + 1;

    return str.substr(str_begin, str_range);
}

static bool loadCalibrationData() {
    std::map<std::string, std::string> config_data;

    std::ofstream f0{F0_FILEPATH};
    if (!f0) {
        ALOGE("Failed to open %s (%d): %s", F0_FILEPATH, errno, strerror(errno));
    }

    std::ofstream redc{REDC_FILEPATH};
    if (!redc) {
        ALOGE("Failed to open %s (%d): %s", REDC_FILEPATH, errno, strerror(errno));
    }

    std::ifstream cal_data{CALIBRATION_FILEPATH};
    if (!cal_data) {
        ALOGE("Failed to open %s (%d): %s", CALIBRATION_FILEPATH, errno, strerror(errno));
        return false;
    }

    for (std::string line; std::getline(cal_data, line);) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, ':')) {
            std::string value;

            if (std::getline(is_line, value)) {
                config_data[trim(key)] = trim(value);
            }
        }
    }

    if (config_data.find(F0_CONFIG) != config_data.end()) {
        f0 << config_data[F0_CONFIG] << std::endl;
    }

    if (config_data.find(REDC_CONFIG) != config_data.end()) {
        redc << config_data[REDC_CONFIG] << std::endl;
    }

    return true;
}

// passing out ownership, can't make sp yet
IVibrator *makeVibratorService() {
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

    std::ofstream effect{EFFECT_INDEX_PATH};
    if (!state) {
        ALOGE("Failed to open %s (%d): %s", EFFECT_INDEX_PATH, errno, strerror(errno));
    }

    std::ofstream queue{EFFECT_QUEUE_PATH};
    if (!state) {
        ALOGE("Failed to open %s (%d): %s", EFFECT_QUEUE_PATH, errno, strerror(errno));
    }

    std::ofstream scale{DIGI_SCALE_PATH};
    if (!scale) {
        ALOGE("Failed to open %s (%d): %s", DIGI_SCALE_PATH, errno, strerror(errno));
    }

    state << 1 << std::endl;
    if (!state) {
        ALOGE("Failed to set state (%d): %s", errno, strerror(errno));
    }

    if (!loadCalibrationData()) {
        ALOGW("Failed to load calibration data");
    }

    return new Vibrator(std::move(activate), std::move(duration), std::move(effect),
                        std::move(queue), std::move(scale));
}

extern "C" IVibrator *HIDL_FETCH_IVibrator(const char * /*instnace*/) {
    return makeVibratorService();
}
