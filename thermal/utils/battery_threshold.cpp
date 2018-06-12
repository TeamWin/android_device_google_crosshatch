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

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include "battery_threshold.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

enum class AgeRange
{
    Fresh,
    Above200,
    Above400,
    Above600,
    Above900
};
enum class TempRange
{
    AboveZero,
    ZeroToNegFive,
    NegFiveToNegTen,
    BelowNegTen
};

namespace {

TempRange rangeFromTemp(float current_temp)
{
    if (current_temp >= 0.0) {
        return TempRange::AboveZero;
    }
    if ((current_temp >= -5.0) && (current_temp < 0.0)) {
        return TempRange::ZeroToNegFive;
    }
    if ((current_temp >= -10.0) && (current_temp < -5.0)) {
        return TempRange::NegFiveToNegTen;
    }
    if ((current_temp < -10.0)) {
        return TempRange::BelowNegTen;
    }
    return TempRange::AboveZero;
}

AgeRange ageFromCC(int cycle_count)
{
    if ( cycle_count < 200) {
        return AgeRange::Fresh;
    }
    if (( cycle_count >= 200) && (cycle_count < 400)) {
        return AgeRange::Above200;
    }
    if (( cycle_count >= 400) && (cycle_count < 600)) {
        return AgeRange::Above400;
    }
    if (( cycle_count >= 600) && (cycle_count < 900)) {
        return AgeRange::Above600;
    }
    return AgeRange::Above900;
}

std::ostream& operator<<(std::ostream& os, const AgeRange& age)
{
    switch (age) {
    case AgeRange::Fresh:
        os << "Fresh";
        break;
    case AgeRange::Above200:
        os << "Above200";
        break;
    case AgeRange::Above400:
        os << "Above400";
        break;
    case AgeRange::Above600:
        os << "Above600";
        break;
    case AgeRange::Above900:
        os << "Above900";
        break;
    default:
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const TempRange& temp)
{
    switch (temp) {
    case TempRange::AboveZero:
        os << "AboveZero";
        break;
    case TempRange::ZeroToNegFive:
        os << "ZeroToNegFive";
        break;
    case TempRange::NegFiveToNegTen:
        os << "NegFiveToNegTen";
        break;
    case TempRange::BelowNegTen:
        os << "BelowNegTen";
        break;
    default:
        break;
    }
    return os;
}
} // namespace

bool BatteryThresholdLUT::readCycleCount(int& cycle_count) const
{
    std::string contents;
    if (!android::base::ReadFileToString(cycle_count_filename_, &contents)) {
        return false;
    }
    cycle_count = std::stoi(android::base::Trim(contents));
    return true;
}

bool BatteryThresholdLUT::readSoC(int& soc) const
{
    std::string contents;
    if (!android::base::ReadFileToString(
        state_of_charge_filename_, &contents)) {
        return false;
    }
    soc = std::stoi(android::base::Trim(contents));
    return true;
}

bool BatteryThresholdLUT::readStatus(std::string& status) const
{
    if (!android::base::ReadFileToString(status_filename_, &status)) {
        return false;
    }
    status = android::base::Trim(status);
    return true;
}

constexpr float kThresholdDecrease = 5.0;
float BatteryThresholdLUT::adjustThreshold(
    float configured_threshold, float current_temperature) const
{
    int current_soc = 100;
    int cycle_count = 0;
    std::string status;
    bool rc = readStatus(status);
    if (!rc || status != "Discharging") {
        return configured_threshold;
    }

    TempRange temp_range = rangeFromTemp(current_temperature);

    if (!readCycleCount(cycle_count)) {
        return configured_threshold;
    }
    AgeRange age = ageFromCC(cycle_count);

    auto it = battery_conditions_lut_.find({temp_range, age});
    if (it == battery_conditions_lut_.end()) {
        return configured_threshold;
    }
    int low_soc = it->second;

    if (!readSoC(current_soc) || (low_soc <= current_soc)) {
        return configured_threshold;
    }

    //TODO: We should expand the semantics of the Thermal Hal so that the
    //      temperature thresholds have a max and a min. For now, just set the
    //      threshold below the actual value.
    float low_temp_threshold = current_temperature - kThresholdDecrease;
    LOG(DEBUG) << "Low temp and battery conditions present."
        << " (cycle_count: " << cycle_count
        << " SoC: " << current_soc
        << " temperature: " << current_temperature << ")\n"
        << " Recommending threshold : " << low_temp_threshold;
    return low_temp_threshold;
}

BatteryThresholdLUT::BatteryThresholdLUT() :
    battery_conditions_lut_ {
        {{ TempRange::ZeroToNegFive,   AgeRange::Above900 },  40 },
        {{ TempRange::NegFiveToNegTen, AgeRange::Above400 },   5 },
        {{ TempRange::NegFiveToNegTen, AgeRange::Above600 },  15 },
        {{ TempRange::NegFiveToNegTen, AgeRange::Above900 },  50 },
        {{ TempRange::BelowNegTen,     AgeRange::Fresh    },   5 },
        {{ TempRange::BelowNegTen,     AgeRange::Above200 },  12 },
        {{ TempRange::BelowNegTen,     AgeRange::Above400 },  45 },
        {{ TempRange::BelowNegTen,     AgeRange::Above600 },  60 },
        {{ TempRange::BelowNegTen,     AgeRange::Above900 }, 100 }
    },
    cycle_count_filename_{"/sys/class/power_supply/battery/cycle_count"},
    state_of_charge_filename_{"/sys/class/power_supply/battery/capacity"},
    status_filename_{"/sys/class/power_supply/battery/status"}
{
}

bool BatteryThresholdLUT::fillBatteryThresholdDebugInfo(
    std::ostringstream& dump_buf) const
{
    int soc = -1;
    int cycle_count = -1;
    std::string status;
    bool rc = readSoC(soc);
    rc |= readCycleCount(cycle_count);
    rc |= readStatus(status);

    dump_buf << "ThermalBattery: Status: " << status
             << " SoC: " << soc
             << " Cycle count: " << cycle_count << std::endl;
    dump_buf << "ThermalBattery: LookupTable: " << std::endl;

    for (const auto & entry : battery_conditions_lut_) {
        dump_buf << "\t(Temp: " << entry.first.first
                    << ", Age: " << entry.first.second
                    << ") -> " << entry.second << " SoC Level" << std::endl;
    }
    return rc;

}

size_t BatteryThresholdLUT::TempAgeHash::operator()(
    const std::pair<TempRange, AgeRange>& k) const
{
    return static_cast<size_t>(k.first) << 16 | static_cast<size_t>(k.second);
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android
