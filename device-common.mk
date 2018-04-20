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

TARGET_USERIMAGES_USE_F2FS := true

LOCAL_PATH := device/google/crosshatch

# define hardware platform
PRODUCT_PLATFORM := sdm845

# Audio XMLs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.xml

include device/google/crosshatch/device.mk

# Audio fluence, ns, aec property, voice volume steps
PRODUCT_PROPERTY_OVERRIDES += \
    ro.qc.sdk.audio.fluencetype=fluencepro \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.speaker=true \
    persist.audio.fluence.voicecomm=true \
    persist.audio.fluence.voicerec=false \
    ro.config.vc_call_vol_steps=7

# Enable AAudio MMAP/NOIRQ data path.
# 2 is AAUDIO_POLICY_AUTO so it will try MMAP then fallback to Legacy path.
PRODUCT_PROPERTY_OVERRIDES += \
    aaudio.mmap_policy=2
# Allow EXCLUSIVE then fall back to SHARED.
PRODUCT_PROPERTY_OVERRIDES += \
    aaudio.mmap_exclusive_policy=2

# Set lmkd options
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.lmk.low=1001 \
    ro.lmk.medium=800 \
    ro.lmk.critical=0 \
    ro.lmk.critical_upgrade=false \
    ro.lmk.upgrade_pressure=100 \
    ro.lmk.downgrade_pressure=100 \
    ro.lmk.kill_heaviest_task=true \
    ro.lmk.kill_timeout_ms=100 \
    ro.lmk.use_minfree_levels=true \

# A2DP offload enabled for compilation
AUDIO_FEATURE_ENABLED_A2DP_OFFLOAD := true

# A2DP offload supported
PRODUCT_PROPERTY_OVERRIDES += \
ro.bluetooth.a2dp_offload.supported=true

# A2DP offload disabled (UI toggle property)
PRODUCT_PROPERTY_OVERRIDES += \
persist.bluetooth.a2dp_offload.disabled=true

# A2DP offload DSP supported encoder list
PRODUCT_PROPERTY_OVERRIDES += \
persist.bluetooth.a2dp_offload.cap=sbc-aac-aptx-aptxhd-ldac

# Modem loging file
PRODUCT_COPY_FILES += \
    device/google/crosshatch/init.logging.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.$(PRODUCT_PLATFORM).logging.rc

# Dumpstate HAL
PRODUCT_PACKAGES += \
    android.hardware.dumpstate@1.0-service.crosshatch
