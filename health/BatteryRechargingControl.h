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

#ifndef DEVICE_GOOGLE_CROSSHATCH_HEALTH_BATTERYRECHARGINGCONTROL_H
#define DEVICE_GOOGLE_CROSSHATCH_HEALTH_BATTERYRECHARGINGCONTROL_H

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <batteryservice/BatteryService.h>
#include <math.h>
#include <time.h>
#include <utils/Timers.h>
#include <string>

namespace device {
namespace google {
namespace crosshatch {
namespace health {

extern const std::string kChargerStatus;

enum RechargeState {
    WAIT_EOC,          // Wait for the charge done
    RECHARGING_CYCLE,  // During the recharging cycle state
    OVER_LOADING,      // When system power draw is higher than what charges the battery
    NO_POWER_SOURCE,   // No power source detected
    INACTIVE           // Not active the recharging state checking
};

struct sysfsStringEnumMap {
    const char *s;
    int val;
};

/**
 * updateBatteryProperties is called with the active Fuel Gauge properties values.
 * In bluecross case, the Maxim FG. The charger SOC level should not be considered
 * and cannot be used to make any assumption on the SOC we should reporting to the
 * user.
 *
 * Once 100% is reached, the class will track the charger status to detect
 * when reaching charge termination (EOC), then will report 100% as long as
 * in Full or Charging states.
 * The power source might be disconnected at any FG level, recharge_soc_ is used
 * to keep track of that level to be reached within defined transition time.
 */
class BatteryRechargingControl {
  public:
    BatteryRechargingControl();
    void updateBatteryProperties(struct android::BatteryProperties *props);

  private:
    enum RechargeState state_;
    int64_t start_time_;
    /* Keeps track of the target level to detect OVER_LOADING or 0 when no target is set */
    int recharge_soc_;

    int mapSysfsString(const char *str, struct sysfsStringEnumMap map[]);
    int getBatteryStatus(const char *status);
    int RemapSOC(int);
    int64_t getTime();
};

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device

#endif
