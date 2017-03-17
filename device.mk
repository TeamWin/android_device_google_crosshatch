#
# Copyright (C) 2016 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony.mk)

PRODUCT_PROPERTY_OVERRIDES += \
    keyguard.no_require_sim=true \
    ro.com.android.dataroaming=true

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml

# Enforce privapp-permissions whitelist
PRODUCT_PROPERTY_OVERRIDES += \
    ro.control_privapp_permissions=enforce

PRODUCT_PACKAGES += \
    messaging

LOCAL_PATH := device/google/wahoo

SRC_MEDIA_HAL_DIR := hardware/qcom/media/msm8998
SRC_DISPLAY_HAL_DIR := hardware/qcom/display/msm8998
SRC_CAMERA_HAL_DIR := hardware/qcom/camera/msm8998

TARGET_SYSTEM_PROP := $(LOCAL_PATH)/system.prop

# Get kernel-headers
$(call inherit-product, hardware/qcom/msm8998/msm8998.mk)

$(call inherit-product, device/google/wahoo/utils.mk)

ifeq ($(TARGET_PREBUILT_KERNEL),)
    LOCAL_KERNEL := device/google/wahoo-kernel/Image.lz4-dtb
else
    LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_CHARACTERISTICS := nosdcard
PRODUCT_SHIPPING_API_LEVEL := 24

DEVICE_PACKAGE_OVERLAYS := $(LOCAL_PATH)/overlay

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel \
    $(LOCAL_PATH)/init.recovery.hardware.rc:root/init.recovery.$(PRODUCT_HARDWARE).rc \
    $(LOCAL_PATH)/init.hardware.rc:root/init.$(PRODUCT_HARDWARE).rc \
    $(LOCAL_PATH)/init.hardware.usb.rc:root/init.$(PRODUCT_HARDWARE).usb.rc \
    $(LOCAL_PATH)/ueventd.hardware.rc:root/ueventd.$(PRODUCT_HARDWARE).rc \
    $(LOCAL_PATH)/init.radio.sh:system/bin/init.radio.sh \
    $(LOCAL_PATH)/uinput-fpc.kl:system/usr/keylayout/uinput-fpc.kl \
    $(LOCAL_PATH)/uinput-fpc.idc:system/usr/idc/uinput-fpc.idc \

ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
  PRODUCT_COPY_FILES += \
      $(LOCAL_PATH)/init.hardware.diag.rc.userdebug:root/init.$(PRODUCT_HARDWARE).diag.rc
else
  PRODUCT_COPY_FILES += \
      $(LOCAL_PATH)/init.hardware.diag.rc.user:root/init.$(PRODUCT_HARDWARE).diag.rc
endif

BOARD_KERNEL_PAGESIZE := 4096
BOARD_FLASH_BLOCK_SIZE := 131072
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2147483648
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_PERSISTIMAGE_PARTITION_SIZE := 33554432
BOARD_PERSISTIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_VENDORIMAGE_PARTITION_SIZE := 419430400
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4

MSM_VIDC_TARGET_LIST := msm8998 # Get the color format from kernel headers
MASTER_SIDE_CP_TARGET_LIST := msm8998 # ION specific settings

# Vendor Interface Manifest
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/manifest.xml:vendor/manifest.xml

# Use Sdcardfs
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.sdcardfs=1

# A/B support
PRODUCT_PACKAGES += \
    otapreopt_script \
    cppreopts.sh \
    update_engine \
    update_verifier

PRODUCT_PACKAGES += SSRestartDetector

PRODUCT_PACKAGES += \
    bootctrl.msm8998

PRODUCT_PROPERTY_OVERRIDES += \
    ro.cp_system_other_odex=1

AB_OTA_UPDATER := true

AB_OTA_PARTITIONS += \
    boot \
    system

AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_system=true \
    POSTINSTALL_PATH_system=system/bin/otapreopt_script \
    FILESYSTEM_TYPE_system=ext4 \
    POSTINSTALL_OPTIONAL_system=true

# Enable update engine sideloading by including the static version of the
# boot_control HAL and its dependencies.
PRODUCT_STATIC_BOOT_CONTROL_HAL := \
    bootctrl.msm8998 \
    libgptutils \
    libsparse

