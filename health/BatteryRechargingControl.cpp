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

#include "BatteryRechargingControl.h"

namespace device {
namespace google {
namespace crosshatch {
namespace health {

const std::string kChargerStatus = "sys/class/power_supply/battery/status";

static const std::string kStatusIsFull = "Full";
static const std::string kStatusIsCharging = "Charging";
static constexpr int kTransitionTime = 15 * 60;  // Seconds
static constexpr int kFullSoc = 100;

BatteryRechargingControl::BatteryRechargingControl() {
    state_ = INACTIVE;
    recharge_soc_ = 0;
}

int BatteryRechargingControl::mapSysfsString(const char *str, struct sysfsStringEnumMap map[]) {
    for (int i = 0; map[i].s; i++)
        if (!strncmp(str, map[i].s, strlen(map[i].s)))
            return map[i].val;

    return -1;
}

int BatteryRechargingControl::getBatteryStatus(const char *status) {
    int ret;
    struct sysfsStringEnumMap batteryStatusMap[] = {
        {"Unknown", android::BATTERY_STATUS_UNKNOWN},
        {"Charging", android::BATTERY_STATUS_CHARGING},
        {"Discharging", android::BATTERY_STATUS_DISCHARGING},
        {"Not charging", android::BATTERY_STATUS_NOT_CHARGING},
        {"Full", android::BATTERY_STATUS_FULL},
        {NULL, 0},
    };

    ret = mapSysfsString(status, batteryStatusMap);
    if (ret < 0) {
        LOG(ERROR) << "Unknown battery status: " << status;
        ret = android::BATTERY_STATUS_UNKNOWN;
    }

    return ret;
}

int64_t BatteryRechargingControl::getTime(void) {
    return nanoseconds_to_seconds(systemTime(SYSTEM_TIME_BOOTTIME));
}

int BatteryRechargingControl::RemapSOC(int soc) {
    double diff_sec = getTime() - start_time_;
    double ret_soc =
        round(soc * (diff_sec / kTransitionTime) + kFullSoc * (1 - (diff_sec / kTransitionTime)));
    LOG(INFO) << "RemapSOC: " << ret_soc;
    return ret_soc;
}

void BatteryRechargingControl::updateBatteryProperties(struct android::BatteryProperties *props) {
    std::string charger_status;
    double elapsed_time;
    int cur_soc;

    if (!android::base::ReadFileToString(kChargerStatus, &charger_status)) {
        LOG(ERROR) << "Cannot read the charger status";
        return;
    }

    charger_status = android::base::Trim(charger_status);
    props->batteryStatus = getBatteryStatus(charger_status.c_str());

    if ((state_ == INACTIVE) && (props->batteryLevel < kFullSoc))
        return;

    LOG(INFO) << "Entry state_: " << state_ << " charger_status: " << charger_status
              << " batteryLevel: " << props->batteryLevel;
    switch (state_) {
        case INACTIVE:
            state_ = WAIT_EOC;
            recharge_soc_ = 0;
        case WAIT_EOC:
            if (charger_status == kStatusIsFull) {
                state_ = RECHARGING_CYCLE;
                props->batteryLevel = kFullSoc;
            } else if (charger_status != kStatusIsCharging) {
                // charging stopped, assume no more power source
                start_time_ = getTime();
                state_ = NO_POWER_SOURCE;
                props->batteryLevel = RemapSOC(props->batteryLevel);
            }
            break;
        case RECHARGING_CYCLE:
            if (charger_status == kStatusIsFull) {
                recharge_soc_ = 0;
                props->batteryLevel = kFullSoc;
                break;
            } else if (charger_status == kStatusIsCharging) {
                // Recharging cycle start.
                if (recharge_soc_ == 0) {
                    recharge_soc_ = props->batteryLevel;
                    props->batteryLevel = kFullSoc;
                } else {
                    if (props->batteryLevel < recharge_soc_) {
                        // overload condition
                        start_time_ = getTime();
                        state_ = OVER_LOADING;
                        props->batteryLevel = RemapSOC(props->batteryLevel);
                    } else {
                        props->batteryLevel = kFullSoc;
                    }
                }
            } else {
                // charging stopped, assume no more power source
                start_time_ = getTime();
                state_ = NO_POWER_SOURCE;
                props->batteryLevel = RemapSOC(props->batteryLevel);
            }
            break;
        case OVER_LOADING:
        case NO_POWER_SOURCE:
            cur_soc = props->batteryLevel;
            elapsed_time = getTime() - start_time_;
            if (elapsed_time > kTransitionTime) {
                LOG(INFO) << "Time is up, leave remap";
                state_ = INACTIVE;
                break;
            } else {
                LOG(INFO) << "Diff time: " << elapsed_time;
                int battery_level = RemapSOC(props->batteryLevel);
                if ((battery_level == props->batteryLevel) && (battery_level != kFullSoc)) {
                    state_ = INACTIVE;
                    break;
                }
                props->batteryLevel = battery_level;
            }
            if (charger_status == kStatusIsCharging) {
                if ((props->batteryLevel == kFullSoc) && (cur_soc >= recharge_soc_)) {
                    // When user plug in charger and the ret_soc is still 100%
                    // Change condition to Recharging cycle to avoid the SOC
                    // show lower than 100%. (Keep 100%)
                    state_ = RECHARGING_CYCLE;
                    recharge_soc_ = props->batteryLevel;
                }
            }
            break;
        default:
            state_ = WAIT_EOC;
            break;
    }
    LOG(INFO) << "Exit state_: " << state_ << " batteryLevel: " << props->batteryLevel;
}

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device
