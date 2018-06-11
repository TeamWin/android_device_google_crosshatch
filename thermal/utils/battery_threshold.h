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

#ifndef __BATTERY_THRESHOLD_H__
#define __BATTERY_THRESHOLD_H__

#include <android/hardware/thermal/1.0/IThermal.h>
#include <string>
#include <unordered_map>
#include <utility>

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {
using ::android::hardware::hidl_vec;
using ::android::hardware::thermal::V1_0::CpuUsage;
using ::android::hardware::thermal::V1_0::Temperature;
using ::android::hardware::thermal::V1_0::TemperatureType;

enum class AgeRange;
enum class TempRange;
class BatteryThresholdLUT
{
public:
    BatteryThresholdLUT();
    float adjustThreshold(
        float configured_threshold, float current_temperature) const;
    bool fillBatteryThresholdDebugInfo(std::ostringstream& dump_buf) const;
private:
    bool readCycleCount(int& cycle_count) const;
    bool readSoC(int& soc) const;
    bool readStatus(std::string& status) const;

    struct TempAgeHash
    {
        size_t operator()(const std::pair<TempRange, AgeRange>& key) const;
    };
    const std::unordered_map<std::pair<TempRange, AgeRange>, int, TempAgeHash>
        battery_conditions_lut_;
    const char* cycle_count_filename_;
    const char* state_of_charge_filename_;
    const char* status_filename_;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // __THERMAL_HELPER_H__