PRODUCT_PACKAGES += \
    update_engine_sideload

# The following modules are included in debuggable builds only.
PRODUCT_PACKAGES_DEBUG += \
    bootctl \
    update_engine_client

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.full.xml:system/etc/permissions/android.hardware.camera.full.xml\
    frameworks/native/data/etc/android.hardware.camera.raw.xml:system/etc/permissions/android.hardware.camera.raw.xml\
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:system/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:system/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:system/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:system/etc/permissions/android.hardware.sensor.hifi_sensors.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.opengles.aep.xml:system/etc/permissions/android.hardware.opengles.aep.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/android.hardware.nfc.hcef.xml:system/etc/permissions/android.hardware.nfc.hcef.xml \

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.power.sh:system/bin/init.power.sh \

# graphics
PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196610

# Enable low power video mode for 4K encode
PRODUCT_PROPERTY_OVERRIDES += \
    vidc.debug.perf.mode=2

# OEM Unlock reporting
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.oem_unlock_supported=1

PRODUCT_PROPERTY_OVERRIDES += \
    persist.cne.feature=1 \
    persist.data.iwlan.enable=true \
    persist.radio.RATE_ADAPT_ENABLE=1 \
    persist.radio.ROTATION_ENABLE=1 \
    persist.radio.VT_ENABLE=1 \
    persist.radio.VT_HYBRID_ENABLE=1 \
    persist.radio.apm_sim_not_pwdn=1 \
    persist.radio.data_ltd_sys_ind=1 \
    persist.radio.is_wps_enabled=true \
    persist.radio.videopause.mode=1 \
    persist.rcs.supported=1 \
    rild.libpath=/vendor/lib64/libril-qc-qmi-1.so

# Set snapshot timer to 3 second
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.snapshot_enabled=1 \
    persist.radio.snapshot_timer=3

PRODUCT_PROPERTY_OVERRIDES += \
  ro.vendor.extension_library=libqti-perfd-client.so

# camera gyro and laser sensor
PRODUCT_PROPERTY_OVERRIDES += \
  persist.camera.gyro.android=4 \
  persist.camera.tof.direct=1 \
  persist.camera.max.previewfps=60 \
  persist.camera.sensor.hdr=2

# WLAN driver configuration files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf \
    $(LOCAL_PATH)/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf \
    $(LOCAL_PATH)/wifi_concurrency_cfg.txt:system/etc/wifi/wifi_concurrency_cfg.txt \
    $(LOCAL_PATH)/WCNSS_qcom_cfg.ini:system/etc/firmware/wlan/qca_cld/WCNSS_qcom_cfg.ini

PRODUCT_PACKAGES += \
    hwcomposer.msm8998 \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service \
    gralloc.msm8998 \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.mapper@2.0-impl \
    libbt-vendor

# Light HAL
PRODUCT_PACKAGES += \
    lights.$(PRODUCT_HARDWARE) \
    android.hardware.light@2.0-impl \
    android.hardware.light@2.0-service

# gatekeeper HAL
PRODUCT_PACKAGES +=                         \
    android.hardware.gatekeeper@1.0-impl    \
    android.hardware.gatekeeper@1.0-service

# Keymaster HAL
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-impl

# Bluetooth HAL
PRODUCT_PACKAGES += \
  android.hardware.bluetooth@1.0-impl    \
  android.hardware.bluetooth@1.0-service

# NFC packages
PRODUCT_PACKAGES += \
    nfc_nci.$(PRODUCT_HARDWARE) \
    NfcNci \
    Tag \
    android.hardware.nfc@1.0-impl

PRODUCT_COPY_FILES += \
    device/google/wahoo/nfc/libnfc-brcm.conf:system/etc/libnfc-brcm.conf \
    device/google/wahoo/nfc/libnfc-nxp.conf:system/etc/libnfc-nxp.conf \

PRODUCT_PACKAGES += \
    libmm-omxcore \
    libOmxCore \
    libstagefrighthw \
    libOmxVdec \
    libOmxVdecHevc \
    libOmxVenc \
    libc2dcolorconvert

