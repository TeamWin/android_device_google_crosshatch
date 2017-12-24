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

#include <cerrno>

#include <android-base/logging.h>

#include "Thermal.h"
#include "thermal-helper.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

namespace {

using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::android::hardware::thermal::V1_0::ThermalStatusCode;
using ::android::hardware::Void;

template <typename T, typename U>
Return<void> setFailureAndCallback(
    T _hidl_cb, hidl_vec<U> data, const std::string& debug_msg) {
    ThermalStatus status;
    status.code = ThermalStatusCode::FAILURE;
    status.debugMessage = debug_msg;
    _hidl_cb(status, data);
    return Void();
}

template <typename T, typename U>
Return<void> setInitFailureAndCallback(T _hidl_cb, hidl_vec<U> data) {
    return setFailureAndCallback(
        _hidl_cb, data, "Failure initializing thermal HAL");
}

}  // namespace

// Methods from ::android::hardware::thermal::V1_0::IThermal.
Return<void> Thermal::getTemperatures(getTemperatures_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<Temperature> temperatures;
    temperatures.resize(kMaxTempSensors);

    if (!thermal_helper.isInitializedOk()) {
        LOG(ERROR) << "ThermalHAL not initialized properly.";
        return setInitFailureAndCallback(_hidl_cb, temperatures);
    }

    ssize_t res = thermal_helper.fillTemperatures(&temperatures);
    if (res < 0) {
        return setFailureAndCallback(_hidl_cb, temperatures, strerror(-res));
    }

    for (const auto& t : temperatures) {
        LOG(DEBUG) << "getTemperatures "
                   << " Type: " << static_cast<int>(t.type)
                   << " Name: " << t.name
                   << " CurrentValue: " << t.currentValue
                   << " ThrottlingThreshold: " << t.throttlingThreshold
                   << " ShutdownThreshold: " << t.shutdownThreshold
                   << " VrThrottlingThreshold: " << t.vrThrottlingThreshold;
    }
    _hidl_cb(status, temperatures);
    return Void();
}

Return<void> Thermal::getCpuUsages(getCpuUsages_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<CpuUsage> cpu_usages;
    cpu_usages.resize(kMaxCpus);

    if (!thermal_helper.isInitializedOk()) {
        return setInitFailureAndCallback(_hidl_cb, cpu_usages);
    }

    ssize_t res = thermal_helper.fillCpuUsages(&cpu_usages);
    if (res < 0) {
        return setFailureAndCallback(_hidl_cb, cpu_usages, strerror(-res));
    }

    for (const auto& usage : cpu_usages) {
        LOG(DEBUG) << "getCpuUsages "
                   << " Name: " << usage.name
                   << " Active: " << usage.active
                   << " Total: " << usage.total
                   << " IsOnline: " << usage.isOnline;
    }
    _hidl_cb(status, cpu_usages);
    return Void();
}

Return<void> Thermal::getCoolingDevices(getCoolingDevices_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<CoolingDevice> cooling_devices;

    if (!thermal_helper.isInitializedOk()) {
        return setInitFailureAndCallback(_hidl_cb, cooling_devices);
    }
    LOG(DEBUG) << "No Cooling Devices";
    _hidl_cb(status, cooling_devices);
    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android
