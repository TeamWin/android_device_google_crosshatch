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

PRODUCT_HARDWARE := blueline

include device/google/crosshatch/device-common.mk

DEVICE_PACKAGE_OVERLAYS += device/google/crosshatch/blueline/overlay

PRODUCT_PACKAGES += \
    TelephonyOverlay2018Blueline

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.insmod.blueline.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/init.insmod.cfg

# Audio XMLs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/mixer_paths_tavil_b1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths_tavil_b1.xml \
    $(LOCAL_PATH)/audio_policy_volumes_b1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    $(LOCAL_PATH)/audio_platform_info_tavil_b1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_platform_info_tavil_b1.xml

PRODUCT_COPY_FILES += \
    device/google/crosshatch/nfc/libnfc-nxp.blueline.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp.conf \
    device/google/crosshatch/nfc/libnfc-nxp.blueline.uicc.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp-G013A.conf \
    device/google/crosshatch/nfc/libnfc-nxp.blueline.ese.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp-G013B.conf

PRODUCT_PRODUCT_PROPERTIES += ro.com.google.ime.height_ratio=1.2

# Enable iorapd perfetto tracing for app starts
PRODUCT_PRODUCT_PROPERTIES += \
    iorapd.perfetto.enable=true
# Enable iorapd readahead for app starts
PRODUCT_PRODUCT_PROPERTIES += \
    iorapd.readahead.enable=true
