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

namespace android {
namespace hardware {
namespace fastboot {
namespace V1_0 {
namespace implementation {

Return<void> Fastboot::getPartitionType(const ::android::hardware::hidl_string& partitionName,
                                  getPartitionType_cb _hidl_cb) {
    std::string path = "/dev/block/by-name/" + std::string(partitionName);
    if (access(path.c_str(), F_OK) < 0) {
        _hidl_cb(FileSystemType::RAW, { Status::INVALID_ARGUMENT, "Invalid physical partition" });
    } else {
        // For bluecross devices, all valid physical partitions need to return raw.
        _hidl_cb(FileSystemType::RAW, { Status::SUCCESS, "" });
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
