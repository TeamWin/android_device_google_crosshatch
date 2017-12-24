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

#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <unordered_map>
#include <string>

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

class Sensors {
    public:
     Sensors() = default;
     Sensors(const Sensors&) = delete;
     void operator=(const Sensors&) = delete;

     std::string getSensorPath(const std::string& sensor_name);
     bool addSensor(const std::string& sensor_name, const std::string& path);
     bool readSensorFile(
         const std::string& sensor_name, std::string* data,
         std::string* file_path);
     size_t getNumSensors() const { return sensor_names_to_path_map_.size(); }

    private:
     std::unordered_map<std::string, std::string> sensor_names_to_path_map_;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif
