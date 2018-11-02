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

include device/google/crosshatch/device.mk

# Audio fluence, ns, aec property, voice volume steps
PRODUCT_PROPERTY_OVERRIDES += \
    ro.qc.sdk.audio.fluencetype=fluencepro \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.speaker=true \
    persist.audio.fluence.voicecomm=true \
    persist.audio.fluence.voicerec=false \
    ro.config.vc_call_vol_steps=7

# Bug 77867216
PRODUCT_PROPERTY_OVERRIDES += audio.adm.buffering.ms=3
PRODUCT_PROPERTY_OVERRIDES += vendor.audio.adm.buffering.ms=3
PRODUCT_PROPERTY_OVERRIDES += audio_hal.period_multiplier=2
PRODUCT_PROPERTY_OVERRIDES += af.fast_track_multiplier=1

# Pixelstats broken mic detection
PRODUCT_PROPERTY_OVERRIDES += vendor.audio.mic_break=true

# Setting vendor SPL
VENDOR_SECURITY_PATCH = "2018-09-05"

# MIDI feature
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml

# Audio low latency feature
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml

# Pro audio feature
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.audio.pro.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.pro.xml

# Enable AAudio MMAP/NOIRQ data path.
# 1 is AAUDIO_POLICY_NEVER  means only use Legacy path.
# 2 is AAUDIO_POLICY_AUTO   means try MMAP then fallback to Legacy path.
# 3 is AAUDIO_POLICY_ALWAYS means only use MMAP path.
PRODUCT_PROPERTY_OVERRIDES += aaudio.mmap_policy=2
# 1 is AAUDIO_POLICY_NEVER  means only use SHARED mode
# 2 is AAUDIO_POLICY_AUTO   means try EXCLUSIVE then fallback to SHARED mode.
# 3 is AAUDIO_POLICY_ALWAYS means only use EXCLUSIVE mode.
PRODUCT_PROPERTY_OVERRIDES += aaudio.mmap_exclusive_policy=2

# Increase the apparent size of a hardware burst from 1 msec to 2 msec.
# A "burst" is the number of frames processed at one time.
# That is an increase from 48 to 96 frames at 48000 Hz.
# The DSP will still be bursting at 48 frames but AAudio will think the burst is 96 frames.
# A low number, like 48, might increase power consumption or stress the system.
PRODUCT_PROPERTY_OVERRIDES += aaudio.hw_burst_min_usec=2000

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
persist.bluetooth.a2dp_offload.disabled=false

# A2DP offload DSP supported encoder list
PRODUCT_PROPERTY_OVERRIDES += \
persist.bluetooth.a2dp_offload.cap=sbc-aac-aptx-aptxhd-ldac

# Modem loging file
PRODUCT_COPY_FILES += \
    device/google/crosshatch/init.logging.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.$(PRODUCT_PLATFORM).logging.rc

# Dumpstate HAL
PRODUCT_PACKAGES += \
    android.hardware.dumpstate@1.0-service.crosshatch

# whitelisted app
PRODUCT_COPY_FILES += \
    device/google/crosshatch/qti_whitelist.xml:system/etc/sysconfig/qti_whitelist.xml

