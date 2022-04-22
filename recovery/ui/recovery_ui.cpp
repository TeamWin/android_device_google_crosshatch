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

#include <android-base/endian.h>
#include <android-base/logging.h>

#include <app_nugget.h>
#include <nos/debug.h>
#include <nos/NuggetClient.h>

#include <device.h>
#include <screen_ui.h>

namespace android {
namespace device {
namespace google {
namespace crosshatch {

namespace {

/** Wipe user data from Titan M. */
bool WipeTitanM() {
    // Connect to Titan M
    ::nos::NuggetClient client;
    client.Open();
    if (!client.IsOpen()) {
        LOG(ERROR) << "Failed to connect to Titan M";
        return false;
    }

    // Tell it to wipe user data
    const uint32_t magicValue = htole32(ERASE_CONFIRMATION);
    std::vector<uint8_t> magic(sizeof(magicValue));
    memcpy(magic.data(), &magicValue, sizeof(magicValue));
    const uint32_t status
            = client.CallApp(APP_ID_NUGGET, NUGGET_PARAM_NUKE_FROM_ORBIT, magic, nullptr);
    if (status != APP_SUCCESS) {
        LOG(ERROR) << "Titan M user data wipe failed: " << ::nos::StatusCodeString(status)
                   << " (" << status << ")";
        return false;
    }

    LOG(INFO) << "Titan M wipe successful";
    return true;
}

} // namespace

class crosshatchDevice : public ::Device
{
public:
    crosshatchDevice(::ScreenRecoveryUI* const ui) : ::Device(ui) {}

    /** Hook to wipe user data not stored in /data */
    bool PostWipeData() override {
        // Try to do everything but report a failure if anything wasn't successful
        bool totalSuccess = true;
        ::RecoveryUI* const ui = GetUI();

        ui->Print("Wiping Titan M...\n");
        if (!WipeTitanM()) {
            totalSuccess = false;
        }

        // Extendable to wipe other components

        return totalSuccess;
    }
};

} // namespace crosshatch
} // namespace google
} // namespace device
} // namespace android

Device *make_device()
{
    return new ::android::device::google::crosshatch::crosshatchDevice(new ::ScreenRecoveryUI);
}
