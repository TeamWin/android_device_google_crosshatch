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

# Copy TWRP ramdisk files automatically
TARGET_RECOVERY_DEVICE_DIRS := $(DEVICE_PATH)

# Kernel
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive
BOARD_KERNEL_IMAGE_NAME := Image.lz4
TARGET_PREBUILT_KERNEL := $(LOCAL_KERNEL)

# Qcom Decryption
BUILD_BROKEN_ELF_PREBUILT_PRODUCT_COPY_FILES := true
BOARD_USES_QCOM_FBE_DECRYPTION := true
DISABLE_ARTIFACT_PATH_REQUIREMENTS := true

# Recovery
TARGET_RECOVERY_DEVICE_MODULES += \
    android.hardware.vibrator@1.0 \
    android.hardware.vibrator@1.1 \
    android.hardware.vibrator@1.2 \
    android.hardware.vibrator@1.2-impl.crosshatch \
    libion \
    vibrator.crosshatch-service
RECOVERY_BINARY_SOURCE_FILES += \
    $(TARGET_OUT_VENDOR_EXECUTABLES)/hw/vibrator.crosshatch-service
RECOVERY_LIBRARY_SOURCE_FILES += \
    $(TARGET_OUT_SHARED_LIBRARIES)/libion.so \
    $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/android.hardware.vibrator@1.0.so \
    $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/android.hardware.vibrator@1.1.so \
    $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/android.hardware.vibrator@1.2.so \
    $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/hw/android.hardware.vibrator@1.2-impl.crosshatch.so

# TWRP Standard Flags
TW_THEME := portrait_hdpi
BOARD_SUPPRESS_SECURE_ERASE := true
TARGET_RECOVERY_QCOM_RTC_FIX := true
TW_INPUT_BLACKLIST := "hbtp_vm"
TW_DEFAULT_BRIGHTNESS := "80"
TW_INCLUDE_CRYPTO := true
TW_EXCLUDE_DEFAULT_USB_INIT := true
TW_INCLUDE_NTFS_3G := true
TW_INCLUDE_REPACKTOOLS := true
TW_INCLUDE_RESETPROP := true
PLATFORM_VERSION := 99.87.36
PLATFORM_VERSION_LAST_STABLE := $(PLATFORM_VERSION)
PLATFORM_SECURITY_PATCH := 2099-12-31
LZMA_RAMDISK_TARGETS := recovery,boot
TW_OVERRIDE_SYSTEM_PROPS := \
    "ro.build.date.utc;ro.bootimage.build.date.utc=ro.build.date.utc;ro.odm.build.date.utc=ro.build.date.utc;ro.product.build.date.utc=ro.build.date.utc;ro.system.build.date.utc=ro.build.date.utc;ro.system_ext.build.date.utc=ro.build.date.utc;ro.vendor.build.date.utc=ro.build.date.utc;ro.build.product;ro.build.fingerprint=ro.system.build.fingerprint;ro.build.version.incremental;ro.product.name=ro.product.system.name"
TW_LOAD_VENDOR_MODULES := "videobuf2-memops.ko videobuf2-vmalloc.ko heatmap.ko ftm5.ko sec_touch.ko"
TW_USE_FSCRYPT_POLICY := 1

# TWRP Debug Flags
#TWRP_EVENT_LOGGING := true
TARGET_USES_LOGD := true
TWRP_INCLUDE_LOGCAT := true
TARGET_RECOVERY_DEVICE_MODULES += debuggerd
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/debuggerd
TARGET_RECOVERY_DEVICE_MODULES += strace
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/strace
#TARGET_RECOVERY_DEVICE_MODULES += twrpdec
#TW_RECOVERY_ADDITIONAL_RELINK_FILES += $(TARGET_RECOVERY_ROOT_OUT)/sbin/twrpdec

#
# For local builds only
#
# TWRP zip installer
# See https://gerrit.twrp.me/c/android_build/+/5445 for details
ifneq ($(wildcard bootable/recovery/installer/.),)
    USE_RECOVERY_INSTALLER := true
    RECOVERY_INSTALLER_PATH := bootable/recovery/installer
endif

# Custom TWRP Versioning
# See https://github.com/minimal-manifest-twrp/android_device_common_version-info for details
ifneq ($(wildcard device/common/version-info/.),)
    # device version is optional - the default value is "0" if nothing is set in device tree
    CUSTOM_TWRP_DEVICE_VERSION := 0
    # version prefix is optional - the default value is "LOCAL" if nothing is set in device tree
    CUSTOM_TWRP_VERSION_PREFIX := CPTB

    include device/common/version-info/custom_twrp_version.mk

    ifeq ($(CUSTOM_TWRP_VERSION),)
        CUSTOM_TWRP_VERSION := $(shell date +%Y%m%d)-01
    endif
endif
#
# end local build flags
#
