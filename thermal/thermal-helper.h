/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include <ftw.h>
#include <fnmatch.h>

#include <android/hardware/thermal/1.0/IThermal.h>
#include "utils/cooling_devices.h"
#include "utils/sensors.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

using ::android::hardware::hidl_vec;
using ::android::hardware::thermal::V1_0::CpuUsage;
using ::android::hardware::thermal::V1_0::Temperature;
using ::android::hardware::thermal::V1_0::TemperatureType;

constexpr char kSkinSensorType[] = "quiet-therm-adc";
constexpr float kMultiplier = .001;

struct ThrottlingThresholds {
    ThrottlingThresholds() : cpu(NAN), gpu(NAN), ss(NAN), battery(NAN) {}
    float cpu;
    float gpu;
    float ss;
    float battery;
};

class ThermalHelper {
 public:
    ThermalHelper();
    ~ThermalHelper() = default;

    bool fillTemperatures(hidl_vec<Temperature>* temperatures);
    bool fillCpuUsages(hidl_vec<CpuUsage>* cpu_usages);

    // Dissallow copy and assign.
    ThermalHelper(const ThermalHelper&) = delete;
    void operator=(const ThermalHelper&) = delete;

    bool isInitializedOk() const { return is_initialized_; }

    // Checks the throttling data after seeing a modification. This function
    // will store the new throttling number into the throttling level map, decide
    // whether we should notify for throttling, and set the is_throttling and
    // temperature params used to call notifyThrottling(). Returns true if we
    // need to notify for throttling and sets the notify params accordingly. If
    // not the function returns false and leaves notify_params untouched.
    bool checkThrottlingData(
        const std::pair<std::string, std::string>& throttling_data,
        std::pair<bool, Temperature>* notify_params);

    // Returns a vector of all cooling devices that has been found on the
    // device.
    std::vector<std::string> getCoolingDevicePaths();

    // Read the temperature of a single sensor.
    bool readTemperature(
        const std::string& sensor_name, Temperature* out) const;

    // Read the value of a single cooling device.
    bool readCoolingDevice(const std::string& cooling_device, int* data) const;

    const std::map<std::string, std::string>& getValidCoolingDeviceMap() const;

 private:
    bool initializeSensorMap();
    bool initializeCoolingDevices();

    // Read the throttling levels from
    // cooling_device_path_to_throttling_level_map_ and return the maximum level
    // of throttling.
    int getMaxThrottlingLevelFromMap() const;

    Sensors thermal_sensors_;
    CoolingDevices cooling_devices_;

    std::unordered_map<std::string, int>
        cooling_device_path_to_throttling_level_map_;
    ThrottlingThresholds thresholds_;
    ThrottlingThresholds vr_thresholds_;
    ThrottlingThresholds shutdown_thresholds_;
    const bool is_initialized_;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // __THERMAL_HELPER_H__

