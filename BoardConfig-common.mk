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

include build/make/target/board/BoardConfigMainlineCommon.mk

TARGET_BOARD_PLATFORM := sdm845
TARGET_BOARD_INFO_FILE := device/google/crosshatch/board-info.txt
USES_DEVICE_GOOGLE_B1C1 := true

TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := generic
TARGET_CPU_VARIANT_RUNTIME := kryo385

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-a
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := generic
TARGET_2ND_CPU_VARIANT_RUNTIME := kryo385

TARGET_BOARD_COMMON_PATH := device/google/crosshatch/sdm845

BUILD_BROKEN_DUP_RULES := true

BOARD_KERNEL_CMDLINE += console=ttyMSM0,115200n8 androidboot.console=ttyMSM0 printk.devkmsg=on
BOARD_KERNEL_CMDLINE += msm_rtb.filter=0x237
BOARD_KERNEL_CMDLINE += ehci-hcd.park=3
BOARD_KERNEL_CMDLINE += service_locator.enable=1
BOARD_KERNEL_CMDLINE += cgroup.memory=nokmem
BOARD_KERNEL_CMDLINE += lpm_levels.sleep_disabled=1
BOARD_KERNEL_CMDLINE += usbcore.autosuspend=7
BOARD_KERNEL_CMDLINE += loop.max_part=7
BOARD_KERNEL_CMDLINE += androidboot.boot_devices=soc/1d84000.ufshc

BOARD_KERNEL_BASE        := 0x00000000
BOARD_KERNEL_PAGESIZE    := 4096

BOARD_INCLUDE_DTB_IN_BOOTIMG := true
BOARD_BOOT_HEADER_VERSION := 2
BOARD_MKBOOTIMG_ARGS += --header_version $(BOARD_BOOT_HEADER_VERSION)

# DTBO partition definitions
BOARD_PREBUILT_DTBOIMAGE := device/google/crosshatch-kernel/dtbo.img
BOARD_DTBOIMG_PARTITION_SIZE := 8388608

TARGET_NO_KERNEL := false
BOARD_USES_RECOVERY_AS_BOOT := true
BOARD_USES_METADATA_PARTITION := true

# Board uses A/B OTA.
AB_OTA_UPDATER := true

AB_OTA_PARTITIONS += \
    boot \
    system \
    vbmeta \
    dtbo

# Skip product and system_ext partition for nodap build
ifeq ($(filter %_nodap,$(TARGET_PRODUCT)),)
AB_OTA_PARTITIONS += \
    product \
    system_ext
endif

ifneq ($(filter %_mainline,$(TARGET_PRODUCT)),)
AB_OTA_PARTITIONS += \
    vbmeta_system
endif

# Partitions (listed in the file) to be wiped under recovery.
TARGET_RECOVERY_WIPE := device/google/crosshatch/recovery.wipe
ifneq ($(filter %_mainline,$(TARGET_PRODUCT)),)
TARGET_RECOVERY_FSTAB := device/google/crosshatch/fstab.mainline.hardware
else
TARGET_RECOVERY_FSTAB := device/google/crosshatch/fstab.hardware
endif
TARGET_RECOVERY_PIXEL_FORMAT := RGBX_8888
TARGET_RECOVERY_UI_LIB := \
    librecovery_ui_pixel \
    libfstab

ifneq ($(filter %_mainline,$(TARGET_PRODUCT)),)
BOARD_AVB_VBMETA_SYSTEM := system system_ext
BOARD_AVB_VBMETA_SYSTEM_KEY_PATH := external/avb/test/data/testkey_rsa2048.pem
BOARD_AVB_VBMETA_SYSTEM_ALGORITHM := SHA256_RSA2048
BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX := $(PLATFORM_SECURITY_PATCH_TIMESTAMP)
BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX_LOCATION := 1
endif

# product.img
ifneq ($(PRODUCT_NO_PRODUCT_PARTITION), true)
ifneq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
  BOARD_PRODUCTIMAGE_PARTITION_SIZE := 314572800
