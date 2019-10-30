# Copyright 2017 The Android Open Source Project
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

# If you don't need to do a full clean build but would like to touch
# a file or delete some intermediate files, add a clean step to the end
# of the list.  These steps will only be run once, if they haven't been
# run before.
#
# E.g.:
#     $(call add-clean-step, touch -c external/sqlite/sqlite3.h)
#     $(call add-clean-step, rm -rf $(PRODUCT_OUT)/obj/STATIC_LIBRARIES/libz_intermediates)
#
# Always use "touch -c" and "rm -f" or "rm -rf" to gracefully deal with
# files that are missing or have been moved.
#
# Use $(PRODUCT_OUT) to get to the "out/target/product/blah/" directory.
# Use $(OUT_DIR) to refer to the "out" directory.
#
# If you need to re-do something that's already mentioned, just copy
# the command and add it to the bottom of the list.  E.g., if a change
# that you made last week required touching a file and a change you
# made today requires touching the same file, just copy the old
# touch step and add it to the end of the list.
#
# ************************************************
# NEWER CLEAN STEPS MUST BE AT THE END OF THE LIST
# ************************************************

# For example:
#$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/AndroidTests_intermediates)
#$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/JAVA_LIBRARIES/core_intermediates)
#$(call add-clean-step, find $(OUT_DIR) -type f -name "IGTalkSession*" -print0 | xargs -0 rm -f)
#$(call add-clean-step, rm -rf $(PRODUCT_OUT)/data/*)

$(call add-clean-step, rm -rf $(PRODUCT_OUT))
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor)

# Remove /system/lib[64]/vndk-sp/libz.so
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/lib/vndk-sp/libz.so)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/lib64/vndk-sp/libz.so)

# Remove Power HAL 1.2
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.power@1.2-service.crosshatch-libperfmgr.rc)

# Remove Vibrator HAL 1.1
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.vibrator@1.1-service.crosshatch.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.vibrator@1.1-service.crosshatch)

# Remove /product mount point
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/product)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/root/product)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/product)

# Remove android.hardware.audio*@2.0 implementation
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/lib/hw/android.hardware.audio*@2.0-impl.so)

# Adds product.img
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/product)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/root/product)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/app)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/etc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/fonts)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/framework)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/media)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/priv-app)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/product)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/usr)

# Remove default android.hardware.health@2.0-service
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.health@2.0-service)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.health@2.0-service.rc)

# Remove healthd
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/bin/healthd)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/etc/init/healthd.rc)

# Remove KM3 RC
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.keymaster@3.0-service-qti.rc)

# Remove Codec2.0 software process
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/hw/hardware.google.media.c2@1.0-service-software)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/hardware.google.media.c2@1.0-service-software.rc)

# Remove default android.hardware.graphics.composer@2.1
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.graphics.composer@2.1-service)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.graphics.composer@2.1-service.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/lib64/hw/android.hardware.graphics.composer@2.1-impl.so)

# Remove StrongBox RC
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.keymaster@4.0-service.citadel.rc)

# Migrate to versioned VNDK directory
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/lib/vndk-sp)

# Remove non-qti-display mapper (b/79269048)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/lib/hw/android.hardware.graphics.mapper@2.0-impl.so)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/lib64/hw/android.hardware.graphics.mapper@2.0-impl.so)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/lib/hw/android.hardware.graphics.allocator@2.0-impl.so)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/lib64/hw/android.hardware.graphics.allocator@2.0-impl.so)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.graphics.allocator@2.0-service)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.graphics.allocator@2.0-service.rc)

#AU300 cleanup
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/product)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/obj/include)

# Secure_element HAL for eSE1
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.secure_element@1.0-service.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.secure_element@1.0-service)

# ThermalHAL 1.1
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.thermal@1.1-service.crosshatch.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.thermal@1.1-service.crosshatch)

# Verified boot xml moved to /product
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/etc/permissions/android.software.verified_boot.xml)

# Move libnfc-nci.conf to /product
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/etc/libnfc-nci.conf)

# Remove default android.hardware.graphics.composer@2.3
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.graphics.composer@2.2-service)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.graphics.composer@2.2-service.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/lib64/hw/android.hardware.graphics.composer@2.2-impl.so)

# Remove /firmware which used to be a symlink to /vendor/firmware_mnt
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/root/firmware)

$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/app/webview)

# Moved to /product
$(call add-clean-step, rm -f $(PRODUCT_OUT)/system/etc/sysconfig/qti_whitelist.xml)

# Rename power HAL
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.power@1.3-service.crosshatch-libperfmgr.rc)

# Remove generic atrace HAL
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.atrace@1.0-service.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/vintf/manifest/android.hardware.atrace@1.0-service.xml)

# Combine memtrack/thermal/vibrator HALs into small_hals.crosshatch-service
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.vibrator@1.2-service.crosshatch.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.vibrator@1.2-service.crosshatch)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/etc/init/android.hardware.memtrack@1.0-service.rc)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/vendor/bin/hw/android.hardware.memtrack@1.0-service)
