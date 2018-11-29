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

TARGET_BOARD_PLATFORM := sdm845
TARGET_BOARD_INFO_FILE := device/google/crosshatch/board-info.txt
USES_DEVICE_GOOGLE_B1C1 := true
TARGET_NO_BOOTLOADER := true

TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-2a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := cortex-a75

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-a
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := cortex-a75

TARGET_BOARD_COMMON_PATH := device/google/crosshatch/sdm845

BOARD_KERNEL_CMDLINE += console=ttyMSM0,115200n8 androidboot.console=ttyMSM0 printk.devkmsg=on
BOARD_KERNEL_CMDLINE += msm_rtb.filter=0x237
BOARD_KERNEL_CMDLINE += ehci-hcd.park=3
BOARD_KERNEL_CMDLINE += service_locator.enable=1
BOARD_KERNEL_CMDLINE += firmware_class.path=/vendor/firmware
BOARD_KERNEL_CMDLINE += cgroup.memory=nokmem
BOARD_KERNEL_CMDLINE += lpm_levels.sleep_disabled=1
BOARD_KERNEL_CMDLINE += usbcore.autosuspend=7

BOARD_KERNEL_BASE        := 0x00000000
BOARD_KERNEL_PAGESIZE    := 4096
ifeq ($(filter-out crosshatch_kasan blueline_kasan, $(TARGET_PRODUCT)),)
BOARD_KERNEL_OFFSET      := 0x80000
BOARD_KERNEL_TAGS_OFFSET := 0x02500000
BOARD_RAMDISK_OFFSET     := 0x02700000
BOARD_MKBOOTIMG_ARGS     := --kernel_offset $(BOARD_KERNEL_OFFSET) --ramdisk_offset $(BOARD_RAMDISK_OFFSET) --tags_offset $(BOARD_KERNEL_TAGS_OFFSET)
else
BOARD_KERNEL_TAGS_OFFSET := 0x01E00000
BOARD_RAMDISK_OFFSET     := 0x02000000
endif

BOARD_BOOT_HEADER_VERSION := 1
BOARD_MKBOOTIMG_ARGS += --header_version $(BOARD_BOOT_HEADER_VERSION)

# DTBO partition definitions
BOARD_PREBUILT_DTBOIMAGE := device/google/crosshatch-kernel/dtbo.img
BOARD_DTBOIMG_PARTITION_SIZE := 8388608

TARGET_NO_BOOTLOADER ?= true
TARGET_NO_KERNEL := false
TARGET_NO_RECOVERY := true
BOARD_USES_RECOVERY_AS_BOOT := true
BOARD_BUILD_SYSTEM_ROOT_IMAGE := true
BOARD_USES_METADATA_PARTITION := true

# Partitions (listed in the file) to be wiped under recovery.
TARGET_RECOVERY_WIPE := device/google/crosshatch/recovery.wipe
TARGET_RECOVERY_FSTAB := device/google/crosshatch/fstab.hardware
TARGET_RECOVERY_PIXEL_FORMAT := RGBX_8888
TARGET_RECOVERY_UI_LIB := \
  librecovery_ui_crosshatch \
  libnos_citadel_for_recovery \
  libnos_for_recovery

BOARD_AVB_ENABLE := true
BOARD_AVB_ROLLBACK_INDEX := $(PLATFORM_SECURITY_PATCH_TIMESTAMP)

# Enable chain partition for system.
BOARD_AVB_SYSTEM_KEY_PATH := external/avb/test/data/testkey_rsa2048.pem
BOARD_AVB_SYSTEM_ALGORITHM := SHA256_RSA2048
BOARD_AVB_SYSTEM_ROLLBACK_INDEX := $(PLATFORM_SECURITY_PATCH_TIMESTAMP)
BOARD_AVB_SYSTEM_ROLLBACK_INDEX_LOCATION := 1

BOARD_PROPERTY_OVERRIDES_SPLIT_ENABLED := true

# product.img
BOARD_PRODUCTIMAGE_PARTITION_SIZE := 314572800
BOARD_PRODUCTIMAGE_FILE_SYSTEM_TYPE := ext4
TARGET_COPY_OUT_PRODUCT := product

# system.img
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2952790016
BOARD_SYSTEMIMAGE_JOURNAL_SIZE := 0
BOARD_SYSTEMIMAGE_EXTFS_INODE_COUNT := 4096

