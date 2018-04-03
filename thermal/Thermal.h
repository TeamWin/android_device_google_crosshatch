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
#ifndef ANDROID_HARDWARE_THERMAL_V1_1_CROSSHATCH_THERMAL_H
#define ANDROID_HARDWARE_THERMAL_V1_1_CROSSHATCH_THERMAL_H

#include <android/hardware/thermal/1.0/IThermal.h>
#include <hidl/Status.h>
#include <hidl/MQDescriptor.h>
#include "thermal-helper.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

using ::android::hardware::thermal::V1_0::IThermal;
using ::android::hardware::Return;

class Thermal : public IThermal {
 public:
    Thermal() : thermal_helper() {}

    // Disallow copy and assign.
    Thermal(const Thermal&) = delete;
    void operator= (const Thermal&) = delete;

    // Methods from ::android::hardware::thermal::V1_0::IThermal.
    Return<void> getTemperatures(getTemperatures_cb _hidl_cb) override;
    Return<void> getCpuUsages(getCpuUsages_cb _hidl_cb) override;
    Return<void> getCoolingDevices(getCoolingDevices_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& args) override;
 private:
    ThermalHelper thermal_helper;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif
