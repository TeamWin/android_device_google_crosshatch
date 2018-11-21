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

#define LOG_TAG "android.hardware.power.stats@1.0-service.pixel"

#include <android/log.h>
#include <hidl/HidlTransportSupport.h>

#include <pixelpowerstats/GenericStateResidencyDataProvider.h>
#include <pixelpowerstats/PowerStats.h>

using android::OK;
using android::sp;
using android::status_t;

// libhwbinder:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

// Generated HIDL files
using android::hardware::power::stats::V1_0::IPowerStats;
using android::hardware::power::stats::V1_0::PowerEntityInfo;
using android::hardware::power::stats::V1_0::PowerEntityStateSpace;
using android::hardware::power::stats::V1_0::PowerEntityType;
using android::hardware::power::stats::V1_0::implementation::PowerStats;

// Pixel specific
using android::hardware::google::pixel::powerstats::GenericStateResidencyDataProvider;
using android::hardware::google::pixel::powerstats::PowerEntityConfig;
using android::hardware::google::pixel::powerstats::StateResidencyConfig;

// RPM runs at 19.2Mhz. Divide by 19200 for msec
const uint64_t RPM_CLK = 19200;

int main(int /* argc */, char ** /* argv */) {
    ALOGI("power.stats service 1.0 is starting.");

    PowerStats *service = new PowerStats();

    // Add power entities related to rpmh
    uint32_t apssId = service->addPowerEntity("APSS", PowerEntityType::SUBSYSTEM);
    uint32_t mpssId = service->addPowerEntity("MPSS", PowerEntityType::SUBSYSTEM);
    uint32_t adspId = service->addPowerEntity("ADSP", PowerEntityType::SUBSYSTEM);
    uint32_t cdspId = service->addPowerEntity("CDSP", PowerEntityType::SUBSYSTEM);
    uint32_t slpiId = service->addPowerEntity("SLPI", PowerEntityType::SUBSYSTEM);

    auto rpmSdp =
        std::make_shared<GenericStateResidencyDataProvider>("/sys/power/rpmh_stats/master_stats");
    std::function<uint64_t(uint64_t)> rpmConvertToMs = [](uint64_t a) { return a / RPM_CLK; };
    std::vector<StateResidencyConfig> rpmStateResidencyConfigs = {
        {.name = "Sleep",
         .entryCountSupported = true,
         .entryCountPrefix = "Sleep Count:",
         .totalTimeSupported = true,
         .totalTimePrefix = "Sleep Accumulated Duration:",
         .totalTimeTransform = rpmConvertToMs,
         .lastEntrySupported = true,
         .lastEntryPrefix = "Sleep Last Entered At:",
         .lastEntryTransform = rpmConvertToMs}};
    rpmSdp->addEntity(apssId, PowerEntityConfig("APSS", rpmStateResidencyConfigs));
    rpmSdp->addEntity(mpssId, PowerEntityConfig("MPSS", rpmStateResidencyConfigs));
    rpmSdp->addEntity(adspId, PowerEntityConfig("ADSP", rpmStateResidencyConfigs));
    rpmSdp->addEntity(cdspId, PowerEntityConfig("CDSP", rpmStateResidencyConfigs));
    rpmSdp->addEntity(slpiId, PowerEntityConfig("SLPI", rpmStateResidencyConfigs));
    service->addStateResidencyDataProvider(std::move(rpmSdp));

    // Add SoC power entity
    uint32_t socId = service->addPowerEntity("SoC", PowerEntityType::POWER_DOMAIN);
    auto socSdp =
        std::make_shared<GenericStateResidencyDataProvider>("/sys/power/system_sleep/stats");
    std::vector<StateResidencyConfig> socStateResidencyConfigs = {
        {.name = "AOSD",
         .header = "RPM Mode:aosd",
         .entryCountSupported = true,
         .entryCountPrefix = "count:",
         .totalTimeSupported = true,
         .totalTimePrefix = "actual last sleep(msec):",
         .lastEntrySupported = false},
        {.name = "CXSD",
         .header = "RPM Mode:cxsd",
         .entryCountSupported = true,
         .entryCountPrefix = "count:",
         .totalTimeSupported = true,
         .totalTimePrefix = "actual last sleep(msec):",
         .lastEntrySupported = false}};
    socSdp->addEntity(socId, PowerEntityConfig(socStateResidencyConfigs));
    service->addStateResidencyDataProvider(std::move(socSdp));

    // TODO(117887759): Add WLAN power entity

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    status_t status = service->registerAsService();
    if (status != OK) {
        ALOGE("Could not register service for power.stats HAL Iface (%d), exiting.", status);
        return 1;
    }

    ALOGI("power.stats service is ready");
    joinRpcThreadpool();

    // In normal operation, we don't expect the thread pool to exit
    ALOGE("power.stats service is shutting down");
    return 1;
}
