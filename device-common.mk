#
# Copyright (C) 2017 The Android Open-Source Project
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

# Audio XMLs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/mixer_paths_tasha.xml:system/etc/mixer_paths_tasha.xml \
    $(LOCAL_PATH)/mixer_paths_tavil.xml:system/etc/mixer_paths_tavil.xml \
    $(LOCAL_PATH)/audio_platform_info.xml:system/etc/audio_platform_info.xml \
    $(LOCAL_PATH)/audio_platform_info_tavil.xml:system/etc/audio_platform_info_tavil.xml

include device/google/wahoo/device.mk

# Kernel modules
PRODUCT_COPY_FILES += \
    device/google/wahoo-kernel/synaptics_dsx_core_htc.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/synaptics_dsx_core_htc.ko \
    device/google/wahoo-kernel/synaptics_dsx_rmi_dev_htc.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/synaptics_dsx_rmi_dev_htc.ko \
    device/google/wahoo-kernel/synaptics_dsx_fw_update_htc.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/synaptics_dsx_fw_update_htc.ko

# Audio fluence, ns, aec property, voice volume steps
PRODUCT_PROPERTY_OVERRIDES += \
    ro.qc.sdk.audio.fluencetype=fluencepro \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.speaker=true \
    persist.audio.fluence.voicecomm=true \
    persist.audio.fluence.voicerec=false \
    ro.config.vc_call_vol_steps=7

# Enable SM log mechanism by default
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.smlog_switch=1
endif

# SM loging file
PRODUCT_COPY_FILES += \
    device/google/crosshatch/init.logging.rc:root/init.$(PRODUCT_HARDWARE).logging.rc

# Dumpstate HAL
PRODUCT_PACKAGES += \
    android.hardware.dumpstate@1.0-service.crosshatch
