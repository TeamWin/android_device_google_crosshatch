#include "PowerStats.h"
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>
#include <inttypes.h>
#include <stdlib.h>
#include <algorithm>
#include <exception>
#include <thread>

namespace android {
namespace hardware {
namespace power {
namespace stats {
namespace V1_0 {
namespace implementation {

PowerStats::PowerStats() {}

Return<void> PowerStats::getRailInfo(getRailInfo_cb _hidl_cb) {
    hidl_vec<RailInfo> rInfo;
    _hidl_cb(rInfo, Status::NOT_SUPPORTED);
    return Void();
}

Return<void> PowerStats::getEnergyData(const hidl_vec<uint32_t> &railIndices,
                                       getEnergyData_cb _hidl_cb) {
    (void)railIndices;
    hidl_vec<EnergyData> eVal;
    _hidl_cb(eVal, Status::NOT_SUPPORTED);
    return Void();
}

Return<void> PowerStats::streamEnergyData(uint32_t timeMs, uint32_t samplingRate,
                                          streamEnergyData_cb _hidl_cb) {
    (void)timeMs;
    (void)samplingRate;
    _hidl_cb(MessageQueueSync::Descriptor(), 0, 0, Status::NOT_SUPPORTED);
    return Void();
}

Return<void> PowerStats::getPowerEntityInfo(getPowerEntityInfo_cb _hidl_cb) {
    hidl_vec<PowerEntityInfo> eInfo;
    _hidl_cb(eInfo, Status::NOT_SUPPORTED);
    return Void();
}

Return<void> PowerStats::getPowerEntityStateInfo(const hidl_vec<uint32_t> &powerEntityIds,
                                                 getPowerEntityStateInfo_cb _hidl_cb) {
    (void)powerEntityIds;
    hidl_vec<PowerEntityStateSpace> powerEntityStateSpaces;
    _hidl_cb(powerEntityStateSpaces, Status::NOT_SUPPORTED);
    return Void();
}

Return<void> PowerStats::getPowerEntityStateResidencyData(
    const hidl_vec<uint32_t> &powerEntityIds, getPowerEntityStateResidencyData_cb _hidl_cb) {
    (void)powerEntityIds;
    hidl_vec<PowerEntityStateResidencyResult> results;
    _hidl_cb(results, Status::NOT_SUPPORTED);
    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace stats
}  // namespace power
}  // namespace hardware
}  // namespace android
