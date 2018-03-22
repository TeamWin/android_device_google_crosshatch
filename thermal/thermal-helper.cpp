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

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

#include <dirent.h>
#include <sys/types.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>

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
constexpr char kSkinSensorName[] = "quiet-therm-adc";
constexpr char kBatterySensorName[] = "battery";
constexpr float kMultiplier = .001;

// This is a golden set of thermal sensor names and their types.  Used when we
// read in sensor values.
const static std::map<std::string, TemperatureType>
kValidThermalSensorNameTypeMap = {
    {"cpu0-silver-usr", TemperatureType::CPU},  // CPU0
    {"cpu1-silver-usr", TemperatureType::CPU},  // CPU1
    {"cpu2-silver-usr", TemperatureType::CPU},  // CPU2
    {"cpu3-silver-usr", TemperatureType::CPU},  // CPU3
    {"cpu0-gold-usr", TemperatureType::CPU},  // CPU4
    {"cpu1-gold-usr", TemperatureType::CPU},  // CPU5
    {"cpu2-gold-usr", TemperatureType::CPU},  // CPU6
    {"cpu3-gold-usr", TemperatureType::CPU},  // CPU7
    // GPU thermal sensors.
    {"gpu0-usr", TemperatureType::GPU},
    {"gpu1-usr", TemperatureType::GPU},
    {kBatterySensorName, TemperatureType::BATTERY},
    {kSkinSensorName, TemperatureType::SKIN},
    {"usbc-therm-adc", TemperatureType::UNKNOWN},  // USB-C thermistor.
};

