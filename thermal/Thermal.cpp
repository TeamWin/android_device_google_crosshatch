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

#include <cerrno>
#include <mutex>
#include <string>

#include <android-base/file.h>
#include <android-base/logging.h>

#include "Thermal.h"
#include "thermal-helper.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

namespace {

using ::android::hardware::hidl_death_recipient;
using ::android::hardware::thermal::V1_0::CoolingDevice;
using ::android::hardware::thermal::V1_0::Temperature;
using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::android::hardware::thermal::V1_0::ThermalStatusCode;
using ::android::hardware::Void;
using ::android::hidl::base::V1_0::IBase;
using ::android::wp;

sp<IThermalCallback> gThermalCallback;
std::mutex gThermalCallbackMutex;

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

// This function will hold gThermalCallbackMutex when called.
void checkThermalCallbackAndNotify(
        const std::pair<bool, Temperature>& notify_params) {
    std::lock_guard<std::mutex> _lock(gThermalCallbackMutex);
    if (gThermalCallback) {
        Return<void> ret = gThermalCallback->notifyThrottling(
            notify_params.first, notify_params.second);

        if (!ret.isOk()) {
            if (ret.isDeadObject()) {
                gThermalCallback = nullptr;
                LOG(WARNING) << "ThermalCallback died. "
                             << "Throttling event dropped.";
            } else {
                LOG(WARNING)
                    << "Failed to send throttling event to "
                    << "ThermalCallback";
            }
        }
    } else {
        LOG(WARNING)
            << "No ThermalCallback registered. "
            << "Throttling event dropped.";
    }
}

}  // namespace

void Thermal::serviceDied(
        uint64_t cookie __unused, const wp<IBase>& who __unused) {
    std::lock_guard<std::mutex> _lock(gThermalCallbackMutex);
    gThermalCallback = nullptr;
    LOG(ERROR) << "IThermalCallback HIDL service died";
}

// On init we will spawn a thread which will continually watch for
// throttling.  When throttling is seen, if we have a callback registered
// the thread will call notifyThrottling() else it will log the dropped
// throttling event and do nothing.  The thread is only killed when
// Thermal() is killed.
Thermal::Thermal() : thermal_helper_(), cpu_throttling_watcher_() {
    cpu_throttling_watcher_.registerFilesToWatch(
        thermal_helper_.getCoolingDevicePaths());
    cpu_throttling_watcher_.registerCallback(
        std::bind(&Thermal::notifyIfThrottlingSeen, this,
                  std::placeholders::_1));
    cpu_throttling_watcher_.registerQueueOverflowCallback(
        std::bind(&Thermal::resetStateWhenWatcherQueueOverflows, this));
    cpu_throttling_watcher_.startWatchingDeviceFiles();
}

