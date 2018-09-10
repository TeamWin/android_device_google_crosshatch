
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

#include "BatteryMetricsLogger.h"

namespace device {
namespace google {
namespace crosshatch {
namespace health {

using ::hardware::google::pixelstats::V1_0::IPixelStats;
using BatterySnapshotType = ::hardware::google::pixelstats::V1_0::IPixelStats::BatterySnapshotType;
using BatteryHealthSnapshotArgs =
    ::hardware::google::pixelstats::V1_0::IPixelStats::BatteryHealthSnapshotArgs;
using android::sp;

BatteryMetricsLogger::BatteryMetricsLogger() {
    last_sample_ = 0;
    last_upload_ = 0;
    num_res_samples_ = 0;
    num_samples_ = 0;
    memset(min_, 0, sizeof(min_));
    memset(max_, 0, sizeof(max_));
    accum_resistance_ = 0;
}

int64_t BatteryMetricsLogger::getTime(void) {
    return nanoseconds_to_seconds(systemTime(SYSTEM_TIME_BOOTTIME));
}

bool BatteryMetricsLogger::uploadOutlierMetric(sp<IPixelStats> client, sampleType type) {
    BatteryHealthSnapshotArgs min_ss = {static_cast<BatterySnapshotType>(0),
                                        min_[type][TEMP],
                                        min_[type][VOLT],
                                        min_[type][CURR],
                                        min_[type][OCV],
                                        min_[type][RES],
                                        min_[type][SOC]};
    BatteryHealthSnapshotArgs max_ss = {static_cast<BatterySnapshotType>(0),
                                        max_[type][TEMP],
                                        max_[type][VOLT],
                                        max_[type][CURR],
                                        max_[type][OCV],
                                        max_[type][RES],
                                        max_[type][SOC]};
    switch (type) {
        case TEMP:
            min_ss.type = BatterySnapshotType::MIN_TEMP;
            max_ss.type = BatterySnapshotType::MAX_TEMP;
            break;
        case RES:
            min_ss.type = BatterySnapshotType::MIN_RESISTANCE;
            max_ss.type = BatterySnapshotType::MAX_RESISTANCE;
            break;
        case VOLT:
            min_ss.type = BatterySnapshotType::MIN_VOLTAGE;
            max_ss.type = BatterySnapshotType::MAX_VOLTAGE;
            break;
        case CURR:
            min_ss.type = BatterySnapshotType::MIN_CURRENT;
            max_ss.type = BatterySnapshotType::MAX_CURRENT;
            break;
        case SOC:
            min_ss.type = BatterySnapshotType::MIN_BATT_LEVEL;
            max_ss.type = BatterySnapshotType::MAX_BATT_LEVEL;
            break;
        case OCV:
        case TIME:
        case NUM_FIELDS:
        default:
            LOG(ERROR) << "Can't upload metric of type " << std::to_string(type);
            return false;
    }

    client->reportBatteryHealthSnapshot(min_ss);
    client->reportBatteryHealthSnapshot(max_ss);
    return true;
}

bool BatteryMetricsLogger::uploadMetrics(void) {
    int64_t time = getTime();
    int32_t avg_resistance = 0;

    if (last_sample_ == 0)
        return false;

    LOG(INFO) << "Uploading metrics at time " << std::to_string(time) << " w/ "
              << std::to_string(num_samples_) << " samples";

    if (num_res_samples_)
        avg_resistance = accum_resistance_ / num_res_samples_;

    LOG(INFO) << "Logging metrics to tron";

    sp<IPixelStats> client = IPixelStats::tryGetService();
    if (!client) {
        LOG(ERROR) << "Unable to connect to PixelStats service";
        return false;
    }

    // Only log and upload the min and max for metric types we want to upload
    for (int metric = kMetricMin; metric <= kMetricMax; metric++) {
        if (metric == RES && num_res_samples_ == 0)
            continue;
        std::string log_min = "min-" + std::to_string(metric) + " ";
        std::string log_max = "max-" + std::to_string(metric) + " ";
        for (int j = 0; j < NUM_FIELDS; j++) {
            log_min += std::to_string(min_[metric][j]) + " ";
            log_max += std::to_string(max_[metric][j]) + " ";
        }
        LOG(INFO) << log_min;
        LOG(INFO) << log_max;
        // Upload min/max metrics
        uploadOutlierMetric(client, static_cast<sampleType>(metric));
    }

    // Upload average metric
    BatteryHealthSnapshotArgs avg_res_ss = {
        BatterySnapshotType::AVG_RESISTANCE, 0, 0, 0, 0, avg_resistance, 0};
    if (num_res_samples_)
        client->reportBatteryHealthSnapshot(avg_res_ss);

    // Clear existing data
    memset(min_, 0, sizeof(min_));
    memset(max_, 0, sizeof(max_));
    num_res_samples_ = 0;
    num_samples_ = 0;
    last_upload_ = time;
    accum_resistance_ = 0;
    LOG(INFO) << "Finished uploading to tron";
    return true;
}

bool BatteryMetricsLogger::recordSample(struct android::BatteryProperties *props) {
    std::string resistance_str, ocv_str;
    int32_t resistance, ocv;
    int32_t time = getTime();

    LOG(INFO) << "Recording a sample at time " << std::to_string(time);

    if (!android::base::ReadFileToString(kBatteryResistance, &resistance_str)) {
        LOG(ERROR) << "Can't read the battery resistance";
        resistance = 0;
    } else if (!(resistance = stoi(resistance_str))) {
        LOG(ERROR) << "Can't parse battery resistance value " << resistance_str;
    }

    if (!android::base::ReadFileToString(kBatteryOCV, &ocv_str)) {
        LOG(ERROR) << "Can't read the open-circuit voltage (ocv) value";
        ocv = 0;
    } else if (!(ocv = stoi(ocv_str))) {
        LOG(ERROR) << "Can't parse open-circuit voltage (ocv) value " << ocv_str;
    }

    int32_t sample[NUM_FIELDS] = {time,
                                  resistance,
                                  props->batteryCurrent,
                                  props->batteryVoltage,
                                  props->batteryTemperature,
                                  props->batteryLevel,
                                  ocv};
    if (props->batteryStatus != android::BATTERY_STATUS_CHARGING) {
        accum_resistance_ += resistance;
        num_res_samples_++;
    }

    // Only calculate the min and max for metric types we want to upload
    for (int metric = kMetricMin; metric <= kMetricMax; metric++) {
        // Discard resistance min/max when charging
        if (metric == RES && props->batteryStatus == android::BATTERY_STATUS_CHARGING)
            continue;
        if (num_samples_ == 0 || (metric == RES && num_res_samples_ == 0) ||
            sample[metric] < min_[metric][metric]) {
            for (int i = 0; i < NUM_FIELDS; i++) {  // update new min with current sample
                min_[metric][i] = sample[i];
            }
        }
        if (num_samples_ == 0 || (metric == RES && num_res_samples_ == 0) ||
            sample[metric] > max_[metric][metric]) {
            for (int i = 0; i < NUM_FIELDS; i++) {  // update new max with current sample
                max_[metric][i] = sample[i];
            }
        }
    }

    num_samples_++;
    last_sample_ = time;
    return true;
}

void BatteryMetricsLogger::logBatteryProperties(struct android::BatteryProperties *props) {
    int32_t time = getTime();
    if (last_sample_ == 0 || time - last_sample_ >= kSamplePeriod)
        recordSample(props);
    if (last_sample_ - last_upload_ > kUploadPeriod || num_samples_ >= kMaxSamples)
        uploadMetrics();

    return;
}

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device
