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

#include <sstream>
#include <vector>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>

#include "thermal-helper.h"
#include "utils/ThermalConfigParser.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

constexpr char kThermalSensorsRoot[] = "/sys/devices/virtual/thermal";
constexpr char kCpuOnlineRoot[] = "/sys/devices/system/cpu";
constexpr char kCpuUsageFile[] = "/proc/stat";
constexpr char kCpuOnlineFileSuffix[] = "online";
constexpr char kThermalConfigPrefix[] = "/vendor/etc/thermal-engine-";
constexpr char kTemperatureFileSuffix[] = "temp";
constexpr char kSensorTypeFileSuffix[] = "type";
constexpr char kThermalZoneDirSuffix[] = "thermal_zone";
constexpr char kCoolingDeviceDirSuffix[] = "cooling_device";
constexpr unsigned int kMaxCpus = 8;
constexpr unsigned int kMaxSensorSearchNum = 100;

// This is a golden set of thermal sensor type and their temperature types.
// Used when we read in sensor values.
const std::map<std::string, TemperatureType>
kValidThermalSensorTypeMap = {
    {"cpu0-silver-usr", TemperatureType::CPU},  // CPU0
    {"cpu1-silver-usr", TemperatureType::CPU},  // CPU1
    {"cpu2-silver-usr", TemperatureType::CPU},  // CPU2
    {"cpu3-silver-usr", TemperatureType::CPU},  // CPU3
    {"cpu0-gold-usr", TemperatureType::CPU},    // CPU4
    {"cpu1-gold-usr", TemperatureType::CPU},    // CPU5
    {"cpu2-gold-usr", TemperatureType::CPU},    // CPU6
    {"cpu3-gold-usr", TemperatureType::CPU},    // CPU7
    // GPU thermal sensors.
    {"gpu0-usr", TemperatureType::GPU},
    {"gpu1-usr", TemperatureType::GPU},
    // Battery thermal sensor.
    {"battery", TemperatureType::BATTERY},
    // Skin thermal sensor.
    {kSkinSensorType, TemperatureType::SKIN},
    // USBC thermal sensor.
    {"usbc-therm-adc", TemperatureType::UNKNOWN},
};

namespace {

using android::base::StringPrintf;

void parseCpuUsagesFileAndAssignUsages(hidl_vec<CpuUsage>* cpu_usages) {
    uint64_t cpu_num, user, nice, system, idle;
    std::string cpu_name;
    std::string data;
    if (!android::base::ReadFileToString(kCpuUsageFile, &data)) {
        LOG(ERROR) << "Error reading Cpu usage file: " << kCpuUsageFile;
        return;
    }

    std::istringstream stat_data(data);
    std::string line;
    while (std::getline(stat_data, line)) {
        if (line.find("cpu") == 0 && isdigit(line[3])) {
            // Split the string using spaces.
            std::vector<std::string> words = android::base::Split(line, " ");
            cpu_name = words[0];
            cpu_num = std::stoi(cpu_name.substr(3));

            if (cpu_num < kMaxCpus) {
                user = std::stoi(words[1]);
                nice = std::stoi(words[2]);
                system = std::stoi(words[3]);
                idle = std::stoi(words[4]);

                // Check if the CPU is online by reading the online file.
                std::string cpu_online_path = StringPrintf(
                    "%s/%s/%s", kCpuOnlineRoot, cpu_name.c_str(),
                    kCpuOnlineFileSuffix);
                std::string is_online;
                if (!android::base::ReadFileToString(
                    cpu_online_path, &is_online)) {
                    LOG(ERROR) << "Could not open Cpu online file: "
                                 << cpu_online_path;
                    return;
                }
                is_online = android::base::Trim(is_online);

                (*cpu_usages)[cpu_num].name = cpu_name;
                (*cpu_usages)[cpu_num].active = user + nice + system;
                (*cpu_usages)[cpu_num].total = user + nice + system + idle;
                (*cpu_usages)[cpu_num].isOnline = (is_online == "1") ?
                    true : false;
            } else {
                LOG(ERROR) << "Unexpected cpu number: " << words[0];
                return;
            }
        }
    }
}

float getThresholdFromType(const TemperatureType type,
                           const ThrottlingThresholds& threshold) {
    switch (type) {
        case TemperatureType::CPU:
          return threshold.cpu;
        case TemperatureType::GPU:
          return threshold.gpu;
        case TemperatureType::BATTERY:
          return threshold.battery;
        case TemperatureType::SKIN:
          return threshold.ss;
        default:
          return NAN;
    }
}

}  // namespace

