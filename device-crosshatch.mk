#
# Copyright 2016 The Android Open Source Project
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

PRODUCT_HARDWARE := crosshatch

include device/google/crosshatch/device-common.mk

DEVICE_PACKAGE_OVERLAYS += device/google/crosshatch/crosshatch/overlay

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.insmod.crosshatch.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/init.insmod.cfg

# Audio XMLs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/mixer_paths_tavil_c1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths_tavil_c1.xml \
    $(LOCAL_PATH)/audio_policy_volumes_c1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    $(LOCAL_PATH)/audio_platform_info_tavil_c1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_platform_info_tavil_c1.xml

PRODUCT_COPY_FILES += \
    device/google/crosshatch/nfc/libnfc-nxp.crosshatch.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp.conf \
    device/google/crosshatch/nfc/libnfc-nxp.crosshatch.uicc.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp-G013C.conf \
    device/google/crosshatch/nfc/libnfc-nxp.crosshatch.ese.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp-G013D.conf

PRODUCT_PACKAGES += \
    NoCutoutOverlay

# Enable iorapd prefetching by default for crosshatch targets
PRODUCT_PRODUCT_PROPERTIES += \
    iorapd.readahead.enable=true

# Disable Camera Pinning by default for crosshatch targets
PRODUCT_PRODUCT_PROPERTIES += \
    pinner.pin_camera=false