// Methods from ::android::hardware::thermal::V1_0::IThermal.
Return<void> Thermal::getTemperatures(getTemperatures_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    hidl_vec<Temperature> temperatures;

    if (!thermal_helper_.isInitializedOk()) {
        LOG(ERROR) << "ThermalHAL not initialized properly.";
        return setInitFailureAndCallback(_hidl_cb, temperatures);
    }

    if (!thermal_helper_.fillTemperatures(&temperatures)) {
        return setFailureAndCallback(_hidl_cb, temperatures,
                "Failed to read thermal sensors.");
    }

    for (const auto& t : temperatures) {
        LOG(DEBUG) << "getTemperatures "
                   << " Type: " << android::hardware::thermal::V1_0::toString(t.type)
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

    if (!thermal_helper_.isInitializedOk()) {
        return setInitFailureAndCallback(_hidl_cb, cpu_usages);
    }

    if (!thermal_helper_.fillCpuUsages(&cpu_usages)) {
        return setFailureAndCallback(_hidl_cb, cpu_usages,
            "Failed to get CPU usages.");
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

    if (!thermal_helper_.isInitializedOk()) {
        return setInitFailureAndCallback(_hidl_cb, cooling_devices);
    }
    LOG(DEBUG) << "No cooling device.";
    _hidl_cb(status, cooling_devices);
    return Void();
}

Return<void> Thermal::debug(
        const hidl_handle& handle, const hidl_vec<hidl_string>&) {
    if (handle != nullptr && handle->numFds >= 1) {
        int fd = handle->data[0];
        std::ostringstream dump_buf;

        if (!thermal_helper_.isInitializedOk()) {
            dump_buf << "ThermalHAL not initialized properly." << std::endl;
        } else {
            hidl_vec<Temperature> temperatures;
            hidl_vec<CpuUsage> cpu_usages;

            dump_buf << "getTemperatures:" << std::endl;
            if (!thermal_helper_.fillTemperatures(&temperatures)) {
                dump_buf << "Failed to read thermal sensors." << std::endl;
            }

            for (const auto& t : temperatures) {
                dump_buf << "Name: " << t.name << " Type: "
                         << android::hardware::thermal::V1_0::toString(t.type)
                         << " CurrentValue: " << t.currentValue
                         << " ThrottlingThreshold: " << t.throttlingThreshold
                         << " ShutdownThreshold: " << t.shutdownThreshold
                         << " VrThrottlingThreshold: "
                         << t.vrThrottlingThreshold << std::endl;
            }

            dump_buf << "getCpuUsages:" << std::endl;
            if (!thermal_helper_.fillCpuUsages(&cpu_usages)) {
                dump_buf << "Failed to get CPU usages." << std::endl;
            }

            for (const auto& usage : cpu_usages) {
                dump_buf << "Name: " << usage.name
                         << " Active: " << usage.active
                         << " Total: " << usage.total
                         << " IsOnline: " << usage.isOnline
                         << std::endl;
            }

        }
        std::string buf = dump_buf.str();
        if (!android::base::WriteStringToFd(buf, fd)) {
            PLOG(ERROR) << "Failed to dump state to fd";
        }
        fsync(fd);
    }
    return Void();
}

// Methods from ::android::hardware::thermal::V1_1::IThermal.
Return<void> Thermal::registerThermalCallback(const sp<IThermalCallback>& cb) {
    std::lock_guard<std::mutex> _lock(gThermalCallbackMutex);
    if (gThermalCallback) {
        LOG(WARNING) << "Thermal callback was already assigned!";
    }

    auto cpu_throttling_watcher_status =
        cpu_throttling_watcher_.getWatcherThreadStatus();
    if (cpu_throttling_watcher_status == std::future_status::ready) {
        LOG(ERROR) << "The CPU throttling watcher thread stopped running. "
                   << "Restarting it.";
        cpu_throttling_watcher_.startWatchingDeviceFiles();
    }

    gThermalCallback = cb;
    if (gThermalCallback) {
        gThermalCallback->linkToDeath(this, 0x451F /* cookie, unused */);
        LOG(INFO) << "ThermalCallback registered.";
    } else {
        LOG(INFO) << "ThermalCallback unregistered.";
    }
    return Void();
}

void Thermal::notifyIfThrottlingSeen(
        const std::pair<std::string, std::string>& throttling_data) {
    std::pair<bool, Temperature> notify_params;
    bool shouldNotify = thermal_helper_.checkThrottlingData(
        throttling_data, &notify_params);

    if (shouldNotify) {
        checkThermalCallbackAndNotify(notify_params);
    }
}

void Thermal::resetStateWhenWatcherQueueOverflows() {
    int throttling_value;
    for (const auto& entry : thermal_helper_.getValidCoolingDeviceMap()) {
        if (thermal_helper_.readCoolingDevice(entry.first, &throttling_value)) {
            notifyIfThrottlingSeen(std::make_pair(
                entry.first, std::to_string(throttling_value)));
        }
    }
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android
