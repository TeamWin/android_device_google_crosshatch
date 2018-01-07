/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * *    * Redistributions of source code must retain the above copyright
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

#ifndef __THERMAL_HELPER_H__
#define __THERMAL_HELPER_H__

#include <string>
#include <unordered_map>
#include <vector>

#include <ftw.h>
#include <fnmatch.h>

#include <android/hardware/thermal/1.0/IThermal.h>
#include "sensors.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

using ::android::hardware::thermal::V1_0::CpuUsage;
using ::android::hardware::thermal::V1_0::Temperature;

namespace {

constexpr unsigned int kMaxCpus = 8;
// 1 temp sensor for each CPU + 2 GPU, battery, and skin sensor.
constexpr unsigned int kMaxTempSensors = 12;

struct ThrottlingThresholds {
    ThrottlingThresholds() : cpu(NAN), gpu(NAN), ss(NAN), battery(NAN) {}
    float cpu;
    float gpu;
    float ss;
    float battery;
};

}  // namespace

class ThermalHelper {
 public:
    ThermalHelper();

    bool fillTemperatures(hidl_vec<Temperature>* temperatures);
    bool fillCpuUsages(hidl_vec<CpuUsage>* cpu_usages);

    // Dissallow copy and assign.
    ThermalHelper(const ThermalHelper&) = delete;
    void operator=(const ThermalHelper&) = delete;

    bool isInitializedOk() const { return is_initialized_; }

 private:
    // Assign thresholds by reading the config files.
    void initializeThresholds(
        const std::string& thermal_config,
        const std::string& vr_thermal_config);

    bool initializeSensorMap();

    // Read the temperature of a single sensor.
    bool readTemperature(
        const std::string& sensor_name, Temperature* out);

    Sensors thermal_sensors;

    bool is_initialized_;
    ThrottlingThresholds thresholds_;
    ThrottlingThresholds vr_thresholds_;
    ThrottlingThresholds shutdown_thresholds_;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif
