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

LOCAL_PATH := device/google/crosshatch

ifeq ($(TARGET_PREBUILT_KERNEL),)
    LOCAL_KERNEL := device/google/crosshatch-kernel/Image.gz-dtb
else
    LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_CHARACTERISTICS := nosdcard
PRODUCT_SHIPPING_API_LEVEL := 24

DEVICE_PACKAGE_OVERLAYS := $(LOCAL_PATH)/overlay

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel \
    $(LOCAL_PATH)/fstab.hardware:root/fstab.$(PRODUCT_HARDWARE) \
    $(LOCAL_PATH)/init.recovery.hardware.rc:root/init.recovery.$(PRODUCT_HARDWARE).rc \
    $(LOCAL_PATH)/init.hardware.rc:root/init.$(PRODUCT_HARDWARE).rc \
    $(LOCAL_PATH)/init.hardware.usb.rc:root/init.$(PRODUCT_HARDWARE).usb.rc \
    $(LOCAL_PATH)/ueventd.hardware.rc:root/ueventd.$(PRODUCT_HARDWARE).rc \

MSM_VIDC_TARGET_LIST := msmcobalt # Get the color format from kernel headers
MASTER_SIDE_CP_TARGET_LIST := msmcobalt # ION specific settings

PRODUCT_PACKAGES += \
    hwcomposer.msmcobalt \
    gralloc.msmcobalt \

# setup dalvik vm configs
$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

#Android EGL implementation
PRODUCT_PACKAGES += libGLES_android

# A/B support
PRODUCT_PACKAGES += \
    otapreopt_script \
    cppreopts.sh \
    update_engine \
    update_verifier

PRODUCT_PROPERTY_OVERRIDES += \
    ro.cp_system_other_odex=1

# Use the A/B updater.
AB_OTA_UPDATER := true

AB_OTA_PARTITIONS += \
    boot \
    system

AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_system=true \
    POSTINSTALL_PATH_system=system/bin/otapreopt_script \
    FILESYSTEM_TYPE_system=ext4 \
    POSTINSTALL_OPTIONAL_system=true

