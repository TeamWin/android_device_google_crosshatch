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

#include "SysfsCollector.h"

#define LOG_TAG "pixelstats-vendor"

#include <android-base/file.h>
#include <android-base/parseint.h>
#include <android-base/strings.h>
#include <hardware/google/pixelstats/1.0/IPixelStats.h>
#include <utils/Log.h>
#include <utils/StrongPointer.h>
#include <utils/Timers.h>

#include <sys/timerfd.h>
#include <string>

namespace device {
namespace google {
namespace crosshatch {

using android::sp;
using android::base::ReadFileToString;
using ::hardware::google::pixelstats::V1_0::IPixelStats;

SysfsCollector::SysfsCollector(const struct SysfsPaths &sysfs_paths)
    : kSlowioReadCntPath(sysfs_paths.SlowioReadCntPath),
      kSlowioWriteCntPath(sysfs_paths.SlowioWriteCntPath),
      kSlowioUnmapCntPath(sysfs_paths.SlowioUnmapCntPath),
      kSlowioSyncCntPath(sysfs_paths.SlowioSyncCntPath),
      kCycleCountBinsPath(sysfs_paths.CycleCountBinsPath),
      kImpedancePath(sysfs_paths.ImpedancePath),
      kCodecPath(sysfs_paths.CodecPath) {}

/**
 * Read the contents of kCycleCountBinsPath and report them via IPixelStats HAL.
 * The contents are expected to be N buckets total, the nth of which indicates the
 * number of times battery %-full has been increased with the n/N% full bucket.
 */
void SysfsCollector::logBatteryChargeCycles() {
    std::string file_contents;
    if (strlen(kCycleCountBinsPath) == 0) {
        ALOGV("Battery charge cycle path not specified");
        return;
    }
    if (!ReadFileToString(kCycleCountBinsPath, &file_contents)) {
        ALOGE("Unable to read battery charge cycles %s - %s", kCycleCountBinsPath, strerror(errno));
        return;
    }

    std::replace(file_contents.begin(), file_contents.end(), ' ', ',');
    pixelstats_->reportChargeCycles(android::base::Trim(file_contents));
}

/**
 * Check the codec for failures over the past 24hr.
 */
void SysfsCollector::logCodecFailed() {
    std::string file_contents;
    if (strlen(kCodecPath) == 0) {
        ALOGV("Audio codec path not specified");
        return;
    }
    if (!ReadFileToString(kCodecPath, &file_contents)) {
        ALOGE("Unable to read codec state %s - %s", kCodecPath, strerror(errno));
        return;
    }
    if (file_contents == "0") {
        return;
    } else {
        pixelstats_->reportHardwareFailed(IPixelStats::HardwareType::CODEC, 0,
                                          IPixelStats::HardwareErrorCode::COMPLETE);
    }
}

void SysfsCollector::reportSlowIoFromFile(const char *path,
                                          const IPixelStats::IoOperation &operation) {
    std::string file_contents;
    if (strlen(path) == 0) {
        ALOGV("slow_io path not specified");
        return;
    }
    if (!ReadFileToString(path, &file_contents)) {
        ALOGE("Unable to read slowio %s - %s", path, strerror(errno));
        return;
    } else {
        int32_t slow_io_count = 0;
        if (sscanf(file_contents.c_str(), "%d", &slow_io_count) != 1) {
            ALOGE("Unable to parse %s from file %s to int.", file_contents.c_str(), path);
        } else if (slow_io_count > 0) {
            pixelstats_->reportSlowIo(operation, slow_io_count);
        }
        // Clear the stats
        if (!android::base::WriteStringToFile("0", path, true)) {
            ALOGE("Unable to clear SlowIO entry %s - %s", path, strerror(errno));
        }
    }
}

/**
 * Check for slow IO operations.
 */
void SysfsCollector::logSlowIO() {
    reportSlowIoFromFile(kSlowioReadCntPath, IPixelStats::IoOperation::READ);
    reportSlowIoFromFile(kSlowioWriteCntPath, IPixelStats::IoOperation::WRITE);
    reportSlowIoFromFile(kSlowioUnmapCntPath, IPixelStats::IoOperation::UNMAP);
    reportSlowIoFromFile(kSlowioSyncCntPath, IPixelStats::IoOperation::SYNC);
}

/**
 * Report the last-detected impedance of left & right speakers.
 */
void SysfsCollector::logSpeakerImpedance() {
    std::string file_contents;
    if (strlen(kImpedancePath) == 0) {
        ALOGV("Audio impedance path not specified");
        return;
    }
    if (!ReadFileToString(kImpedancePath, &file_contents)) {
        ALOGE("Unable to read impedance path %s", kImpedancePath);
        return;
    }

    float left, right;
    if (sscanf(file_contents.c_str(), "%g,%g", &left, &right) != 2) {
        ALOGE("Unable to parse speaker impedance %s", file_contents.c_str());
        return;
    }
    pixelstats_->reportSpeakerImpedance(0, left * 1000);
    pixelstats_->reportSpeakerImpedance(1, right * 1000);
}

void SysfsCollector::logAll() {
    pixelstats_ = IPixelStats::tryGetService();
    if (!pixelstats_) {
        ALOGE("Unable to connect to PixelStats service");
        return;
    }

    logBatteryChargeCycles();
    logCodecFailed();
    logSlowIO();
    logSpeakerImpedance();

    pixelstats_.clear();
}

/**
 * Loop forever collecting stats from sysfs nodes and reporting them via
 * IPixelStats.
 */
void SysfsCollector::collect(void) {
    int timerfd = timerfd_create(CLOCK_BOOTTIME, 0);
    if (timerfd < 0) {
        ALOGE("Unable to create timerfd - %s", strerror(errno));
        return;
    }

    // Sleep for 30 seconds on launch to allow codec driver to load.
    sleep(30);

    // Collect first set of stats on boot.
    logAll();

    // Collect stats every 24hrs after.
    struct itimerspec period;
    const int kSecondsPerDay = 60 * 60 * 24;
    period.it_interval.tv_sec = kSecondsPerDay;
    period.it_interval.tv_nsec = 0;
    period.it_value.tv_sec = kSecondsPerDay;
    period.it_value.tv_nsec = 0;

    if (timerfd_settime(timerfd, 0, &period, NULL)) {
        ALOGE("Unable to set 24hr timer - %s", strerror(errno));
        return;
    }

    while (1) {
        int readval;
        do {
            char buf[8];
            errno = 0;
            readval = read(timerfd, buf, sizeof(buf));
        } while (readval < 0 && errno == EINTR);
        if (readval < 0) {
            ALOGE("Timerfd error - %s\n", strerror(errno));
            return;
        }
        logAll();
    }
}

}  // namespace crosshatch
}  // namespace google
}  // namespace device
