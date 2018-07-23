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

#define LOG_TAG "dhealth"

#include "DeviceHealth.h"

#include <android-base/properties.h>
#include <cutils/klog.h>

namespace device {
namespace google {
namespace crosshatch {
namespace health {

DeviceHealth::DeviceHealth() {
    is_user_build_ = android::base::GetProperty("ro.build.type", "") == "user";
}

void DeviceHealth::update(struct android::BatteryProperties *props) {
    if (!is_user_build_ &&
        android::base::GetProperty("persist.vendor.disable.thermal.control", "") == "1") {
        props->batteryTemperature = 200;
    }
}

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device
