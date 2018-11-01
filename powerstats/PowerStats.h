#ifndef ANDROID_HARDWARE_POWERSTATS_V1_0_POWERSTATS_H
#define ANDROID_HARDWARE_POWERSTATS_V1_0_POWERSTATS_H

#include <android/hardware/power/stats/1.0/IPowerStats.h>
#include <fmq/MessageQueue.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android {
namespace hardware {
namespace power {
namespace stats {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::power::stats::V1_0::EnergyData;
using ::android::hardware::power::stats::V1_0::PowerEntityInfo;
using ::android::hardware::power::stats::V1_0::PowerEntityStateInfo;
using ::android::hardware::power::stats::V1_0::PowerEntityStateResidencyData;
using ::android::hardware::power::stats::V1_0::PowerEntityStateResidencyResult;
using ::android::hardware::power::stats::V1_0::PowerEntityStateSpace;
using ::android::hardware::power::stats::V1_0::PowerEntityType;
using ::android::hardware::power::stats::V1_0::RailInfo;
using ::android::hardware::power::stats::V1_0::Status;

typedef MessageQueue<EnergyData, kSynchronizedReadWrite> MessageQueueSync;
struct PowerStats : public IPowerStats {
    PowerStats();
    // Methods from ::android::hardware::power::stats::V1_0::IPowerStats follow.
    Return<void> getRailInfo(getRailInfo_cb _hidl_cb) override;
    Return<void> getEnergyData(const hidl_vec<uint32_t> &railIndices,
                               getEnergyData_cb _hidl_cb) override;
    Return<void> streamEnergyData(uint32_t timeMs, uint32_t samplingRate,
                                  streamEnergyData_cb _hidl_cb) override;
    Return<void> getPowerEntityInfo(getPowerEntityInfo_cb _hidl_cb) override;
    Return<void> getPowerEntityStateInfo(const hidl_vec<uint32_t> &powerEntityIds,
                                         getPowerEntityStateInfo_cb _hidl_cb) override;
    Return<void> getPowerEntityStateResidencyData(
        const hidl_vec<uint32_t> &powerEntityIds,
        getPowerEntityStateResidencyData_cb _hidl_cb) override;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace stats
}  // namespace power
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_POWERSTATS_V1_0_POWERSTATS_H
