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

static constexpr char kChargerStatus[] = "sys/class/power_supply/battery/status";
static constexpr char kStatusIsFull[] = "Full";
static constexpr char kStatusIsCharging[] = "Charging";
static constexpr int kTransitionTime = 900;  // Seconds; Transition time is 15 minutes.
static constexpr int kFullSoc = 100;
// Recharging cycle value is 98, it means it must be lower than 98 to re-charge. The
// startup charging level is 97. when SOC is lower than 97, it means overloading.
static constexpr int kRechargeSoc = 97;

BatteryRechargingControl::BatteryRechargingControl() {
    state_ = INACTIVE;
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

int BatteryRechargingControl::RemapSOC(int soc) {
    double diff_sec = difftime(time(NULL), start_time_);
    double ret_soc =
        round(soc * (diff_sec / kTransitionTime) + 100 * (1 - (diff_sec / kTransitionTime)));
    LOG(INFO) << "RemapSOC: " << ret_soc;
    return ret_soc;
}

int BatteryRechargingControl::updateBatteryProperties(struct android::BatteryProperties *props) {
    std::string charger_status;
    int ret_soc, battery_soc;
    double elapsed_time;

    battery_soc = props->batteryLevel;

    if (!android::base::ReadFileToString(std::string(kChargerStatus), &charger_status)) {
        LOG(ERROR) << "Cannot read the charging status";
        return battery_soc;
    }

    ret_soc = battery_soc;

    props->batteryStatus = getBatteryStatus(charger_status.c_str());

    if ((state_ == INACTIVE) && (battery_soc < kFullSoc))
        return battery_soc;

    LOG(INFO) << "State: " << state_ << "; battery state: " << props->batteryStatus;

    switch (state_) {
        case WAIT_EOC:
            // Charger status is FULL means the charge_done is trigger and the charging is stop.
            // kFullSoc: SOC=100, means the charge full, but it still can charge more till charge done.
            if ((charger_status.compare(0, strlen(kStatusIsFull), kStatusIsFull) == 0) &&
                (battery_soc == kFullSoc)) {
                state_ = RECHARGING_CYCLE;
            }
            break;
        case RECHARGING_CYCLE:
            if (charger_status.compare(0, strlen(kStatusIsFull), kStatusIsFull) == 0) {
                ret_soc = kFullSoc;
            } else if (charger_status.compare(0, strlen(kStatusIsCharging), kStatusIsCharging) == 0) {
                // Recharging cycle start.
                if (battery_soc >= kRechargeSoc) {
                    ret_soc = kFullSoc;
                } else {
                    // overload condition:
                    // When the SOC is lower than recharging cycle value and
                    // the device is re-charging, it means the power consumption
                    // is over the charging.
                    start_time_ = time(NULL);
                    state_ = OVER_LOADING;
                    ret_soc = RemapSOC(battery_soc);
                }
            } else {
                // Charging gone
                start_time_ = time(NULL);
                state_ = CABLE_OUT;
                ret_soc = RemapSOC(battery_soc);
            }
            break;
        case OVER_LOADING:
        case CABLE_OUT:
            elapsed_time = difftime(time(NULL), start_time_);
            if (elapsed_time > kTransitionTime) {
                LOG(INFO) << "Time is up, leave remap";
                state_ = INACTIVE;
                break;
            } else {
                LOG(INFO) << "Diff time: " << elapsed_time;
                ret_soc = RemapSOC(battery_soc);
                if (ret_soc == battery_soc) {
                    state_ = INACTIVE;
                    break;
                }
            }
            if (charger_status.compare(0, strlen(kStatusIsCharging), kStatusIsCharging) == 0) {
                if ((ret_soc == kFullSoc) && (battery_soc >= kRechargeSoc)) {
                    // When user plug in charger and the ret_soc is still 100%
                    // Change condition to Recharging cycle to avoid the SOC
                    // show lower than 100%. (Keep 100%)
                    state_ = RECHARGING_CYCLE;
                }
            }
            break;
        default:
            state_ = WAIT_EOC;
            break;
    }
    LOG(INFO) << "New state: " << state_ << "; battery state: " << props->batteryStatus;
    return ret_soc;
}

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device
