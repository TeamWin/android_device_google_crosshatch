/*
 * Copyright (C) 2017 The Android Open Source Project
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

#include <dirent.h>
#include <sys/types.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>

#include "ThermalConfigParser.h"
#include "thermal-helper.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

constexpr char kThermalSensorsRoot[] = "/sys/devices/virtual/thermal";
constexpr char kCpuOnlineRoot[] = "/sys/devices/system/cpu";
constexpr char kThermalConfigPrefix[] = "/vendor/etc/thermal-engine-";
constexpr char kCpuUsageFile[] = "/proc/stat";
constexpr char kCpuOnlineFileSuffix[] = "online";
constexpr char kTemperatureFileSuffix[] = "temp";
constexpr char kSensorTypeFileSuffix[] = "type";
constexpr char kThermalZoneDirSuffix[] = "thermal_zone";
constexpr unsigned int kMaxCpus = 8;

// This is a golden set of thermal sensor names and their types.  Used when we
// read in sensor values.
const std::map<std::string, TemperatureType>
kValidThermalSensorNameTypeMap = {
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
    {"quiet-therm-adc", TemperatureType::SKIN},
    // USBC thermal sensor.
    {"usbc-therm-adc", TemperatureType::UNKNOWN},  // USB-C thermistor.
};

namespace {

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
                std::string cpu_online_root(kCpuOnlineRoot);
                std::string cpu_online_path(
                    cpu_online_root + "/" + cpu_name + "/" +
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
    switch(type) {
        case TemperatureType::CPU:
          return threshold.cpu;
        case TemperatureType::GPU:
          return threshold.gpu;
        case TemperatureType::BATTERY:
          return threshold.battery;
        case TemperatureType::SKIN:
          return threshold.ss;
        default:
          LOG(WARNING) << "Attempting to read unknown sensor type: "
                       << android::hardware::thermal::V1_0::toString(type);
          return NAN;
    }
}

}  // namespace

/*
 * Populate the sensor_name_to_file_map_ map by walking through the file tree,
 * reading the type file and assigning the temp file path to the map.  If
 * everything succeeds is_initialized_ is set to true.
 */
ThermalHelper::ThermalHelper() {
    std::string hw = android::base::GetProperty("ro.hardware", "");
    std::string thermal_config(kThermalConfigPrefix + hw + ".conf");
    std::string vr_thermal_config(kThermalConfigPrefix + hw + "-vr.conf");
    is_initialized_ = initializeSensorMap();
    InitializeThresholdsFromThermalConfig(thermal_config,
                                          vr_thermal_config,
                                          kValidThermalSensorNameTypeMap,
                                          &thresholds_,
                                          &shutdown_thresholds_,
                                          &vr_thresholds_);
}

bool ThermalHelper::readTemperature(
        const std::string& sensor_name, Temperature* out) {
    // Read the file.  If the file can't be read temp will be empty string.
    std::string temp;
    std::string path;

    if (!thermal_sensors.readSensorFile(sensor_name, &temp, &path)) {
        LOG(ERROR) << "readTemperature: sensor not found: " << sensor_name;
        return false;
    }

    if (temp.empty() && !path.empty()) {
        LOG(ERROR) << "readTEmperature: failed to open file: " << path;
        return false;
    }

    out->type = kValidThermalSensorNameTypeMap.at(sensor_name);
    out->name = sensor_name;
    out->currentValue = std::stoi(temp) * kMultiplier;
    out->throttlingThreshold = getThresholdFromType(
        kValidThermalSensorNameTypeMap.at(sensor_name), thresholds_);
    out->shutdownThreshold = getThresholdFromType(
        kValidThermalSensorNameTypeMap.at(sensor_name),
        shutdown_thresholds_);
    out->vrThrottlingThreshold = getThresholdFromType(
        kValidThermalSensorNameTypeMap.at(sensor_name),
        vr_thresholds_);

    LOG(DEBUG) << android::base::StringPrintf(
        "readTemperature: %d, %s, %g, %g, %g, %g",
        out->type, out->name.c_str(), out->currentValue,
        out->throttlingThreshold, out->shutdownThreshold,
        out->vrThrottlingThreshold);

    return true;
}

bool ThermalHelper::initializeSensorMap() {
    auto thermal_zone_dir = std::unique_ptr<DIR, int (*)(DIR*)>(
        opendir(kThermalSensorsRoot), closedir);
    struct dirent* dp;
    size_t num_thermal_zones = 0;
    while ((dp = readdir(thermal_zone_dir.get())) != nullptr) {
        std::string dir_name(dp->d_name);
        if (dir_name.find(kThermalZoneDirSuffix) != std::string::npos) {
            ++num_thermal_zones;
        }
    }

    for (size_t sensor_zone_num = 0; sensor_zone_num < num_thermal_zones;
            ++sensor_zone_num) {
        std::string path = android::base::StringPrintf("%s/%s%zu",
                                                       kThermalSensorsRoot,
                                                       kThermalZoneDirSuffix,
                                                       sensor_zone_num);
        std::string sensor_name;
        if (android::base::ReadFileToString(
                path + "/" + kSensorTypeFileSuffix, &sensor_name)) {
            sensor_name = android::base::Trim(sensor_name);
            if (kValidThermalSensorNameTypeMap.find(sensor_name) !=
                kValidThermalSensorNameTypeMap.end()) {
                  if (!thermal_sensors.addSensor(
                      sensor_name, path + "/" + kTemperatureFileSuffix)) {
                        LOG(ERROR) << "Could not add " << sensor_name
                                   << "to sensors map";
                  }
            }
        }
    }
    return (kValidThermalSensorNameTypeMap.size()
            == thermal_sensors.getNumSensors());
}

bool ThermalHelper::fillTemperatures(hidl_vec<Temperature>* temperatures) {
    temperatures->resize(kValidThermalSensorNameTypeMap.size());
    int current_index = 0;
    for (const auto& name_type_pair : kValidThermalSensorNameTypeMap) {
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

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // nmamespace android
