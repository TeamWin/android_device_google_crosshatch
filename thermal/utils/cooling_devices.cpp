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
#include <fstream>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>

#include "cooling_devices.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

constexpr char kCoolingDeviceCurStateSuffix[] = "cur_state";

bool CoolingDevices::addCoolingDevice(
        const std::string& cooling_device_name, const std::string& path) {
    return cooling_device_name_to_path_map_.emplace(
        cooling_device_name, path).second;
}

bool CoolingDevices::getCoolingDeviceState(
        const std::string& cooling_device_name, int* data) const {
    auto cooling_device_itr = cooling_device_name_to_path_map_.find(
        cooling_device_name);
    if (cooling_device_itr == cooling_device_name_to_path_map_.end()) {
        return false;
    }

    std::string path = android::base::StringPrintf(
        "%s/%s", cooling_device_itr->second.c_str(),
        kCoolingDeviceCurStateSuffix);

    std::string cooling_device_data;
    android::base::ReadFileToString(path, &cooling_device_data);
    cooling_device_data = android::base::Trim(cooling_device_data);

    if (cooling_device_data.empty()) {
        LOG(ERROR) << "Could not read "
                   << getCoolingDevicePath(cooling_device_name);
        return false;
    }

    *data = std::stoi(cooling_device_data);
    return true;
}

std::string CoolingDevices::getCoolingDevicePath(
        const std::string& cooling_device_name) const {
    auto cooling_device_itr = cooling_device_name_to_path_map_.find(
        cooling_device_name);
    if (cooling_device_itr == cooling_device_name_to_path_map_.end()) {
        return "";
    }
    return cooling_device_itr->second;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android
