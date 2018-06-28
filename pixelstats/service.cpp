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

#define LOG_TAG "pixelstats"

#include <android-base/logging.h>
#include <utils/StrongPointer.h>

#include "DropDetect.h"
#include "SysfsCollector.h"
#include "UeventListener.h"

using android::sp;
using namespace device::google::crosshatch;

int main() {
    LOG(INFO) << "starting PixelStats";

    if (!DropDetect::start()) {
        LOG(ERROR) << "Unable to launch drop detection";
        return 1;
    }

    UeventListener::ListenForeverInNewThread();

    SysfsCollector collector;
    collector.collect();  // This blocks forever.

    return 0;
}