// This is a golden set of cooling device types and their corresponding sensor
// thernal zone name.
static const std::map<std::string, std::string>
kValidCoolingDeviceTypeMap = {
    {"thermal-cpufreq-0", "cpu0-silver-usr"},  // CPU0
    {"thermal-cpufreq-1", "cpu1-silver-usr"},  // CPU1
    {"thermal-cpufreq-2", "cpu2-silver-usr"},  // CPU2
    {"thermal-cpufreq-3", "cpu3-silver-usr"},  // CPU3
    {"thermal-cpufreq-4", "cpu0-gold-usr"},  // CPU4
    {"thermal-cpufreq-5", "cpu1-gold-usr"},  // CPU5
    {"thermal-cpufreq-6", "cpu2-gold-usr"},  // CPU6
    {"thermal-cpufreq-7", "cpu3-gold-usr"},  // CPU7
};

/*
 * Populate the sensor_name_to_file_map_ map by walking through the file tree,
 * reading the type file and assigning the temp file path to the map.  If we do
 * not succeed, abort.
 */
ThermalHelper::ThermalHelper() :
        is_initialized_(initializeSensorMap() && initializeCoolingDevices()) {
    if (!is_initialized_) {
        LOG(FATAL) << "ThermalHAL could not be initialized properly.";
    }

    std::string hw = android::base::GetProperty("ro.hardware", "");
    std::string thermal_config(kThermalConfigPrefix + hw + ".conf");
    std::string vr_thermal_config(kThermalConfigPrefix + hw + "-vr.conf");
    InitializeThresholdsFromThermalConfig(thermal_config,
                                          vr_thermal_config,
                                          kValidThermalSensorTypeMap,
                                          &thresholds_,
                                          &shutdown_thresholds_,
                                          &vr_thresholds_);
}

std::vector<std::string> ThermalHelper::getCoolingDevicePaths() {
    std::vector<std::string> paths;
    for (const auto& entry : kValidCoolingDeviceTypeMap) {
        std::string path = cooling_devices_.getCoolingDevicePath(entry.first);
        if (!path.empty()) {
            paths.push_back(path + "/cur_state");
        }
    }
    return paths;
}

const std::map<std::string, std::string>&
ThermalHelper::getValidCoolingDeviceMap() const {
    return kValidCoolingDeviceTypeMap;
}

bool ThermalHelper::readCoolingDevice(
        const std::string& cooling_device, int* data) const {
    return cooling_devices_.getCoolingDeviceState(cooling_device, data);
}

bool ThermalHelper::readTemperature(
        const std::string& sensor_name, Temperature* out) const {
    // Read the file.  If the file can't be read temp will be empty string.
    std::string temp;
    std::string path;

    if (!thermal_sensors_.readSensorFile(sensor_name, &temp, &path)) {
        LOG(ERROR) << "readTemperature: sensor not found: " << sensor_name;
        return false;
    }

    if (temp.empty() && !path.empty()) {
        LOG(ERROR) << "readTemperature: failed to open file: " << path;
        return false;
    }

    out->type = kValidThermalSensorTypeMap.at(sensor_name);
    out->name = sensor_name;
    out->currentValue = std::stoi(temp) * kMultiplier;
    out->throttlingThreshold = getThresholdFromType(
        kValidThermalSensorTypeMap.at(sensor_name), thresholds_);
    out->shutdownThreshold = getThresholdFromType(
        kValidThermalSensorTypeMap.at(sensor_name),
        shutdown_thresholds_);
    out->vrThrottlingThreshold = getThresholdFromType(
        kValidThermalSensorTypeMap.at(sensor_name),
        vr_thresholds_);

    LOG(DEBUG) << StringPrintf(
        "readTemperature: %d, %s, %g, %g, %g, %g",
        out->type, out->name.c_str(), out->currentValue,
        out->throttlingThreshold, out->shutdownThreshold,
        out->vrThrottlingThreshold);

    return true;
}

bool ThermalHelper::initializeSensorMap() {
    for (size_t sensor_zone_num = 0; sensor_zone_num < kMaxSensorSearchNum;
            ++sensor_zone_num) {
        std::string sensor_name_path = StringPrintf(
            "%s/%s%zu/%s", kThermalSensorsRoot, kThermalZoneDirSuffix,
            sensor_zone_num, kSensorTypeFileSuffix);
        std::string sensor_temp_path = StringPrintf(
            "%s/%s%zu/%s", kThermalSensorsRoot, kThermalZoneDirSuffix,
            sensor_zone_num, kTemperatureFileSuffix);

        std::string sensor_name;
        if (android::base::ReadFileToString(sensor_name_path, &sensor_name)) {
            sensor_name = android::base::Trim(sensor_name);
            if (kValidThermalSensorTypeMap.find(sensor_name) !=
                kValidThermalSensorTypeMap.end()) {
                  if (!thermal_sensors_.addSensor(
                      sensor_name, sensor_temp_path)) {
                        LOG(ERROR) << "Could not add " << sensor_name
                                   << "to sensors map";
                  }
            }
        }

        if (kValidThermalSensorTypeMap.size()
                == thermal_sensors_.getNumSensors()) {
            return true;
        }
    }
    return false;
}