endif
BOARD_PRODUCTIMAGE_FILE_SYSTEM_TYPE := ext4
else
TARGET_COPY_OUT_PRODUCT := system/product
endif

# system.img
ifneq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
  BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2952790016
ifeq ($(PRODUCT_NO_PRODUCT_PARTITION), true)
  # Increase inode count to add product modules
  BOARD_SYSTEMIMAGE_EXTFS_INODE_COUNT := 8192
else
  BOARD_SYSTEMIMAGE_EXTFS_INODE_COUNT := 4096
endif
endif
BOARD_SYSTEMIMAGE_JOURNAL_SIZE := 0

# userdata.img
BOARD_USERDATAIMAGE_PARTITION_SIZE := 10737418240
BOARD_USERDATAIMAGE_FILE_SYSTEM_TYPE := f2fs

# persist.img
BOARD_PERSISTIMAGE_PARTITION_SIZE := 41943040
BOARD_PERSISTIMAGE_FILE_SYSTEM_TYPE := ext4

# boot.img
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x04000000

# system_ext.img
ifneq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
TARGET_COPY_OUT_SYSTEM_EXT := system/system_ext
else
BOARD_SYSTEM_EXTIMAGE_FILE_SYSTEM_TYPE := ext4
endif
ifeq ($(PRODUCT_NO_PRODUCT_PARTITION), true)
# no system_ext partition as well
TARGET_COPY_OUT_SYSTEM_EXT := system/system_ext
endif

ifeq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
BOARD_SUPER_PARTITION_GROUPS := google_dynamic_partitions
BOARD_GOOGLE_DYNAMIC_PARTITIONS_PARTITION_LIST := \
    system \
    vendor \
    product \
    system_ext

ifeq ($(PRODUCT_RETROFIT_DYNAMIC_PARTITIONS), true)
# Normal Pixel 3 must retrofit dynamic partitions.
BOARD_SUPER_PARTITION_SIZE := 4072669184
BOARD_SUPER_PARTITION_METADATA_DEVICE := system
BOARD_SUPER_PARTITION_BLOCK_DEVICES := system vendor product
BOARD_SUPER_PARTITION_SYSTEM_DEVICE_SIZE := 2952790016
BOARD_SUPER_PARTITION_VENDOR_DEVICE_SIZE := 805306368
BOARD_SUPER_PARTITION_PRODUCT_DEVICE_SIZE := 314572800
# Assume 4MB metadata size.
# TODO(b/117997386): Use correct metadata size.
BOARD_GOOGLE_DYNAMIC_PARTITIONS_SIZE := 4069523456
else
# Mainline Pixel 3 has an actual super partition.

BOARD_SUPER_PARTITION_SIZE := 12884901888
# Assume 1MB metadata size.
# TODO(b/117997386): Use correct metadata size.
BOARD_GOOGLE_DYNAMIC_PARTITIONS_SIZE := 6441402368

endif # PRODUCT_RETROFIT_DYNAMIC_PARTITIONS
endif # PRODUCT_USE_DYNAMIC_PARTITIONS

BOARD_FLASH_BLOCK_SIZE := 131072

BOARD_ROOT_EXTRA_SYMLINKS := /vendor/dsp:/dsp
BOARD_ROOT_EXTRA_SYMLINKS += /mnt/vendor/persist:/persist

# Add QC specific symlinks for backward compatibility
# Move the symlinks here instead of removing them
ifeq ($(PRODUCT_USE_QC_SPECIFIC_SYMLINKS), true)
BOARD_ROOT_EXTRA_SYMLINKS += /vendor/firmware_mnt:/firmware
endif

include device/google/crosshatch-sepolicy/crosshatch-sepolicy.mk

TARGET_FS_CONFIG_GEN := device/google/crosshatch/config.fs

QCOM_BOARD_PLATFORMS += sdm845
BOARD_HAVE_BLUETOOTH_QCOM := true
BOARD_HAVE_QCOM_FM := false
BOARD_USES_COMMON_BLUETOOTH_HAL := true