PRODUCT_COPY_FILES += \
    device/google/crosshatch/recovery/root/etc/twrp.fstab:recovery/root/etc/twrp.fstab \
    device/google/crosshatch/recovery/root/sbin/prepdecrypt.sh:recovery/root/sbin/prepdecrypt.sh \
    device/google/crosshatch/recovery/root/system/etc/vintf/manifest.xml:recovery/root/manifest.xml \
    device/google/crosshatch/recovery/root/plat_hwservice_contexts:recovery/root/plat_hwservice_contexts \
    device/google/crosshatch/recovery/root/plat_service_contexts:recovery/root/plat_service_contexts \
    device/google/crosshatch/recovery/root/vendor_hwservice_contexts:recovery/root/vendor_hwservice_contexts \
    device/google/crosshatch/recovery/root/vendor/compatibility_matrix.xml:recovery/root/vendor/compatibility_matrix.xml \
    device/google/crosshatch/recovery/root/vendor/compatibility_matrix.1.xml:recovery/root/vendor/compatibility_matrix.1.xml \
    device/google/crosshatch/recovery/root/vendor/compatibility_matrix.2.xml:recovery/root/vendor/compatibility_matrix.2.xml \
    device/google/crosshatch/recovery/root/vendor/compatibility_matrix.3.xml:recovery/root/vendor/compatibility_matrix.3.xml \
    device/google/crosshatch/recovery/root/vendor/compatibility_matrix.device.xml:recovery/root/vendor/compatibility_matrix.device.xml \
    device/google/crosshatch/recovery/root/vendor/compatibility_matrix.legacy.xml:recovery/root/vendor/compatibility_matrix.legacy.xml \
    device/google/crosshatch/recovery/root/vendor/etc/vintf/manifest.xml:recovery/root/vendor/etc/vintf/manifest.xml \
    device/google/crosshatch/recovery/root/vendor/etc/vintf/compatibility_matrix.xml:recovery/root/vendor/etc/vintf/compatibility_matrix.xml \
    device/google/crosshatch/recovery/root/sbin/qseecomd:recovery/root/sbin/qseecomd \
    device/google/crosshatch/recovery/root/sbin/libdiag.so:recovery/root/sbin/libdiag.so \
    device/google/crosshatch/recovery/root/sbin/libdrmfs.so:recovery/root/sbin/libdrmfs.so \
    device/google/crosshatch/recovery/root/sbin/libdrmtime.so:recovery/root/sbin/libdrmtime.so \
    device/google/crosshatch/recovery/root/sbin/libQSEEComAPI.so:recovery/root/sbin/libQSEEComAPI.so \
    device/google/crosshatch/recovery/root/sbin/librpmb.so:recovery/root/sbin/librpmb.so \
    device/google/crosshatch/recovery/root/sbin/libssd.so:recovery/root/sbin/libssd.so \
    device/google/crosshatch/recovery/root/sbin/libtime_genoff.so:recovery/root/sbin/libtime_genoff.so \
    device/google/crosshatch/recovery/root/sbin/libgptutils.so:recovery/root/sbin/libgptutils.so \
    device/google/crosshatch/recovery/root/sbin/android.hardware.keymaster@4.0-service-qti:recovery/root/sbin/android.hardware.keymaster@4.0-service-qti \
    device/google/crosshatch/recovery/root/vendor/lib64/libkeymasterdeviceutils.so:recovery/root/vendor/lib64/libkeymasterdeviceutils.so \
    device/google/crosshatch/recovery/root/vendor/lib64/libkeymasterutils.so:recovery/root/vendor/lib64/libkeymasterutils.so \
    device/google/crosshatch/recovery/root/vendor/lib64/libqtikeymaster4.so:recovery/root/vendor/lib64/libqtikeymaster4.so \
    device/google/crosshatch/recovery/root/sbin/android.hardware.weaver@1.0-service.citadel:recovery/root/sbin/android.hardware.weaver@1.0-service.citadel \
    device/google/crosshatch/recovery/root/vendor/lib64/libnos.so:recovery/root/vendor/lib64/libnos.so \
    device/google/crosshatch/recovery/root/vendor/lib64/libnosprotos.so:recovery/root/vendor/lib64/libnosprotos.so \
    device/google/crosshatch/recovery/root/vendor/lib64/android.hardware.weaver@1.0-impl.nos.so:recovery/root/vendor/lib64/android.hardware.weaver@1.0-impl.nos.so \
    device/google/crosshatch/recovery/root/vendor/lib64/libnos_citadeld_proxy.so:recovery/root/vendor/lib64/libnos_citadeld_proxy.so \
    device/google/crosshatch/recovery/root/vendor/lib64/nos_app_weaver.so:recovery/root/vendor/lib64/nos_app_weaver.so \
    device/google/crosshatch/recovery/root/vendor/lib64/libnos_datagram.so:recovery/root/vendor/lib64/libnos_datagram.so \
    device/google/crosshatch/recovery/root/sbin/vndservicemanager:recovery/root/sbin/vndservicemanager \
    device/google/crosshatch/recovery/root/vndservice_contexts:recovery/root/vndservice_contexts \
    device/google/crosshatch/recovery/root/sbin/citadeld:recovery/root/sbin/citadeld \
    device/google/crosshatch/recovery/root/vendor/lib64/libnos_client_citadel.so:recovery/root/vendor/lib64/libnos_client_citadel.so \
    device/google/crosshatch/recovery/root/vendor/lib64/libnos_transport.so:recovery/root/vendor/lib64/libnos_transport.so \
    device/google/crosshatch/recovery/root/vendor/lib64/libnos_datagram_citadel.so:recovery/root/vendor/lib64/libnos_datagram_citadel.so \
    device/google/crosshatch/recovery/root/sbin/android.hardware.boot@1.0-service:recovery/root/sbin/android.hardware.boot@1.0-service \
    device/google/crosshatch/recovery/root/vendor/lib64/hw/bootctrl.sdm845.so:recovery/root/vendor/lib64/hw/bootctrl.sdm845.so \
    device/google/crosshatch/recovery/root/vendor/lib64/hw/android.hardware.boot@1.0-impl.so:recovery/root/vendor/lib64/hw/android.hardware.boot@1.0-impl.so \
