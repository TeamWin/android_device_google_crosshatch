#include <algorithm>

#include <android-base/file.h>
#include "sensors.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

std::string Sensors::getSensorPath(const std::string& sensor_name) {
    if (sensor_names_to_path_map_.find(sensor_name) !=
            sensor_names_to_path_map_.end()) {
        return sensor_names_to_path_map_.at(sensor_name);
    }
    return "";
}

bool Sensors::addSensor(
        const std::string& sensor_name, const std::string& path) {
    return sensor_names_to_path_map_.emplace(sensor_name, path).second;
}

bool Sensors::readSensorFile(
        const std::string& sensor_name, std::string* data,
        std::string* file_path) {
    std::string sensor_reading;
    if (sensor_names_to_path_map_.find(sensor_name) ==
            sensor_names_to_path_map_.end()) {
        *data = "";
        *file_path = "";
        return false;
    }

    android::base::ReadFileToString(
        sensor_names_to_path_map_.at(sensor_name), &sensor_reading);
    // Strip the newline.
    sensor_reading.erase(
        std::remove(sensor_reading.begin(), sensor_reading.end(), '\n'),
        sensor_reading.end());
    *data = sensor_reading;
    *file_path = sensor_names_to_path_map_.at(sensor_name);
    return true;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android