bool ThermalHelper::initializeCoolingDevices() {
    std::string base_path(kThermalSensorsRoot);
    for (size_t cooling_device_num = 0;
            cooling_device_num < kMaxSensorSearchNum; ++cooling_device_num) {
        std::string path = StringPrintf(
            "%s/%s%zu", kThermalSensorsRoot, kCoolingDeviceDirSuffix,
            cooling_device_num);
        std::string cooling_device_name_path = StringPrintf(
            "%s/%s", path.c_str(), kSensorTypeFileSuffix);

        std::string cooling_device_name;
        if (android::base::ReadFileToString(
                cooling_device_name_path, &cooling_device_name)) {

            cooling_device_name = android::base::Trim(cooling_device_name);
            if (kValidCoolingDeviceTypeMap.find(cooling_device_name) !=
                kValidCoolingDeviceTypeMap.end()) {
                  if (!cooling_devices_.addCoolingDevice(
                          cooling_device_name, path)) {
                      LOG(ERROR) << "Could not add " << cooling_device_name
                                 << "to cooling device map";
                      continue;
                  }

                  int data;
                  if (cooling_devices_.getCoolingDeviceState(
                          cooling_device_name, &data)) {
                      cooling_device_path_to_throttling_level_map_.emplace(
                          cooling_devices_.getCoolingDevicePath(
                              cooling_device_name).append("/cur_state"),
                          data);
                  } else {
                      LOG(ERROR) << "Could not read cooling device value.";
                  }
            }
        }

        if (kValidCoolingDeviceTypeMap.size() ==
                cooling_devices_.getNumCoolingDevices()) {
            return true;
        }
    }
    return false;
}

bool ThermalHelper::fillTemperatures(hidl_vec<Temperature>* temperatures) {
    temperatures->resize(kValidThermalSensorTypeMap.size());
    int current_index = 0;
    for (const auto& name_type_pair : kValidThermalSensorTypeMap) {
        Temperature temp;
        if (readTemperature(name_type_pair.first, &temp)) {
            (*temperatures)[current_index] = temp;
        } else {
            LOG(ERROR) << "Error reading temperature for sensor: "
                       << name_type_pair.first;
            return false;
        }
        ++current_index;
    }
    return current_index > 0;
}

bool ThermalHelper::fillCpuUsages(hidl_vec<CpuUsage>* cpu_usages) {
    cpu_usages->resize(kMaxCpus);
    parseCpuUsagesFileAndAssignUsages(cpu_usages);
    return true;
}

int ThermalHelper::getMaxThrottlingLevelFromMap() const {
    auto max_element =  std::max_element(
        cooling_device_path_to_throttling_level_map_.begin(),
        cooling_device_path_to_throttling_level_map_.end(),
        [] (const std::pair<std::string, int>& p1,
            const std::pair<std::string, int>& p2) {
          return p1.second < p2.second; });
    return max_element->second;
}

bool ThermalHelper::checkThrottlingData(
        const std::pair<std::string, std::string>& throttling_data,
        std::pair<bool, Temperature>* notify_params) {
    Temperature temp;
    if (!readTemperature(kSkinSensorType, &temp)) {
        LOG(ERROR) << "Could not read skin sensor temperature.";
    }

    // If throttling data is in the map add it into the map and check the
    // conditions for notification. If not just check if we're alreadhy
    // throttling or not and notify.
    std::string cooling_device = throttling_data.first;
    if (!cooling_device.empty() &&
            cooling_device_path_to_throttling_level_map_.find(cooling_device) !=
            cooling_device_path_to_throttling_level_map_.end()) {
        int throttling_level = std::stoi(throttling_data.second);
        int max_throttling_level = getMaxThrottlingLevelFromMap();
        cooling_device_path_to_throttling_level_map_[throttling_data.first] =
            throttling_level;

        // We only want to send throttling notifications whenever a new
        // throttling level is reached or if we stop throttling. This first case
        // is to check if a CPU has been throttled higher than the current max.
        // This means that we have to notify throttling and set is_throttling to
        // true. The second case is to check that we are no longer throttling.
        // Meaning that we notify throttling and set is_throttling to false.
        if (max_throttling_level < throttling_level) {
            *notify_params = std::make_pair(true , temp);
            return true;
        } else if (max_throttling_level != 0 &&
                   getMaxThrottlingLevelFromMap() == 0) {
            *notify_params = std::make_pair(false , temp);
            return true;
        }
    }

    return false;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android
