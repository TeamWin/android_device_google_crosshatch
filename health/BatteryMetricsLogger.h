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

#ifndef DEVICE_GOOGLE_CROSSHATCH_HEALTH_BATTERYMETRICSLOGGER_H
#define DEVICE_GOOGLE_CROSSHATCH_HEALTH_BATTERYMETRICSLOGGER_H

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <batteryservice/BatteryService.h>
#include <math.h>
#include <time.h>
#include <utils/Timers.h>
#include <string>

#include <hardware/google/pixelstats/1.0/IPixelStats.h>

namespace device {
namespace google {
namespace crosshatch {
namespace health {

class BatteryMetricsLogger {
  public:
    BatteryMetricsLogger();
    void logBatteryProperties(struct android::BatteryProperties *props);

  private:
    enum sampleType {
        TIME,        // time in seconds
        RES,         // resistance in milli-ohms
        CURR,        // current in mA
        VOLT,        // voltage in mV
        TEMP,        // temp in deci-degC
        SOC,         // SoC in % battery level
        OCV,         // open-circuit voltage in mV
        NUM_FIELDS,  // do not reference
    };

    const char *kBatteryResistance = "/sys/class/power_supply/maxfg/resistance";
    const char *kBatteryOCV = "/sys/class/power_supply/maxfg/voltage_ocv";
    const int kSamplePeriod = 10 * 60;       // 10 minutes
    const int kUploadPeriod = 24 * 60 * 60;  // 1 day
    const int kMaxSamples = 144;             // 24h * 60min / 10 min sample rate
    const sampleType kMetricMin = RES, kMetricMax = SOC;

    // min and max are referenced by type in both the X and Y axes
    // i.e. min[TYPE] is the event where the minimum of that type occurred, and
    // min[TYPE][TYPE] is the reading of that type at that minimum event
    int32_t min_[NUM_FIELDS][NUM_FIELDS];
    int32_t max_[NUM_FIELDS][NUM_FIELDS];
    int32_t num_res_samples_;   // number of res samples since last upload
    int32_t num_samples_;       // number of min/max samples since last upload
    int64_t accum_resistance_;  // accumulative resistance
    int64_t last_sample_;       // time in seconds since boot of last sample
    int64_t last_upload_;       // time in seconds since boot of last upload

    int64_t getTime();
    bool recordSample(struct android::BatteryProperties *props);
    bool uploadMetrics();
    bool uploadOutlierMetric(android::sp<::hardware::google::pixelstats::V1_0::IPixelStats> client,
                             sampleType type);
};

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device

#endif
