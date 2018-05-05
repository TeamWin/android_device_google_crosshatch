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

#include <algorithm>

#include <android-base/file.h>
#include <android-base/strings.h>
#include "sensors.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

std::string Sensors::getSensorPath(const std::string& sensor_name) {
    if (sensor_names_to_path_map_.find(sensor_name) !=
            sensor_names_to_path_map_.end()) {
        return sensor_names_to_path_map_.at(sensor_name);
    }
    return "";
}

bool Sensors::addSensor(
        const std::string& sensor_name, const std::string& path) {
    return sensor_names_to_path_map_.emplace(sensor_name, path).second;
}

bool Sensors::readSensorFile(
        const std::string& sensor_name, std::string* data,
        std::string* file_path) const {
    std::string sensor_reading;
    if (sensor_names_to_path_map_.find(sensor_name) ==
            sensor_names_to_path_map_.end()) {
        *data = "";
        *file_path = "";
        return false;
    }

    android::base::ReadFileToString(
        sensor_names_to_path_map_.at(sensor_name), &sensor_reading);
    // Strip the newline.
    *data = ::android::base::Trim(sensor_reading);
    *file_path = sensor_names_to_path_map_.at(sensor_name);
    return true;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android