PRODUCT_PACKAGES += \
    android.hardware.camera.provider@2.4-impl \
    camera.device@3.2-impl \
    camera.msm8998 \
    libqomx_core \
    libmmjpeg_interface \
    libmmcamera_interface

PRODUCT_PACKAGES += \
    sensors.$(PRODUCT_HARDWARE) \
    android.hardware.sensors@1.0-impl

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/sensors/hals.conf:system/etc/sensors/hals.conf

PRODUCT_PACKAGES += \
    fs_config_dirs \
    fs_config_files

# Boot control HAL
PRODUCT_PACKAGES += \
    android.hardware.boot@1.0-impl \
    android.hardware.boot@1.0-service \

# Vibrator HAL
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.0-impl

# Thermal packages
PRODUCT_PACKAGES += \
    android.hardware.thermal@1.0-impl \
    android.hardware.thermal@1.0-service

#GNSS HAL
PRODUCT_PACKAGES += \
    gps.conf \
    libgps.utils \
    android.hardware.gnss@1.0-impl-qti \
    android.hardware.gnss@1.0-service-qti

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/sec_config:system/etc/sec_config


HOSTAPD := hostapd
HOSTAPD += hostapd_cli
PRODUCT_PACKAGES += $(HOSTAPD)

WPA := wpa_supplicant.conf
WPA += wpa_supplicant_wcn.conf
WPA += wpa_supplicant
PRODUCT_PACKAGES += $(WPA)

PRODUCT_PACKAGES += \
    android.hardware.wifi@1.0-service \
    wificond \
    wifilogd

LIB_NL := libnl_2
PRODUCT_PACKAGES += $(LIB_NL)

PRODUCT_PACKAGES += \
    audio.primary.msm8998 \
    audio.a2dp.default \
    audio.usb.default \
    audio.r_submix.default \
    libaudio-resampler

PRODUCT_PACKAGES += \
    android.hardware.audio@2.0-impl \
    android.hardware.audio.effect@2.0-impl \
    android.hardware.broadcastradio@1.0-impl \
    android.hardware.soundtrigger@2.0-impl

# Thermal packages
PRODUCT_PACKAGES += \
    thermal.default

ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
PRODUCT_PACKAGES += \
    tinyplay \
    tinycap \
    tinymix \
    tinypcminfo \
    cplay
endif

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio_policy_configuration.xml:system/etc/audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:system/etc/a2dp_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:system/etc/usb_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:system/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:system/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:system/etc/default_volume_tables.xml \

# audio hal tables
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/mixer_paths.xml:system/etc/mixer_paths.xml \
    $(LOCAL_PATH)/mixer_paths_tasha.xml:system/etc/mixer_paths_tasha.xml \
    $(LOCAL_PATH)/mixer_paths_tavil.xml:system/etc/mixer_paths_tavil.xml \
    $(LOCAL_PATH)/audio_platform_info.xml:system/etc/audio_platform_info.xml \
    $(LOCAL_PATH)/audio_platform_info_tavil.xml:system/etc/audio_platform_info_tavil.xml \

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/media_codecs.xml:system/etc/media_codecs.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml \
    $(LOCAL_PATH)/media_profiles.xml:system/etc/media_profiles.xml \

PRODUCT_PROPERTY_OVERRIDES += \
    audio.snd_card.open.retries=50

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/lowi.conf:system/etc/lowi.conf

# Fingerprint HIDL implementation
PRODUCT_PACKAGES += \
    fingerprint.fpc \
    android.hardware.biometrics.fingerprint@2.1-service

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.fingerprint.xml:system/etc/permissions/android.hardware.fingerprint.xml

# Vendor seccomp policy files for media components:
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/seccomp_policy/mediacodec.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy

ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
# Subsystem ramdump
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.ssr.enable_ramdumps=1
endif

# Subsystem silent restart
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.ssr.restart_level=modem

# setup dalvik vm configs
$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

# Share fstab between devices
# enables File Based Encryption (FBE)
PRODUCT_COPY_FILES += \
    device/google/wahoo/fstab.hardware:root/fstab.$(PRODUCT_HARDWARE)

# Provide meaningful APN configuration
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/apns-full-conf.xml:system/etc/apns-conf.xml

# Use the default charger mode images
PRODUCT_PACKAGES += \
    charger_res_images
