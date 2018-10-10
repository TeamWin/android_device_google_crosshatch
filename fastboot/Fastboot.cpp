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

#include "fastboot/Fastboot.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <android-base/unique_fd.h>

namespace android {
namespace hardware {
namespace fastboot {
namespace V1_0 {
namespace implementation {

constexpr const char* BRIGHTNESS_FILE = "/sys/class/backlight/panel0-backlight/brightness";
constexpr int DISPLAY_BRIGHTNESS_DIM_THRESHOLD = 20;

using  OEMCommandHandler = std::function<Result(const std::vector<std::string>&)>;

Return<void> Fastboot::getPartitionType(const ::android::hardware::hidl_string& /* partitionName */,
                                  getPartitionType_cb _hidl_cb) {
    // For bluecross devices, all partitions need to return raw.
    _hidl_cb(FileSystemType::RAW, { Status::SUCCESS, "" });
    return Void();
}

Return<void> Fastboot::getVariant(getVariant_cb _hidl_cb) {
    _hidl_cb("MSM USF", {Status::SUCCESS, "" });
    return Void();
}

Return<void> Fastboot::getOffModeChargeState(getOffModeChargeState_cb _hidl_cb) {
    constexpr const char* kDevinfoPath = "/dev/block/by-name/devinfo";
    constexpr int kDevInfoOffModeChargeOffset = 15;

    uint8_t off_mode_charge_status = 0;
    android::base::unique_fd fd(TEMP_FAILURE_RETRY(open(kDevinfoPath,
                                                        O_RDONLY | O_BINARY)));
    if (!android::base::ReadFullyAtOffset(fd, &off_mode_charge_status, 1 /* byte count */,
                                          kDevInfoOffModeChargeOffset)) {
        _hidl_cb(false,
                 { Status::FAILURE_UNKNOWN, "Unable to read off-mode-charge state" });
    } else {
        _hidl_cb(off_mode_charge_status != 0, { Status::SUCCESS, "" });
    }

    return Void();
}

Return<void> Fastboot::getBatteryVoltageFlashingThreshold(
        getBatteryVoltageFlashingThreshold_cb _hidl_cb) {
    constexpr int kMinVoltageForFlashing = 3500;
    _hidl_cb(kMinVoltageForFlashing, { Status::SUCCESS, "" });
    return Void();
}

Result SetBrightnessLevel(const std::vector<std::string>& args) {
    if (!args.size()) {
        return { Status::INVALID_ARGUMENT, "Brightness level unspecified" };
    }

    auto level = std::stoi(args[0]);

    if (level < 0 || level > 100)  {
        return { Status::INVALID_ARGUMENT, "Brighness level must be between 0 and 100" };
    }

    // Avoid screen being dimmed too much.
    if (level < DISPLAY_BRIGHTNESS_DIM_THRESHOLD) {
        level = DISPLAY_BRIGHTNESS_DIM_THRESHOLD;
    }

    if (android::base::WriteStringToFile(std::to_string(level), BRIGHTNESS_FILE)) {
            return { Status::SUCCESS, "" };
    }

    return { Status::FAILURE_UNKNOWN, "Unable to set display brightness" };
}

Return<void> Fastboot::doOemCommand(const ::android::hardware::hidl_string& oemCmdArgs,
                          doOemCommand_cb _hidl_cb) {
    const std::unordered_map<std::string, OEMCommandHandler> kOEMCmdMap = {
        {FB_OEM_SET_BRIGHTNESS, SetBrightnessLevel},
    };

    auto args = android::base::Split(oemCmdArgs, " ");
    if (args.size() < 2) {
        _hidl_cb({ Status::INVALID_ARGUMENT, "Invalid OEM command" });
        return Void();
    }

    // args[0] will be "oem", args[1] will be the command name
    auto cmd_handler = kOEMCmdMap.find(args[1]);
    if (cmd_handler != kOEMCmdMap.end()) {
        _hidl_cb(cmd_handler->second(std::vector<std::string>(args.begin() + 2, args.end())));
    } else {
        _hidl_cb({ Status::FAILURE_UNKNOWN, "Unknown OEM command" });
    }

    return Void();
}

Fastboot::Fastboot() {}

// Methods from ::android::hidl::base::V1_0::IBase follow.

extern "C" IFastboot* HIDL_FETCH_IFastboot(const char* /* name */) {
    return new Fastboot();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace fastboot
}  // namespace hardware
}  // namespace android
