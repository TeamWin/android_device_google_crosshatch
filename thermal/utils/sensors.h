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

#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <string>
#include <unordered_map>

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

class Sensors {
    public:
     Sensors() = default;
     ~Sensors() = default;
     Sensors(const Sensors&) = delete;
     void operator=(const Sensors&) = delete;

     std::string getSensorPath(const std::string& sensor_name);
     // Returns true if add was successful, false otherwise.
     bool addSensor(const std::string& sensor_name, const std::string& path);
     // If sensor is not found in the sensor names to path map, this will set
     // data and file path to empty and return false.  If the sensor is found,
     // this function will fill in data and file_path accordingly then return
     // true.
     bool readSensorFile(
         const std::string& sensor_name, std::string* data,
         std::string* file_path) const;
     size_t getNumSensors() const { return sensor_names_to_path_map_.size(); }

    private:
     std::unordered_map<std::string, std::string> sensor_names_to_path_map_;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // __SENSORS_H__

