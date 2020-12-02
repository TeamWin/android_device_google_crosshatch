#
# Copyright 2020 The Android Open Source Project
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

# Makefile for the system-only partial build of aosp_crosshatch.
# Used for creating a merged Vendor Freeze device.

# Include VNDK v30, needed by the Android 11 vendor half.
PRODUCT_EXTRA_VNDK_VERSIONS = 30

# Disable building certain non-system partitions in this build.
PRODUCT_BUILD_BOOT_IMAGE := false
PRODUCT_BUILD_RAMDISK_IMAGE := false
PRODUCT_BUILD_RECOVERY_IMAGE := false
PRODUCT_BUILD_VBMETA_IMAGE := false
PRODUCT_BUILD_VENDOR_IMAGE := false
PRODUCT_BUILD_VENDOR_BOOT_IMAGE := false

$(call inherit-product, device/google/crosshatch/aosp_crosshatch.mk)

PRODUCT_NAME := aosp_crosshatch_vf
