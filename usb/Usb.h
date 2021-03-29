#pragma once

#include <android-base/file.h>
#include <android/hardware/usb/1.2/IUsbCallback.h>
#include <android/hardware/usb/1.2/types.h>
#include <android/hardware/usb/1.3/IUsb.h>
#include <hidl/Status.h>
#include <utils/Log.h>

#define UEVENT_MSG_LEN 2048
// The type-c stack waits for 4.5 - 5.5 secs before declaring a port non-pd.
// The -partner directory would not be created until this is done.
// Having a margin of ~3 secs for the directory and other related bookeeping
// structures created and uvent fired.
#define PORT_TYPE_TIMEOUT 8

namespace android {
namespace hardware {
namespace usb {
namespace V1_3 {
namespace implementation {

using ::android::sp;
using ::android::base::WriteStringToFile;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::usb::V1_0::PortDataRole;
using ::android::hardware::usb::V1_0::PortPowerRole;
using ::android::hardware::usb::V1_0::PortRole;
using ::android::hardware::usb::V1_0::PortRoleType;
using ::android::hardware::usb::V1_0::Status;
using ::android::hardware::usb::V1_1::PortMode_1_1;
using ::android::hardware::usb::V1_1::PortStatus_1_1;
using ::android::hardware::usb::V1_2::IUsbCallback;
using ::android::hardware::usb::V1_2::PortStatus;
using ::android::hardware::usb::V1_3::IUsb;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;

#define PULLUP_PATH "/config/usb_gadget/g1/UDC"
constexpr char kGadgetName[] = "a600000.dwc3";
#define SOC_PATH "/sys/devices/platform/soc/a600000.ssusb/"
#define ID_PATH SOC_PATH "id"
#define VBUS_PATH SOC_PATH "b_sess"
#define USB_DATA_PATH SOC_PATH "usb_data_enabled"

enum class HALVersion { V1_0, V1_1, V1_2, V1_3 };

struct Usb : public IUsb {
    Usb();

    Return<void> switchRole(const hidl_string &portName, const PortRole &role) override;
    Return<void> setCallback(const sp<V1_0::IUsbCallback>& callback) override;
    Return<void> queryPortStatus() override;
    Return<void> enableContaminantPresenceDetection(const hidl_string &portName, bool enable);
    Return<void> enableContaminantPresenceProtection(const hidl_string &portName, bool enable);
    Return<bool> enableUsbDataSignal(bool enable) override;

    sp<V1_0::IUsbCallback> mCallback_1_0;
    // Protects mCallback variable
    pthread_mutex_t mLock;
    // Protects roleSwitch operation
    pthread_mutex_t mRoleSwitchLock;
    // Threads waiting for the partner to come back wait here
    pthread_cond_t mPartnerCV;
    // lock protecting mPartnerCV
    pthread_mutex_t mPartnerLock;
    // Variable to signal partner coming back online after type switch
    bool mPartnerUp;

    private:
        pthread_t mPoll;
};

}  // namespace implementation
}  // namespace V1_3
}  // namespace usb
}  // namespace hardware
}  // namespace android