# userdata.img
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_USERDATAIMAGE_PARTITION_SIZE := 10737418240
BOARD_USERDATAIMAGE_FILE_SYSTEM_TYPE := f2fs

# persist.img
BOARD_PERSISTIMAGE_PARTITION_SIZE := 41943040
BOARD_PERSISTIMAGE_FILE_SYSTEM_TYPE := ext4

# boot.img
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x04000000

TARGET_COPY_OUT_VENDOR := vendor

BOARD_FLASH_BLOCK_SIZE := 131072

# Install odex files into the other system image
BOARD_USES_SYSTEM_OTHER_ODEX := true

BOARD_ROOT_EXTRA_SYMLINKS := /vendor/lib/dsp:/dsp
BOARD_ROOT_EXTRA_SYMLINKS += /mnt/vendor/persist:/persist
BOARD_ROOT_EXTRA_SYMLINKS += /vendor/firmware_mnt:/firmware

include device/google/crosshatch-sepolicy/crosshatch-sepolicy.mk

TARGET_FS_CONFIG_GEN := device/google/crosshatch/config.fs

QCOM_BOARD_PLATFORMS += sdm845
BOARD_HAVE_BLUETOOTH_QCOM := true
BOARD_HAVE_QCOM_FM := false
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/google/crosshatch/bluetooth

# Enable dex pre-opt to speed up initial boot
ifeq ($(HOST_OS),linux)
  ifeq ($(WITH_DEXPREOPT),)
    WITH_DEXPREOPT := true
    WITH_DEXPREOPT_PIC := true
    ifneq ($(TARGET_BUILD_VARIANT),user)
      # Retain classes.dex in APK's for non-user builds
      DEX_PREOPT_DEFAULT := nostripping
    endif
  endif
endif

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

# Audio
BOARD_USES_ALSA_AUDIO := true
USE_XML_AUDIO_POLICY_CONF := 1
AUDIO_FEATURE_ENABLED_MULTI_VOICE_SESSIONS := true
AUDIO_FEATURE_ENABLED_SND_MONITOR := true
AUDIO_FEATURE_ENABLED_USB_TUNNEL := true
AUDIO_FEATURE_ENABLED_CIRRUS_SPKR_PROTECTION := true
BOARD_SUPPORTS_SOUND_TRIGGER := true
AUDIO_FEATURE_FLICKER_SENSOR_INPUT := true
SOUND_TRIGGER_FEATURE_LPMA_ENABLED := true
AUDIO_FEATURE_ENABLED_MAXX_AUDIO := true

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

# Charger Mode
BOARD_CHARGER_ENABLE_SUSPEND := true

# Vendor Interface Manifest
DEVICE_MANIFEST_FILE := device/google/crosshatch/manifest.xml
DEVICE_MATRIX_FILE := device/google/crosshatch/compatibility_matrix.xml
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE := device/google/crosshatch/device_framework_matrix.xml
DEVICE_FRAMEWORK_MANIFEST_FILE := device/google/crosshatch/framework_manifest.xml

# Userdebug only Vendor Interface Manifest
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
DEVICE_MANIFEST_FILE += device/google/crosshatch/manifest_userdebug.xml
endif

# Remove health /backup instance
DEVICE_FRAMEWORK_MANIFEST_FILE += system/libhidl/vintfdata/manifest_healthd_exclude.xml

BOARD_PROPERTY_OVERRIDES_SPLIT_ENABLED := true

# Use mke2fs to create ext4 images
TARGET_USES_MKE2FS := true

# Kernel modules
ifeq (,$(filter-out blueline_gcc crosshatch_gcc, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/gcc/*.ko)
else ifeq (,$(filter-out blueline_kasan crosshatch_kasan, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/kasan/*.ko)
else ifeq (,$(filter-out blueline_kcfi crosshatch_kcfi, $(TARGET_PRODUCT)))
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/kcfi/*.ko)
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
else
BOARD_VENDOR_KERNEL_MODULES += \
    $(wildcard device/google/crosshatch-kernel/*.ko)
endif

# Testing related defines
BOARD_PERFSETUP_SCRIPT := platform_testing/scripts/perf-setup/b1c1-setup.sh

-include vendor/google_devices/crosshatch/proprietary/BoardConfigVendor.mk