namespace {

using ::android::hardware::thermal::V1_0::TemperatureType;

// For now this just defines the sensor name and thresholds.
struct SensorConfig {
    SensorConfig() : sensor_name(""), threshold(0.0), action("") {}
    SensorConfig(
        const std::string& sensor_name, float threshold,
        const std::string& action)
            : sensor_name(sensor_name),
              threshold(threshold),
              action(action) {}
    std::string sensor_name;
    float threshold;
    std::string action;
};

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
            std::istringstream iss(line);
            std::vector<std::string> words(
                std::istream_iterator<std::string>{iss},
                std::istream_iterator<std::string>());
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

void parseThermalEngineConfig(
        const std::string& config_path, std::vector<SensorConfig>* configs,
        std::vector<SensorConfig>* shutdown_configs) {
    std::string data;
    if (!android::base::ReadFileToString(config_path, &data)) {
        LOG(ERROR) << "Error reading config path: " << config_path;
        return;
    }

    // Parse out sensor name and thresholds for ss configs.
    std::regex ss_block_regex(
        R"([.\n]*\[.*\]\nalgo_type\s+ss\n(?:.*\n)?sensor )"
        R"(([\w\d-]+)\n(?:.*\n)?set_point\s+(\d+))");
    auto block_begin = std::sregex_iterator(
        data.begin(), data.end(), ss_block_regex);
    auto block_end = std::sregex_iterator();
    for (std::sregex_iterator itr = block_begin; itr != block_end; ++itr) {
        configs->emplace_back(
            SensorConfig(
                itr->str(1), std::stoi(itr->str(2)) * kMultiplier, ""));
    }

    // Parse out sensor name, thresholds, and action for monitor configs.
    std::regex monitor_block_regex(
        R"([.\n]*\[.*\]\nalgo_type\s+monitor\n(?:.*\n)?sensor )"
        R"(([\w\d-]+)\n(?:.*\n)?thresholds\s+(\d+)\n(?:.*\n)?actions\s+(\w+))");
    block_begin = std::sregex_iterator(
        data.begin(), data.end(), monitor_block_regex);
    for (std::sregex_iterator itr = block_begin; itr != block_end; ++itr) {
        SensorConfig sensor_config;
        sensor_config.sensor_name = itr->str(1);
        sensor_config.threshold = std::stoi(itr->str(2)) * kMultiplier;
        sensor_config.action = itr->str(3);

        // Filter out the shutdown thresholds.
        if (sensor_config.action == "shutdown") {
            shutdown_configs->emplace_back(sensor_config);
        } else {
            configs->emplace_back(sensor_config);
        }
    }
}

// Assign out if out == NAN or config_threshold < out.
static void checkAndAssignThreshold(float config_threshold, float* out) {
    if (*out == NAN || config_threshold < *out) {
        *out = config_threshold;
    }
}

void assignThresholdsFromConfig(
        const std::vector<SensorConfig>& configs,
        const std::map<std::string, TemperatureType>& sensor_name_type_map,
    ThrottlingThresholds* threshold) {
    for (const SensorConfig& config : configs) {
        switch(sensor_name_type_map.at(config.sensor_name)) {
            case TemperatureType::CPU:
              checkAndAssignThreshold(config.threshold, &threshold->cpu);
              break;
            case TemperatureType::GPU:
              checkAndAssignThreshold(config.threshold, &threshold->gpu);
              break;
            case TemperatureType::BATTERY:
              checkAndAssignThreshold(config.threshold, &threshold->battery);
              break;
            case TemperatureType::SKIN:
              checkAndAssignThreshold(config.threshold, &threshold->ss);
              break;
            default:
              LOG(ERROR) << "Unknown sensor: " << config.sensor_name;
              break;
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
          LOG(ERROR) << "Attempting to read unknown sensor type: "
                     << static_cast<int>(type);
          return NAN;
    }
}

void dumpSensorConfigs(const std::vector<SensorConfig>& sensor_config_vec) {
    for (const auto& sensor_config : sensor_config_vec) {
        LOG(DEBUG) << "Sensor name: " << sensor_config.sensor_name
                   << " with threshold: " << sensor_config.threshold
                   << sensor_config.action.empty() ? ""
                   : " sensor config action" + sensor_config.action;
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
    initializeThresholds(thermal_config, vr_thermal_config);
}

void ThermalHelper::initializeThresholds(
  const std::string& thermal_config, const std::string& vr_thermal_config) {
    // For now just read the blueline configs but it would be easy to make this
    // platform specific.
    std::vector<SensorConfig> sensor_configs;
    std::vector<SensorConfig> shutdown_configs;
    parseThermalEngineConfig(
        thermal_config, &sensor_configs, &shutdown_configs);

    assignThresholdsFromConfig(
        sensor_configs, kValidThermalSensorNameTypeMap, &thresholds_);
    assignThresholdsFromConfig(
        shutdown_configs, kValidThermalSensorNameTypeMap,
        &shutdown_thresholds_);

    LOG(DEBUG) << "Sensor configs";
    dumpSensorConfigs(sensor_configs);
    LOG(DEBUG) << "Shutdown configs";
    dumpSensorConfigs(shutdown_configs);

    sensor_configs.clear();
    parseThermalEngineConfig(
        vr_thermal_config, &sensor_configs, &shutdown_configs);

    LOG(DEBUG) << "VR Sensor configs";
    dumpSensorConfigs(sensor_configs);
    LOG(DEBUG) << "VR Shutdown configs";
    dumpSensorConfigs(shutdown_configs);

    assignThresholdsFromConfig(
        sensor_configs, kValidThermalSensorNameTypeMap, &vr_thresholds_);
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
    std::string base_path(kThermalSensorsRoot);
    DIR* thermal_zone_dir = opendir(base_path.c_str());
    struct dirent* dp;
    size_t num_thermal_zones = 0;
    while ((dp = readdir(thermal_zone_dir)) != nullptr) {
        std::string dir_name(dp->d_name);
        if (dir_name.find(kThermalZoneDirSuffix) != std::string::npos) {
            ++num_thermal_zones;
        }
    }
    closedir(thermal_zone_dir);

    for (size_t sensor_zone_num = 0; sensor_zone_num < num_thermal_zones;
            ++sensor_zone_num) {
        std::string path = base_path + "/" + kThermalZoneDirSuffix +
            std::to_string(sensor_zone_num);
        std::string sensor_name;
        if (android::base::ReadFileToString(
                path + "/" + kSensorTypeFileSuffix, &sensor_name)) {
            sensor_name.erase(
                std::remove(sensor_name.begin(), sensor_name.end(), '\n'),
                sensor_name.end());
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
    if (!temperatures || temperatures->size() < kMaxTempSensors) {
        LOG(ERROR) << "fillTemperatures: incorrect buffer size";
        return false;
    }
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
    return kValidThermalSensorNameTypeMap.size() > 0;
}

bool ThermalHelper::fillCpuUsages(hidl_vec<CpuUsage>* cpu_usages) {
    if (!cpu_usages || cpu_usages->size() < kMaxCpus) {
        LOG(ERROR) << "fillCpuUsages: incorrect buffer size";
        return false;
    }

    parseCpuUsagesFileAndAssignUsages(cpu_usages);
    return true;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // nmamespace android
