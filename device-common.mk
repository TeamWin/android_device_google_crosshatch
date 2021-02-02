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

# Enable HW Codec 2.0 as default service
# Set all codec components are available with their normal ranks
# Set OMX components's default rank large than Codec 2.0 HW components's default rank (0x100)
PRODUCT_PROPERTY_OVERRIDES += debug.stagefright.ccodec=4
PRODUCT_PROPERTY_OVERRIDES += debug.stagefright.omx_default_rank=512

# Pixelstats broken mic detection
PRODUCT_PROPERTY_OVERRIDES += vendor.audio.mic_break=true

# Setting vendor SPL
VENDOR_SECURITY_PATCH = $(PLATFORM_SECURITY_PATCH)

# Set boot SPL
BOOT_SECURITY_PATCH = $(PLATFORM_SECURITY_PATCH)

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
PRODUCT_PRODUCT_PROPERTIES += \
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

# Dmabuf dump tool for bug reports
PRODUCT_PACKAGES += \
    dmabuf_dump

# whitelisted app
PRODUCT_COPY_FILES += \
    device/google/crosshatch/qti_whitelist.xml:$(TARGET_COPY_OUT_PRODUCT)/etc/sysconfig/qti_whitelist.xml

PRODUCT_PACKAGES += \
    llkd
#PRODUCT_PROPERTY_OVERRIDES += \
#    ro.khungtask.enable=false
#

# Enable retrofit dynamic partitions for all blueline
# and crosshatch targets
PRODUCT_USE_DYNAMIC_PARTITIONS := true
PRODUCT_RETROFIT_DYNAMIC_PARTITIONS := true
PRODUCT_PACKAGES += \
    android.hardware.boot@1.0-impl.recovery \
    bootctrl.sdm845 \
    bootctrl.sdm845.recovery \
    check_dynamic_partitions \

AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_product=true \
    POSTINSTALL_PATH_product=bin/check_dynamic_partitions \
    FILESYSTEM_TYPE_product=ext4 \
    POSTINSTALL_OPTIONAL_product=false \

# Set thermal warm reset
PRODUCT_PRODUCT_PROPERTIES += \
    ro.thermal_warmreset = true \

