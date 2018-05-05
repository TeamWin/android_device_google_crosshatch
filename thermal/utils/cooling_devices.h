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

#ifndef __COOLING_DEVICES_H__
#define __COOLING_DEVICES_H__

#include <unordered_map>
#include <string>

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

class CoolingDevices {
    public:
     CoolingDevices() = default;
     ~CoolingDevices() = default;
     CoolingDevices(const CoolingDevices&) = delete;
     void operator=(const CoolingDevices&) = delete;

     // Keep track of the cooling device given the path to its directory.
     // The cooling device names are defined in: thermal-helper.cpp in
     // kValidCoolingDeviceNameTypeMap.  Path is the cooling device directory.
     // This is something like: /sys/devices/virtual/thermal/cooling_device_X.
     // This function assumes that the top level directory at least has a
     // cur_state file in it, which represents the current state of the cooling
     // device. Returns true if add succeeds.  False otherwise.
     bool addCoolingDevice(const std::string& cooling_device_name,
                           const std::string& path);

     // If the cooling device name is in the map this will read the cur_state
     // file within the directory, fill in the data param with the contents of
     // that file, and return true.  Else it will return false and leave data
     // untouched.
     bool getCoolingDeviceState(const std::string& cooling_device_name,
                                int* data) const;

     // Get the file path given the cooling device name. If the cooling device
     // name has not been added to CoolingDevices, this will return an empty
     // string.
     std::string getCoolingDevicePath(
         const std::string& cooling_device_name) const;

     size_t getNumCoolingDevices() const {
         return cooling_device_name_to_path_map_.size();
     }

    private:
     std::unordered_map<std::string, std::string>
         cooling_device_name_to_path_map_;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // __COOLING_DEVICES_H__

