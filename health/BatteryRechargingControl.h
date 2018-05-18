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
#include <string>

namespace device {
namespace google {
namespace crosshatch {
namespace health {

enum RechargeState {
    WAIT_EOC,          // Wait for the charge done.
    RECHARGING_CYCLE,  // During the recharging cycle state
    OVER_LOADING,      // When the power consumption is high than charging
    CABLE_OUT,         // Remove the charging
    INACTIVE           // Not active the recharging state checking.
};

struct sysfsStringEnumMap {
    const char *s;
    int val;
};

class BatteryRechargingControl {
  public:
    BatteryRechargingControl();
    int updateBatteryProperties(struct android::BatteryProperties *props);

  private:
    enum RechargeState state_;
    time_t start_time_;

    int mapSysfsString(const char *str, struct sysfsStringEnumMap map[]);
    int getBatteryStatus(const char *status);
    int RemapSOC(int);
};

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device

#endif
