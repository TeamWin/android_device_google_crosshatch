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

#include <regex>
#include <vector>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>

#include "ThermalConfigParser.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

constexpr float kConfigMultiplier = .001;

// For now this just defines the sensor name and thresholds.
struct SensorConfig {
    SensorConfig() : sensor_name(""), threshold(0.0), action(""),
        rule_name("") {}
    SensorConfig(
        const std::string& sensor_name, float threshold,
        const std::string& action, const std::string& rule_name)
            : sensor_name(sensor_name),
              threshold(threshold),
              action(action),
              rule_name(rule_name) {}
    std::string sensor_name;
    float threshold;
    std::string action;
    std::string rule_name;
};

// Assign out if out == NAN or config_threshold < out.
static void checkAndAssignThreshold(float config_threshold, float* out) {
    if (std::isnan(*out) || config_threshold < *out) {
        *out = config_threshold;
    }
}

static void assignThresholdsFromConfig(
    const std::vector<SensorConfig>& configs,
    const std::map<std::string, SensorInfo>& sensor_name_type_map,
    ThrottlingThresholds* threshold) {
    for (const SensorConfig& config : configs) {
        switch (sensor_name_type_map.at(config.sensor_name).type) {
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
                // For the skin throttling threshold take the min value from
                // the skin-monitor rule.
                if (config.rule_name == "SKIN-MONITOR") {
                    checkAndAssignThreshold(config.threshold, &threshold->ss);
                }
                break;
            default:
                LOG(ERROR) << "Unknown sensor: " << config.sensor_name;
                break;
        }
    }
}

static void parseThermalEngineConfig(
        const std::string& config_path, std::vector<SensorConfig>* configs,
        std::vector<SensorConfig>* shutdown_configs) {
    std::string data;
    if (!android::base::ReadFileToString(config_path, &data)) {
        LOG(ERROR) << "Error reading config path: " << config_path;
        return;
    }

    // Parse out sensor name and thresholds for ss configs.
    static const std::regex ss_block_regex(
        R"([.\n]*\[(.+)\]\nalgo_type\s+ss\n(?:.*\n)?sensor\s+)"
        R"(([\w\d-]+)\n(?:.*\n)?set_point\s+(\d+))");
    auto block_begin = std::sregex_iterator(
        data.begin(), data.end(), ss_block_regex);
    auto block_end = std::sregex_iterator();
    for (std::sregex_iterator itr = block_begin; itr != block_end; ++itr) {
        configs->emplace_back(
            SensorConfig(
                itr->str(2), std::stoi(itr->str(3)) * kConfigMultiplier, "",
                itr->str(1)));
    }

    // Parse out sensor name, thresholds, and action for monitor configs.
    static const std::regex monitor_block_regex(
        R"([.\n]*\[(.+)\]\nalgo_type\s+monitor\n(?:.*\n)?sensor\s+)"
        R"(([\w\d-]+)\n(?:.*\n)?thresholds\s+([^\n]+)\n(?:.*\n)?actions\s+([^\n]+))");
    block_begin = std::sregex_iterator(
        data.begin(), data.end(), monitor_block_regex);
    for (std::sregex_iterator itr = block_begin; itr != block_end; ++itr) {
        SensorConfig sensor_config;

        sensor_config.rule_name = itr->str(1);
        sensor_config.sensor_name = itr->str(2);
        std::string thresholds_str = itr->str(3);
        std::string actions_str = itr->str(4);

        static const std::regex delim_regex{R"(\s+)"};
        std::vector<std::string> thresholds{
            std::sregex_token_iterator(thresholds_str.begin(), thresholds_str.end(), delim_regex, -1),
                    std::sregex_token_iterator()
                    };
        std::vector<std::string> actions{
            std::sregex_token_iterator(actions_str.begin(), actions_str.end(), delim_regex, -1),
                    std::sregex_token_iterator()
                    };

        if (thresholds.size() != actions.size()) {
            LOG(ERROR) << "Unbalanced thresholds ("
                       << thresholds.size()
                       << ") and actions ("
                       << actions.size() << ") in monitor algo for sensor ("
                       << sensor_config.sensor_name
                       << ") of rule (" << sensor_config.rule_name << ")";
            return;
        }

        for (std::size_t i = 0; i < thresholds.size(); ++i) {
            sensor_config.threshold = std::stoi(thresholds[i]) * kConfigMultiplier;
            sensor_config.action = actions[i];

            // Filter out the shutdown thresholds.
            if (sensor_config.action.find("shutdown") != std::string::npos)  {
                shutdown_configs->push_back(sensor_config);
            } else {
                configs->push_back(sensor_config);
            }
        }
    }
}

static void dumpSensorConfigs(
    const std::vector<SensorConfig>& sensor_config_vec,
    const std::map<std::string, SensorInfo>& typeMap) {
    for (const auto& sensor_config : sensor_config_vec) {
        LOG(INFO) << "Sensor name: " << sensor_config.sensor_name
                  << " type: " << android::hardware::thermal::V1_0::toString(
                      typeMap.at(sensor_config.sensor_name).type)
                  << " with threshold: " << sensor_config.threshold
                  << " from rule: " << sensor_config.rule_name
                  << sensor_config.action.empty() ? ""
                : " sensor config action " + sensor_config.action;
    }
}

void InitializeThresholdsFromThermalConfig(
    const std::string& thermal_config,
    const std::string& vr_thermal_config,
    const std::map<std::string, SensorInfo>& typeMap,
    ThrottlingThresholds *thresholds,
    ThrottlingThresholds *shutdown_thresholds,
    ThrottlingThresholds *vr_thresholds) {
    // For now just read the blueline configs but it would be easy to make this
    // platform specific.
    std::vector<SensorConfig> sensor_configs;
    std::vector<SensorConfig> shutdown_configs;
    parseThermalEngineConfig(
        thermal_config, &sensor_configs, &shutdown_configs);

    assignThresholdsFromConfig(sensor_configs, typeMap, thresholds);
    assignThresholdsFromConfig(shutdown_configs, typeMap, shutdown_thresholds);

    LOG(INFO) << "Sensor configs";
    dumpSensorConfigs(sensor_configs, typeMap);
    LOG(INFO) << "Shutdown configs";
    dumpSensorConfigs(shutdown_configs, typeMap);

    sensor_configs.clear();
    shutdown_configs.clear();
    parseThermalEngineConfig(
        vr_thermal_config, &sensor_configs, &shutdown_configs);

    LOG(INFO) << "VR Sensor configs";
    dumpSensorConfigs(sensor_configs, typeMap);
    LOG(INFO) << "VR Shutdown configs";
    dumpSensorConfigs(shutdown_configs, typeMap);

    assignThresholdsFromConfig(
        sensor_configs, typeMap, vr_thresholds);
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // nmamespace android
