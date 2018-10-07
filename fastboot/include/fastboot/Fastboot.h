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

#pragma once

#include <android/hardware/fastboot/1.0/IFastboot.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android {
namespace hardware {
namespace fastboot {
namespace V1_0 {
namespace implementation {

#define FB_OEM_SET_BRIGHTNESS "setbrightness"

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct Fastboot : public IFastboot {
    Fastboot();
    // Methods from ::android::hardware::fastboot::V1_0::IFastboot follow.
    Return<void> getPartitionType(const ::android::hardware::hidl_string& partitionName,
                                  getPartitionType_cb _hidl_cb) override;

    Return<void> doOemCommand(const ::android::hardware::hidl_string& oemCmd,
                              doOemCommand_cb _hidl_cb) override;
    Return<void> getVariant(getVariant_cb _hidl_cb) override;
    Return<void> getOffModeChargeState(getOffModeChargeState_cb _hidl_cb) override;
    Return<void> getBatteryVoltageFlashingThreshold(getBatteryVoltageFlashingThreshold_cb _hidl_cb) override;
};

extern "C" IFastboot* HIDL_FETCH_IFastboot(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace fastboot
}  // namespace hardware
}  // namespace android