# Camera
TARGET_USES_AOSP := true
BOARD_QTI_CAMERA_32BIT_ONLY := false
CAMERA_DAEMON_NOT_PRESENT := true
TARGET_USES_ION := true
TARGET_USES_EASEL := true
BOARD_USES_EASEL := true

# GPS
TARGET_NO_RPC := true
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := default
BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET := true

# RenderScript
OVERRIDE_RS_DRIVER := libRSDriver_adreno.so

# Sensors
USE_SENSOR_MULTI_HAL := true
TARGET_SUPPORT_DIRECT_REPORT := true
# Enable sensor Version V_2
USE_SENSOR_HAL_VER := 2.0

# CHRE
CHRE_DAEMON_ENABLED := true
CHRE_DAEMON_LPMA_ENABLED := true
CHRE_DAEMON_USE_SDSPRPC := true

# wlan
BOARD_WLAN_DEVICE := qcwcn
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211
WIFI_DRIVER_DEFAULT := qca_cld3
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
WIFI_HIDL_FEATURE_AWARE := true
WIFI_HIDL_FEATURE_DUAL_INTERFACE:= true
WIFI_FEATURE_WIFI_EXT_HAL := true
WIFI_FEATURE_IMU_DETECTION := false
WIFI_HIDL_UNIFIED_SUPPLICANT_SERVICE_RC_ENTRY := true

# Audio
BOARD_USES_ALSA_AUDIO := true
AUDIO_FEATURE_ENABLED_MULTI_VOICE_SESSIONS := true
AUDIO_FEATURE_ENABLED_SND_MONITOR := true
AUDIO_FEATURE_ENABLED_USB_TUNNEL := true
AUDIO_FEATURE_ENABLED_CIRRUS_SPKR_PROTECTION := true
BOARD_SUPPORTS_SOUND_TRIGGER := true
AUDIO_FEATURE_FLICKER_SENSOR_INPUT := true
SOUND_TRIGGER_FEATURE_LPMA_ENABLED := true
AUDIO_FEATURE_ENABLED_MAXX_AUDIO := true
AUDIO_FEATURE_ENABLED_24BITS_CAMCORDER := true

# Graphics
TARGET_USES_GRALLOC1 := true
TARGET_USES_HWC2 := true

VSYNC_EVENT_PHASE_OFFSET_NS := 2000000
SF_VSYNC_EVENT_PHASE_OFFSET_NS := 6000000

# Display
TARGET_HAS_WIDE_COLOR_DISPLAY := true
TARGET_HAS_HDR_DISPLAY := true
TARGET_USES_DISPLAY_RENDER_INTENTS := true
TARGET_USES_COLOR_METADATA := true
TARGET_USES_DRM_PP := true

# Vendor Interface Manifest
DEVICE_MANIFEST_FILE := device/google/crosshatch/manifest.xml
DEVICE_MATRIX_FILE := device/google/crosshatch/compatibility_matrix.xml
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE := device/google/crosshatch/device_framework_matrix.xml

# Userdebug only Vendor Interface Manifest
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
DEVICE_FRAMEWORK_MANIFEST_FILE += device/google/crosshatch/framework_manifest_userdebug.xml
DEVICE_MATRIX_FILE += device/google/crosshatch/compatibility_matrix_userdebug.xml
endif

ODM_MANIFEST_SKUS += \
    G013A \
    G013B \
    G013C \
    G013D \

ODM_MANIFEST_G013A_FILES := device/google/crosshatch/nfc/manifest_se_SIM1.xml
ODM_MANIFEST_G013B_FILES := device/google/crosshatch/nfc/manifest_se_eSE1.xml
ODM_MANIFEST_G013C_FILES := device/google/crosshatch/nfc/manifest_se_SIM1.xml
ODM_MANIFEST_G013D_FILES := device/google/crosshatch/nfc/manifest_se_eSE1.xml

# Use mke2fs to create ext4 images
TARGET_USES_MKE2FS := true

