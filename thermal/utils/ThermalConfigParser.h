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

#ifndef __THERMAL_CONFIG_PARSER_H__
#define __THERMAL_CONFIG_PARSER_H__

#include "thermal-helper.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

void InitializeThresholdsFromThermalConfig(
    const std::string& thermal_config,
    const std::string& vr_thermal_config,
    const std::map<std::string, SensorInfo>& typeMap,
    ThrottlingThresholds *thresholds,
    ThrottlingThresholds *shutdown_thresholds,
    ThrottlingThresholds *vr_thresholds);

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif
