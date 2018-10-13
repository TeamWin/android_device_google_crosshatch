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

#define ATRACE_TAG (ATRACE_TAG_POWER | ATRACE_TAG_HAL)
#define LOG_TAG "android.hardware.power@1.3-service.crosshatch-libperfmgr"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>
#include <android-base/stringprintf.h>

#include <pixelpowerstats/Debugging.h>

#include <mutex>

#include <utils/Log.h>
#include <utils/Trace.h>

#include "Power.h"
#include "power-helper.h"
#include "display-helper.h"

/* RPM runs at 19.2Mhz. Divide by 19200 for msec */
#define RPM_CLK 19200

extern struct stats_section master_sections[];

namespace android {
namespace hardware {
namespace power {
namespace V1_3 {
namespace implementation {

using ::android::hardware::power::V1_0::Feature;
using ::android::hardware::power::V1_0::PowerStatePlatformSleepState;
using ::android::hardware::power::V1_0::Status;
using ::android::hardware::power::V1_1::PowerStateSubsystem;
using ::android::hardware::power::V1_1::PowerStateSubsystemSleepState;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::hardware::google::pixel::powerstats::DumpPowerHal1_0PlatStatsToFd;
using ::hardware::google::pixel::powerstats::DumpPowerHal1_1SubsysStatsToFd;

Power::Power() :
        mHintManager(nullptr),
        mInteractionHandler(nullptr),
        mVRModeOn(false),
        mSustainedPerfModeOn(false),
        mCameraStreamingModeOn(false),
        mReady(false) {

    mInitThread =
            std::thread([this](){
                            android::base::WaitForProperty(kPowerHalInitProp, "1");
                            mHintManager = HintManager::GetFromJSON("/vendor/etc/powerhint.json");
                            mInteractionHandler = std::make_unique<InteractionHandler>(mHintManager);
                            mInteractionHandler->Init();
                            std::string state = android::base::GetProperty(kPowerHalStateProp, "");
                            if (state == "CAMERA_STREAMING") {
                                ALOGI("Initialize with CAMERA_STREAMING on");
                                mHintManager->DoHint("CAMERA_STREAMING");
                                mCameraStreamingModeOn = true;
                            } else if (state ==  "SUSTAINED_PERFORMANCE") {
                                ALOGI("Initialize with SUSTAINED_PERFORMANCE on");
                                mHintManager->DoHint("SUSTAINED_PERFORMANCE");
                                mSustainedPerfModeOn = true;
                            } else if (state == "VR_MODE") {
                                ALOGI("Initialize with VR_MODE on");
                                mHintManager->DoHint("VR_MODE");
                                mVRModeOn = true;
                            } else if (state == "VR_SUSTAINED_PERFORMANCE") {
                                ALOGI("Initialize with SUSTAINED_PERFORMANCE and VR_MODE on");
                                mHintManager->DoHint("VR_SUSTAINED_PERFORMANCE");
                                mSustainedPerfModeOn = true;
                                mVRModeOn = true;
                            } else {
                                ALOGI("Initialize PowerHAL");
                            }

                            state = android::base::GetProperty(kPowerHalAudioProp, "");
                            if (state == "LOW_LATENCY") {
                                ALOGI("Initialize with AUDIO_LOW_LATENCY on");
                                mHintManager->DoHint("AUDIO_LOW_LATENCY");
                            }

                            state = android::base::GetProperty(kPowerHalRenderingProp, "");
                            if (state == "EXPENSIVE_RENDERING") {
                                ALOGI("Initialize with EXPENSIVE_RENDERING on");
                                mHintManager->DoHint("EXPENSIVE_RENDERING");
                            }
                            // Now start to take powerhint
                            mReady.store(true);
                        });
    mInitThread.detach();

}

// Methods from ::android::hardware::power::V1_0::IPower follow.
Return<void> Power::setInteractive(bool /* interactive */)  {
    return Void();
}

Return<void> Power::powerHint(PowerHint_1_0 hint, int32_t data) {
    if (!isSupportedGovernor() || !mReady) {
        return Void();
    }

    switch(hint) {
        case PowerHint_1_0::INTERACTION:
            if (mVRModeOn || mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                mInteractionHandler->Acquire(data);
            }
            break;
        case PowerHint_1_0::SUSTAINED_PERFORMANCE:
            if (data && !mSustainedPerfModeOn) {
                ALOGD("SUSTAINED_PERFORMANCE ON");
                if (!mVRModeOn) { // Sustained mode only.
                    mHintManager->DoHint("SUSTAINED_PERFORMANCE");
                    if (!android::base::SetProperty(kPowerHalStateProp, "SUSTAINED_PERFORMANCE")) {
                        ALOGE("%s: could not set powerHAL state property to SUSTAINED_PERFORMANCE", __func__);
                    }
                } else { // Sustained + VR mode.
                    mHintManager->EndHint("VR_MODE");
                    mHintManager->DoHint("VR_SUSTAINED_PERFORMANCE");
                    if (!android::base::SetProperty(kPowerHalStateProp, "VR_SUSTAINED_PERFORMANCE")) {
                        ALOGE("%s: could not set powerHAL state property to VR_SUSTAINED_PERFORMANCE", __func__);
                    }
                }
                mSustainedPerfModeOn = true;
            } else if (!data && mSustainedPerfModeOn) {
                ALOGD("SUSTAINED_PERFORMANCE OFF");
                mHintManager->EndHint("VR_SUSTAINED_PERFORMANCE");
                mHintManager->EndHint("SUSTAINED_PERFORMANCE");
                if (mVRModeOn) { // Switch back to VR Mode.
                    mHintManager->DoHint("VR_MODE");
                    if (!android::base::SetProperty(kPowerHalStateProp, "VR_MODE")) {
                        ALOGE("%s: could not set powerHAL state property to VR_MODE", __func__);
                    }
                } else {
                    if (!android::base::SetProperty(kPowerHalStateProp, "")) {
                        ALOGE("%s: could not clear powerHAL state property", __func__);
                    }
                }
                mSustainedPerfModeOn = false;
            }
            break;
        case PowerHint_1_0::VR_MODE:
            if (data && !mVRModeOn) {
                ALOGD("VR_MODE ON");
                setVrModeThermalConfig(true);
                if (!mSustainedPerfModeOn) { // VR mode only.
                    mHintManager->DoHint("VR_MODE");
                    if (!android::base::SetProperty(kPowerHalStateProp, "VR_MODE")) {
                        ALOGE("%s: could not set powerHAL state property to VR_MODE", __func__);
                    }
                } else { // Sustained + VR mode.
                    mHintManager->EndHint("SUSTAINED_PERFORMANCE");
                    mHintManager->DoHint("VR_SUSTAINED_PERFORMANCE");
                    if (!android::base::SetProperty(kPowerHalStateProp, "VR_SUSTAINED_PERFORMANCE")) {
                        ALOGE("%s: could not set powerHAL state property to VR_SUSTAINED_PERFORMANCE", __func__);
                    }
                }
                mVRModeOn = true;
            } else if (!data && mVRModeOn) {
                ALOGD("VR_MODE OFF");
                setVrModeThermalConfig(false);
                mHintManager->EndHint("VR_SUSTAINED_PERFORMANCE");
                mHintManager->EndHint("VR_MODE");
                if (mSustainedPerfModeOn) { // Switch back to sustained Mode.
                    mHintManager->DoHint("SUSTAINED_PERFORMANCE");
                    if (!android::base::SetProperty(kPowerHalStateProp, "SUSTAINED_PERFORMANCE")) {
                        ALOGE("%s: could not set powerHAL state property to SUSTAINED_PERFORMANCE", __func__);
                    }
                } else {
                    if (!android::base::SetProperty(kPowerHalStateProp, "")) {
                        ALOGE("%s: could not clear powerHAL state property", __func__);
                    }
                }
                mVRModeOn = false;
            }
            break;
        case PowerHint_1_0::LAUNCH:
            ATRACE_BEGIN("launch");
            if (mVRModeOn || mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                if (data) {
                    // Hint until canceled
                    ATRACE_INT("launch_lock", 1);
                    mHintManager->DoHint("LAUNCH");
                    ALOGD("LAUNCH ON");
                } else {
                    ATRACE_INT("launch_lock", 0);
                    mHintManager->EndHint("LAUNCH");
                    ALOGD("LAUNCH OFF");
                }
            }
            ATRACE_END();
            break;
        case PowerHint_1_0::LOW_POWER:
            {
            ATRACE_BEGIN("low-power");

            if (data) {
              // Device in battery saver mode, enable display low power mode
              set_display_lpm(true);
            } else {
              // Device exiting battery saver mode, disable display low power mode
              set_display_lpm(false);
            }
            ATRACE_END();
            }
            break;
        default:
            break;

    }
    return Void();
}

Return<void> Power::setFeature(Feature /*feature*/, bool /*activate*/)  {
    //Nothing to do
    return Void();
}

Return<void> Power::getPlatformLowPowerStats(getPlatformLowPowerStats_cb _hidl_cb) {

    hidl_vec<PowerStatePlatformSleepState> states;
    uint64_t stats[SYSTEM_SLEEP_STATE_COUNT * SYSTEM_STATE_STATS_COUNT] = {0};
    uint64_t *state_stats;
    struct PowerStatePlatformSleepState *state;

    states.resize(SYSTEM_SLEEP_STATE_COUNT);

    if (extract_system_stats(stats, ARRAY_SIZE(stats)) != 0) {
        states.resize(0);
        goto done;
    }

    /* Update statistics for AOSD */
    state = &states[SYSTEM_STATE_AOSD];
    state->name = "AOSD";
    state_stats = &stats[SYSTEM_STATE_AOSD * SYSTEM_STATE_STATS_COUNT];

    state->residencyInMsecSinceBoot = state_stats[ACCUMULATED_TIME_MS];
    state->totalTransitions = state_stats[TOTAL_COUNT];
    state->supportedOnlyInSuspend = false;
    state->voters.resize(0);

    /* Update statistics for CXSD */
    state = &states[SYSTEM_STATE_CXSD];
    state->name = "CXSD";
    state_stats = &stats[SYSTEM_STATE_CXSD * SYSTEM_STATE_STATS_COUNT];

    state->residencyInMsecSinceBoot = state_stats[ACCUMULATED_TIME_MS];
    state->totalTransitions = state_stats[TOTAL_COUNT];
    state->supportedOnlyInSuspend = false;
    state->voters.resize(0);

done:
    _hidl_cb(states, Status::SUCCESS);
    return Void();
}

static int get_master_low_power_stats(hidl_vec<PowerStateSubsystem> *subsystems) {
    uint64_t all_stats[MASTER_COUNT * MASTER_STATS_COUNT] = {0};
    uint64_t *section_stats;
    struct PowerStateSubsystem *subsystem;
    struct PowerStateSubsystemSleepState *state;

    if (extract_master_stats(all_stats, ARRAY_SIZE(all_stats)) != 0) {
        for (size_t i = 0; i < MASTER_COUNT; i++) {
            (*subsystems)[i].name = master_sections[i].label;
            (*subsystems)[i].states.resize(0);
        }
        return -1;
    }

    for (size_t i = 0; i < MASTER_COUNT; i++) {
        subsystem = &(*subsystems)[i];
        subsystem->name = master_sections[i].label;
        subsystem->states.resize(MASTER_SLEEP_STATE_COUNT);

        state = &(subsystem->states[MASTER_SLEEP]);
        section_stats = &all_stats[i * MASTER_STATS_COUNT];

        state->name = "Sleep";
        state->totalTransitions = section_stats[SLEEP_ENTER_COUNT];
        state->residencyInMsecSinceBoot = section_stats[SLEEP_CUMULATIVE_DURATION_MS] / RPM_CLK;
        state->lastEntryTimestampMs = section_stats[SLEEP_LAST_ENTER_TSTAMP_MS] / RPM_CLK;
        state->supportedOnlyInSuspend = false;
    }

    return 0;
}

static int get_wlan_low_power_stats(struct PowerStateSubsystem *subsystem) {
    uint64_t stats[WLAN_STATS_COUNT] = {0};
    struct PowerStateSubsystemSleepState *state;

    subsystem->name = "wlan";

    if (extract_wlan_stats(stats, ARRAY_SIZE(stats)) != 0) {
        subsystem->states.resize(0);
        return -1;
    }

    subsystem->states.resize(WLAN_SLEEP_STATE_COUNT);

    /* Update statistics for Active State */
    state = &subsystem->states[WLAN_STATE_ACTIVE];
    state->name = "Active";
    state->residencyInMsecSinceBoot = stats[CUMULATIVE_TOTAL_ON_TIME_MS];
    state->totalTransitions = stats[DEEP_SLEEP_ENTER_COUNTER];
    state->lastEntryTimestampMs = 0; //FIXME need a new value from Qcom
    state->supportedOnlyInSuspend = false;

    /* Update statistics for Deep-Sleep state */
    state = &subsystem->states[WLAN_STATE_DEEP_SLEEP];
    state->name = "Deep-Sleep";
    state->residencyInMsecSinceBoot = stats[CUMULATIVE_SLEEP_TIME_MS];
    state->totalTransitions = stats[DEEP_SLEEP_ENTER_COUNTER];
    state->lastEntryTimestampMs = stats[LAST_DEEP_SLEEP_ENTER_TSTAMP_MS];
    state->supportedOnlyInSuspend = false;

    return 0;
}

static const std::string get_easel_state_name(int state) {
    if (state == EASEL_OFF) {
        return "Off";
    } else if (state == EASEL_ACTIVE) {
        return "Active";
    } else if (state == EASEL_SUSPEND) {
        return "Suspend";
    } else {
        return "Unknown";
    }
}

// Get low power stats for easel subsystem
static int get_easel_low_power_stats(struct PowerStateSubsystem *subsystem) {
    uint64_t stats[EASEL_SLEEP_STATE_COUNT * EASEL_STATS_COUNT] = {0};
    uint64_t *state_stats;
    struct PowerStateSubsystemSleepState *state;

    subsystem->name = "Easel";

    if (extract_easel_stats(stats, ARRAY_SIZE(stats)) != 0) {
        subsystem->states.resize(0);
        return -1;
    }

    subsystem->states.resize(EASEL_SLEEP_STATE_COUNT);

    for (int easel_state = 0; easel_state < EASEL_SLEEP_STATE_COUNT; easel_state++) {
        state = &subsystem->states[easel_state];
        state_stats = &stats[easel_state * EASEL_STATS_COUNT];

        state->name = get_easel_state_name(easel_state);
        state->residencyInMsecSinceBoot = state_stats[CUMULATIVE_DURATION_MS];
        state->totalTransitions = state_stats[CUMULATIVE_COUNT];
        state->lastEntryTimestampMs = state_stats[LAST_ENTRY_TSTAMP_MS];
        state->supportedOnlyInSuspend = false;
    }

    return 0;
}

// Methods from ::android::hardware::power::V1_1::IPower follow.
Return<void> Power::getSubsystemLowPowerStats(getSubsystemLowPowerStats_cb _hidl_cb) {
    hidl_vec<PowerStateSubsystem> subsystems;

    subsystems.resize(STATS_SOURCE_COUNT);

    // Get low power stats for all of the system masters.
    if (get_master_low_power_stats(&subsystems) != 0) {
        ALOGE("%s: failed to process master stats", __func__);
    }

    // Get WLAN subsystem low power stats.
    if (get_wlan_low_power_stats(&subsystems[SUBSYSTEM_WLAN]) != 0) {
        ALOGE("%s: failed to process wlan stats", __func__);
    }

    // Get Easel subsystem low power stats.
    if (get_easel_low_power_stats(&subsystems[SUBSYSTEM_EASEL]) != 0) {
        ALOGE("%s: failed to process Easel stats", __func__);
    }

    _hidl_cb(subsystems, Status::SUCCESS);
    return Void();
}

bool Power::isSupportedGovernor() {
    std::string buf;
    if (android::base::ReadFileToString("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", &buf)) {
        buf = android::base::Trim(buf);
    }
    // Only support EAS 1.2, legacy EAS
    if (buf == "schedutil" || buf == "sched") {
        return true;
    } else {
        LOG(ERROR) << "Governor not supported by powerHAL, skipping";
        return false;
    }
}

bool Power::setVrModeThermalConfig(bool enabled) {
    std::string vrMode = enabled ? "-vr" : "-novr";

    if (!android::base::SetProperty("vendor.thermal.vr_mode", vrMode)) {
        LOG(ERROR) << "Couldn't set vendor.thermal.vr_mode to \"" << vrMode << "\"";
        return false;
    }
    if (!android::base::SetProperty("ctl.restart", "vendor.thermal-engine")) {
        LOG(ERROR) << "Couldn't set thermal_engine restart property";
        return false;
    }
    return true;
}

Return<void> Power::powerHintAsync(PowerHint_1_0 hint, int32_t data) {
    // just call the normal power hint in this oneway function
    return powerHint(hint, data);
}

// Methods from ::android::hardware::power::V1_2::IPower follow.
Return<void> Power::powerHintAsync_1_2(PowerHint_1_2 hint, int32_t data) {
    if (!isSupportedGovernor() || !mReady) {
        return Void();
    }

    switch(hint) {
        case PowerHint_1_2::AUDIO_LOW_LATENCY:
            ATRACE_BEGIN("audio_low_latency");
            if (data) {
                // Hint until canceled
                ATRACE_INT("audio_low_latency_lock", 1);
                mHintManager->DoHint("AUDIO_LOW_LATENCY");
                ALOGD("AUDIO LOW LATENCY ON");
                if (!android::base::SetProperty(kPowerHalAudioProp, "LOW_LATENCY")) {
                    ALOGE("%s: could not set powerHAL audio state property to LOW_LATENCY", __func__);
                }
            } else {
                ATRACE_INT("audio_low_latency_lock", 0);
                mHintManager->EndHint("AUDIO_LOW_LATENCY");
                ALOGD("AUDIO LOW LATENCY OFF");
                if (!android::base::SetProperty(kPowerHalAudioProp, "")) {
                    ALOGE("%s: could not clear powerHAL audio state property", __func__);
                }
            }
            ATRACE_END();
            break;
        case PowerHint_1_2::AUDIO_STREAMING:
            ATRACE_BEGIN("audio_streaming");
            if (mVRModeOn || mSustainedPerfModeOn) {
                ALOGV("%s: ignoring due to other active perf hints", __func__);
            } else {
                if (data) {
                    // Hint until canceled
                    ATRACE_INT("audio_streaming_lock", 1);
                    mHintManager->DoHint("AUDIO_STREAMING");
                    ALOGD("AUDIO STREAMING ON");
                } else {
                    ATRACE_INT("audio_streaming_lock", 0);
                    mHintManager->EndHint("AUDIO_STREAMING");
                    ALOGD("AUDIO STREAMING OFF");
                }
            }
            ATRACE_END();
            break;
        case PowerHint_1_2::CAMERA_LAUNCH:
            ATRACE_BEGIN("camera_launch");
            if (data > 0) {
                ATRACE_INT("camera_launch_lock", 1);
                mHintManager->DoHint("CAMERA_LAUNCH", std::chrono::milliseconds(data));
                ALOGD("CAMERA LAUNCH ON: %d MS, LAUNCH ON: 2500 MS", data);
                // boosts 2.5s for launching
                mHintManager->DoHint("LAUNCH", std::chrono::milliseconds(2500));
            } else if (data == 0) {
                ATRACE_INT("camera_launch_lock", 0);
                mHintManager->EndHint("CAMERA_LAUNCH");
                ALOGD("CAMERA LAUNCH OFF");
            } else {
                ALOGE("CAMERA LAUNCH INVALID DATA: %d", data);
            }
            ATRACE_END();
            break;
        case PowerHint_1_2::CAMERA_STREAMING:
            ATRACE_BEGIN("camera_streaming");
            if (data > 0) {
                ATRACE_INT("camera_streaming_lock", 1);
                mHintManager->DoHint("CAMERA_STREAMING");
                ALOGD("CAMERA STREAMING ON");
                if (!android::base::SetProperty(kPowerHalStateProp, "CAMERA_STREAMING")) {
                    ALOGE("%s: could not set powerHAL state property to CAMERA_STREAMING", __func__);
                }
                mCameraStreamingModeOn = true;
            } else if (data == 0) {
                ATRACE_INT("camera_streaming_lock", 0);
                mHintManager->EndHint("CAMERA_STREAMING");
                // Boost 1s for tear down
                mHintManager->DoHint("CAMERA_LAUNCH", std::chrono::seconds(1));
                ALOGD("CAMERA STREAMING OFF");
                if (!android::base::SetProperty(kPowerHalStateProp, "")) {
                    ALOGE("%s: could not clear powerHAL state property", __func__);
                }
                mCameraStreamingModeOn = false;
            } else {
                ALOGE("CAMERA STREAMING INVALID DATA: %d", data);
            }
            ATRACE_END();
            break;
        case PowerHint_1_2::CAMERA_SHOT:
            ATRACE_BEGIN("camera_shot");
            if (data > 0) {
                ATRACE_INT("camera_shot_lock", 1);
                mHintManager->DoHint("CAMERA_SHOT", std::chrono::milliseconds(data));
                ALOGD("CAMERA SHOT ON: %d MS", data);
            } else if (data == 0) {
                ATRACE_INT("camera_shot_lock", 0);
                mHintManager->EndHint("CAMERA_SHOT");
                ALOGD("CAMERA SHOT OFF");
            } else {
                ALOGE("CAMERA SHOT INVALID DATA: %d", data);
            }
            ATRACE_END();
            break;
        default:
            return powerHint(static_cast<PowerHint_1_0>(hint), data);
    }
    return Void();
}

// Methods from ::android::hardware::power::V1_3::IPower follow.
Return<void> Power::powerHintAsync_1_3(PowerHint_1_3 hint, int32_t data) {
    if (!isSupportedGovernor() || !mReady) {
        return Void();
    }

    if (hint == PowerHint_1_3::EXPENSIVE_RENDERING) {
        if (mVRModeOn || mSustainedPerfModeOn) {
            ALOGV("%s: ignoring due to other active perf hints", __func__);
            return Void();
        }

        if (data > 0) {
            ATRACE_INT("EXPENSIVE_RENDERING", 1);
            mHintManager->DoHint("EXPENSIVE_RENDERING");
            if (!android::base::SetProperty(kPowerHalRenderingProp, "EXPENSIVE_RENDERING")) {
                ALOGE("%s: could not set powerHAL rendering property to EXPENSIVE_RENDERING",
                      __func__);
            }
        } else {
            ATRACE_INT("EXPENSIVE_RENDERING", 0);
            mHintManager->EndHint("EXPENSIVE_RENDERING");
            if (!android::base::SetProperty(kPowerHalRenderingProp, "")) {
                ALOGE("%s: could not clear powerHAL rendering property", __func__);
            }
        }
    } else {
        return powerHintAsync_1_2(static_cast<PowerHint_1_2>(hint), data);
    }

    return Void();
}

constexpr const char* boolToString(bool b) {
    return b ? "true" : "false";
}

Return<void> Power::debug(const hidl_handle& handle, const hidl_vec<hidl_string>&) {
    if (handle != nullptr && handle->numFds >= 1 && mReady) {
        int fd = handle->data[0];

        std::string buf(android::base::StringPrintf("HintManager Running: %s\n"
                                                    "VRMode: %s\n"
                                                    "CameraStreamingMode: %s\n"
                                                    "SustainedPerformanceMode: %s\n",
                                                    boolToString(mHintManager->IsRunning()),
                                                    boolToString(mVRModeOn),
                                                    boolToString(mCameraStreamingModeOn),
                                                    boolToString(mSustainedPerfModeOn)));
        // Dump nodes through libperfmgr
        mHintManager->DumpToFd(fd);
        if (!android::base::WriteStringToFd(buf, fd)) {
            PLOG(ERROR) << "Failed to dump state to fd";
        }

        // Dump platform low power stats
        getPlatformLowPowerStats([fd](const auto& platStats, const auto result) {
            if (!DumpPowerHal1_0PlatStatsToFd(result, platStats, fd)) {
                PLOG(ERROR) << "Failed to dump platform low power stats to fd";
            }
        });

        // Dump subsystem low power stats
        getSubsystemLowPowerStats([fd](const auto& subsysStats, const auto result) {
            if (!DumpPowerHal1_1SubsysStatsToFd(result, subsysStats, fd)) {
                PLOG(ERROR) << "Failed to dump subsystem low power stats to fd";
            }
        });

        fsync(fd);
    }
    return Void();
}

}  // namespace implementation
}  // namespace V1_3
}  // namespace power
}  // namespace hardware
}  // namespace android