#touch modules
PRODUCT_COPY_FILES += \
    device/google/crosshatch/modules/sec_touch.ko:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/sec_touch.ko \
    device/google/crosshatch/modules/ftm5.ko:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/ftm5.ko \
    device/google/crosshatch/modules/heatmap.ko:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/heatmap.ko \
    device/google/crosshatch/modules/videobuf2-memops.ko:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/videobuf2-memops.ko \
    device/google/crosshatch/modules/videobuf2-vmalloc.ko:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/videobuf2-vmalloc.ko \
    device/google/crosshatch/touchdriver.sh:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/touchdriver.sh \
    device/google/crosshatch/prebuilts/qseecomd:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/qseecomd \
    device/google/crosshatch/prebuilts/libdrmfs.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libdrmfs.so \
    device/google/crosshatch/prebuilts/libxml2.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libxml2.so \
    device/google/crosshatch/prebuilts/libnetd_client.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libnetd_client.so \
    device/google/crosshatch/prebuilts/libspcom.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libspcom.so \
    device/google/crosshatch/prebuilts/libkeymasterdeviceutils.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libkeymasterdeviceutils.so \
    device/google/crosshatch/prebuilts/libkeymasterutils.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libkeymasterutils.so \
    device/google/crosshatch/prebuilts/libqtikeymaster4.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libqtikeymaster4.so \
    device/google/crosshatch/prebuilts/libQSEEComAPI.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libQSEEComAPI.so \
    device/google/crosshatch/prebuilts/libdiag.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libdiag.so \
    device/google/crosshatch/prebuilts/libnos_client_citadel.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libnos_client_citadel.so \
    device/google/crosshatch/prebuilts/libnos_transport.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libnos_transport.so \
    device/google/crosshatch/prebuilts/citadeld:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/citadeld \
    device/google/crosshatch/prebuilts/android.hardware.keymaster@4.0-service.citadel:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/android.hardware.keymaster@4.0-service.citadel \
    device/google/crosshatch/prebuilts/prepdecrypt.sh:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/prepdecrypt.sh \
    device/google/crosshatch/prebuilts/time_daemon:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/time_daemon \
    device/google/crosshatch/prebuilts/libqmi_cci.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libqmi_cci.so \
    device/google/crosshatch/prebuilts/libqmi_common_so.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libqmi_common_so.so \
    device/google/crosshatch/prebuilts/manifest.xml:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/manifest.xml \
    device/google/crosshatch/prebuilts/systemmanifest.xml:$(TARGET_COPY_OUT_RECOVERY)/root/system/etc/vintf/manifest.xml \
    device/google/crosshatch/prebuilts/compatibility_matrix.device.xml:$(TARGET_COPY_OUT_RECOVERY)/root/system/etc/vintf/compatibility_matrix.device.xml \
    device/google/crosshatch/prebuilts/compatibility_matrix.legacy.xml:$(TARGET_COPY_OUT_RECOVERY)/root/system/etc/vintf/compatibility_matrix.legacy.xml \
    device/google/crosshatch/prebuilts/android.hardware.gatekeeper@1.0-service-qti:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/android.hardware.gatekeeper@1.0-service-qti \
    device/google/crosshatch/prebuilts/android.hardware.keymaster@4.0-service-qti:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/android.hardware.keymaster@4.0-service-qti \
    device/google/crosshatch/prebuilts/android.hardware.boot@1.0-service:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/android.hardware.boot@1.0-service \
    device/google/crosshatch/prebuilts/android.hardware.weaver@1.0-impl.nos.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/android.hardware.weaver@1.0-impl.nos.so \
    device/google/crosshatch/prebuilts/android.hardware.weaver@1.0-service.citadel:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/android.hardware.weaver@1.0-service.citadel \
    device/google/crosshatch/prebuilts/android.hardware.authsecret@1.0-service.citadel:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/android.hardware.authsecret@1.0-service.citadel \
    device/google/crosshatch/prebuilts/android.hardware.authsecret@1.0-impl.nos.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/android.hardware.authsecret@1.0-impl.nos.so \
    device/google/crosshatch/prebuilts/android.hardware.oemlock@1.0-service.citadel:$(TARGET_COPY_OUT_RECOVERY)/root/system/bin/android.hardware.oemlock@1.0-service.citadel \
    device/google/crosshatch/prebuilts/nos_app_weaver.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/nos_app_weaver.so \
    device/google/crosshatch/prebuilts/libnos_citadeld_proxy.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libnos_citadeld_proxy.so \
    device/google/crosshatch/prebuilts/libnos_datagram_citadel.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libnos_datagram_citadel.so\
    device/google/crosshatch/prebuilts/libprotobuf-cpp-full.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libprotobuf-cpp-full.so \
    device/google/crosshatch/prebuilts/pixelpowerstats_provider_aidl_interface-cpp.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/pixelpowerstats_provider_aidl_interface-cpp.so \
    device/google/crosshatch/prebuilts/libssd.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libssd.so \
    device/google/crosshatch/prebuilts/libsecureui.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libsecureui.so \
    device/google/crosshatch/prebuilts/librpmb.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/librpmb.so \
    device/google/crosshatch/prebuilts/libdrmtime.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libdrmtime.so \
    device/google/crosshatch/prebuilts/libnos.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libnos.so \
    device/google/crosshatch/prebuilts/libnosprotos.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libnosprotos.so \
    device/google/crosshatch/prebuilts/nos_app_avb.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/nos_app_avb.so \
    device/google/crosshatch/prebuilts/nos_app_keymaster.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/nos_app_keymaster.so \
    device/google/crosshatch/prebuilts/vendor-pixelatoms-cpp.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/vendor-pixelatoms-cpp.so \
    device/google/crosshatch/prebuilts/android.hardware.oemlock@1.0-impl.nos.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/android.hardware.oemlock@1.0-impl.nos.so \
    device/google/crosshatch/prebuilts/android.hardware.keymaster@4.0-impl.nos.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/android.hardware.keymaster@4.0-impl.nos.so \
    device/google/crosshatch/prebuilts/twrp.flags:$(TARGET_COPY_OUT_RECOVERY)/root/system/etc/twrp.flags \
    device/google/crosshatch/prebuilts/ueventd.rc:$(TARGET_COPY_OUT_RECOVERY)/root/ueventd.rc \

PRODUCT_PACKAGES += \
    magiskboot_arm