# Kernel modules
ifeq (,$(filter-out blueline_kasan crosshatch_kasan, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/kasan/*.ko)
else ifeq (,$(filter-out blueline_kernel_debug_memory crosshatch_kernel_debug_memory, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/debug_memory/*.ko)
else ifeq (,$(filter-out blueline_kernel_debug_locking crosshatch_kernel_debug_locking, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/debug_locking/*.ko)
else ifeq (,$(filter-out blueline_kernel_debug_hang crosshatch_kernel_debug_hang, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/debug_hang/*.ko)
else ifeq (,$(filter-out blueline_kernel_debug_api crosshatch_kernel_debug_api, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/debug_api/*.ko)
else ifneq (,$(TARGET_PREBUILT_KERNEL))
    # If TARGET_PREBUILT_KERNEL is set, check whether there are modules packaged with that kernel
    # image. If so, use them, otherwise fall back to the default directory.
    TARGET_PREBUILT_KERNEL_PREBUILT_VENDOR_KERNEL_MODULES := \
        $(wildcard $(dir $(TARGET_PREBUILT_KERNEL))/*.ko)
    ifneq (,$(TARGET_PREBUILT_KERNEL_PREBUILT_VENDOR_KERNEL_MODULES))
        BOARD_VENDOR_KERNEL_MODULES += $(TARGET_PREBUILT_KERNEL_PREBUILT_VENDOR_KERNEL_MODULES)
    else
        BOARD_VENDOR_KERNEL_MODULES += $(wildcard device/google/crosshatch-kernel/*.ko)
    endif
    # Do NOT delete TARGET_PREBUILT..., it will lead to empty BOARD_VENDOR_KERNEL_MODULES.
else
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/*.ko)
endif

# DTB
ifeq (,$(filter-out blueline_kasan crosshatch_kasan, $(TARGET_PRODUCT)))
BOARD_PREBUILT_DTBIMAGE_DIR := device/google/crosshatch-kernel/kasan
else ifeq (,$(filter-out blueline_kernel_debug_memory crosshatch_kernel_debug_memory, $(TARGET_PRODUCT)))
BOARD_PREBUILT_DTBIMAGE_DIR := device/google/crosshatch-kernel/debug_memory
else ifeq (,$(filter-out blueline_kernel_debug_locking crosshatch_kernel_debug_locking, $(TARGET_PRODUCT)))
BOARD_PREBUILT_DTBIMAGE_DIR := device/google/crosshatch-kernel/debug_locking
else ifeq (,$(filter-out blueline_kernel_debug_hang crosshatch_kernel_debug_hang, $(TARGET_PRODUCT)))
BOARD_PREBUILT_DTBIMAGE_DIR := device/google/crosshatch-kernel/debug_hang
else ifeq (,$(filter-out blueline_kernel_debug_api crosshatch_kernel_debug_api, $(TARGET_PRODUCT)))
BOARD_PREBUILT_DTBIMAGE_DIR := device/google/crosshatch-kernel/debug_api
else ifneq (,$(TARGET_PREBUILT_KERNEL))
    # If TARGET_PREBUILT_KERNEL is set, check whether there are dtb files packaged with that kernel
    # image. If so, use them, otherwise fall back to the default directory.
    PREBUILT_PREBUILT_DTBIMAGE_DIR := $(wildcard $(dir $(TARGET_PREBUILT_KERNEL))/*.dtb)
    ifneq (,$(PREBUILT_PREBUILT_DTBIMAGE_DIR)
        BOARD_PREBUILT_DTBIMAGE_DIR := $(dir $(TARGET_PREBUILT_KERNEL))
    else
        BOARD_PREBUILT_DTBIMAGE_DIR += device/google/crosshatch-kernel
    endif
    PREBUILT_VENDOR_KERNEL_MODULES :=
else
BOARD_PREBUILT_DTBIMAGE_DIR := device/google/crosshatch-kernel
endif

# Testing related defines
BOARD_PERFSETUP_SCRIPT := platform_testing/scripts/perf-setup/b1c1-setup.sh
-include vendor/google_devices/crosshatch/proprietary/BoardConfigVendor.mk
