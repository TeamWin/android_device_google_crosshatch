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
#define LOG_TAG "android.hardware.health@2.0-service.crosshatch"
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/parseint.h>
#include <android-base/strings.h>
#include <health2/Health.h>
#include <health2/service.h>
#include <healthd/healthd.h>
#include <hidl/HidlTransportSupport.h>
#include <pixelhealth/BatteryDefender.h>
#include <pixelhealth/BatteryMetricsLogger.h>
#include <pixelhealth/BatteryThermalControl.h>
#include <pixelhealth/CycleCountBackupRestore.h>
#include <pixelhealth/DeviceHealth.h>
#include <pixelhealth/LowBatteryShutdownMetrics.h>

#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "BatteryRechargingControl.h"

namespace {

using android::hardware::health::V2_0::DiskStats;
using android::hardware::health::V2_0::StorageAttribute;
using android::hardware::health::V2_0::StorageInfo;
using ::device::google::crosshatch::health::BatteryRechargingControl;
using hardware::google::pixel::health::BatteryDefender;
using hardware::google::pixel::health::BatteryMetricsLogger;
using hardware::google::pixel::health::BatteryThermalControl;
using hardware::google::pixel::health::CycleCountBackupRestore;
using hardware::google::pixel::health::DeviceHealth;
using hardware::google::pixel::health::LowBatteryShutdownMetrics;

constexpr char kBatteryResistance[] = "/sys/class/power_supply/maxfg/resistance";
constexpr char kBatteryOCV[] = "/sys/class/power_supply/maxfg/voltage_ocv";
constexpr char kVoltageAvg[] = "/sys/class/power_supply/maxfg/voltage_avg";

#define WLC_DIR "/sys/class/power_supply/wireless"

static BatteryDefender battDefender(WLC_DIR "/online");
static BatteryRechargingControl battRechargingControl;
static BatteryThermalControl battThermalControl("sys/devices/virtual/thermal/tz-by-name/soc/mode");
static BatteryMetricsLogger battMetricsLogger(kBatteryResistance, kBatteryOCV);
static LowBatteryShutdownMetrics shutdownMetrics(kVoltageAvg);
static CycleCountBackupRestore ccBackupRestoreBMS(
    8, "/sys/class/power_supply/bms/device/cycle_counts_bins",
    "/mnt/vendor/persist/battery/qcom_cycle_counts_bins");
static CycleCountBackupRestore ccBackupRestoreMAX(
    10, "/sys/class/power_supply/maxfg/cycle_counts_bins",
    "/mnt/vendor/persist/battery/max_cycle_counts_bins",
    "/sys/class/power_supply/maxfg/serial_number");
static DeviceHealth deviceHealth;

#define UFS_DIR "/sys/devices/platform/soc/1d84000.ufshc"
const std::string kUfsHealthEol{UFS_DIR "/health/eol"};
const std::string kUfsHealthLifetimeA{UFS_DIR "/health/lifetimeA"};
const std::string kUfsHealthLifetimeB{UFS_DIR "/health/lifetimeB"};
const std::string kUfsVersion{UFS_DIR "/version"};
const std::string kDiskStatsFile{"/sys/block/sda/stat"};
const std::string kUFSName{"UFS0"};

static bool needs_wlc_updates = false;
constexpr char kWlcCapacity[]{WLC_DIR "/capacity"};

std::ifstream assert_open(const std::string &path) {
    std::ifstream stream(path);
    if (!stream.is_open()) {
        LOG(FATAL) << "Cannot read " << path;
    }
    return stream;
}

template <typename T>
void read_value_from_file(const std::string &path, T *field) {
    auto stream = assert_open(path);
    stream.unsetf(std::ios_base::basefield);
    stream >> *field;
}

void read_ufs_version(StorageInfo *info) {
    uint64_t value;
    read_value_from_file(kUfsVersion, &value);
    std::stringstream ss;
    ss << "ufs " << std::hex << value;
    info->version = ss.str();
}

void fill_ufs_storage_attribute(StorageAttribute *attr) {
    attr->isInternal = true;
    attr->isBootDevice = true;
    attr->name = kUFSName;
}

}  // anonymous namespace

static bool FileExists(const std::string &filename) {
    struct stat buffer;

    return stat(filename.c_str(), &buffer) == 0;
}

void healthd_board_init(struct healthd_config *config) {
    using ::device::google::crosshatch::health::kChargerStatus;

    ccBackupRestoreBMS.Restore();
    ccBackupRestoreMAX.Restore();

    config->batteryStatusPath = kChargerStatus.c_str();

    needs_wlc_updates = FileExists(kWlcCapacity);
}

int healthd_board_battery_update(struct android::BatteryProperties *props) {
    battRechargingControl.updateBatteryProperties(props);
    deviceHealth.update(props);
    battThermalControl.updateThermalState(props);
    battMetricsLogger.logBatteryProperties(props);
    shutdownMetrics.logShutdownVoltage(props);
    ccBackupRestoreBMS.Backup(props->batteryLevel);
    ccBackupRestoreMAX.Backup(props->batteryLevel);
    battDefender.update(props);

    if (needs_wlc_updates &&
        !android::base::WriteStringToFile(std::to_string(props->batteryLevel), kWlcCapacity))
        LOG(INFO) << "Unable to write battery level to wireless capacity";

    return 0;
}

void get_storage_info(std::vector<StorageInfo> &vec_storage_info) {
    vec_storage_info.resize(1);
    StorageInfo *storage_info = &vec_storage_info[0];
    fill_ufs_storage_attribute(&storage_info->attr);

    read_ufs_version(storage_info);
    read_value_from_file(kUfsHealthEol, &storage_info->eol);
    read_value_from_file(kUfsHealthLifetimeA, &storage_info->lifetimeA);
    read_value_from_file(kUfsHealthLifetimeB, &storage_info->lifetimeB);
    return;
}

void get_disk_stats(std::vector<DiskStats> &vec_stats) {
    vec_stats.resize(1);
    DiskStats *stats = &vec_stats[0];
    fill_ufs_storage_attribute(&stats->attr);

    auto stream = assert_open(kDiskStatsFile);
    // Regular diskstats entries
    stream >> stats->reads >> stats->readMerges >> stats->readSectors >> stats->readTicks >>
        stats->writes >> stats->writeMerges >> stats->writeSectors >> stats->writeTicks >>
        stats->ioInFlight >> stats->ioTicks >> stats->ioInQueue;
    return;
}

int main(void) {
    return health_service_main();
}
