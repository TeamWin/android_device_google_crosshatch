/*
 ** Copyright 2018, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

#include <unistd.h>

#define LOG_TAG "wait_for_strongbox"
#include <android-base/logging.h>

#include <keymasterV4_1/Keymaster.h>

using android::hardware::keymaster::V4_1::SecurityLevel;
using android::hardware::keymaster::V4_1::support::Keymaster;

useconds_t kWaitTimeMicroseconds = 1 * 1000;  // 1 milliseconds

int main() {
    for (unsigned cycleCount = 0; /* Forever */; ++cycleCount) {
        auto keymasters = Keymaster::enumerateAvailableDevices();

        bool foundStrongBox = false;
        bool foundTee = false;
        for (auto &dev : keymasters) {
            SecurityLevel securityLevel = dev->halVersion().securityLevel;
            uint8_t majorVersion = dev->halVersion().majorVersion;
            if (securityLevel == SecurityLevel::STRONGBOX && majorVersion == 4) {
                foundStrongBox = true;
            }
            if (securityLevel == SecurityLevel::TRUSTED_ENVIRONMENT && majorVersion == 4) {
                foundTee = true;
            }
        }

        if (foundTee && foundStrongBox) {
            return 0;
        }
        if (cycleCount % 10 == 1) {
            if (!foundStrongBox) {
                LOG(WARNING) << "Still waiting for StrongBox Keymaster";
            }
            if (!foundTee) {
                LOG(WARNING) << "Still waiting for TEE Keymaster";
            }
        }
        usleep(kWaitTimeMicroseconds);
    }
}
