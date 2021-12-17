DEVICE_PATH := device/$(BOARD_VENDOR)/$(COMMON_FOLDER)

# Inherit from common AOSP config
$(call inherit-product, $(SRC_TARGET_DIR)/product/base.mk)

# Enable project quotas and casefolding for emulated storage without sdcardfs
$(call inherit-product, $(SRC_TARGET_DIR)/product/emulated_storage.mk)

# Qcom Decryption
PRODUCT_PACKAGES += \
    qcom_decrypt \
    qcom_decrypt_fbe

# Apex libraries
PRODUCT_COPY_FILES += \
    $(OUT_DIR)/target/product/$(PRODUCT_RELEASE_NAME)/obj/SHARED_LIBRARIES/libandroidicu_intermediates/libandroidicu.so:$(TARGET_COPY_OUT_RECOVERY)/root/system/lib64/libandroidicu.so

# DRV2624 Haptics Waveform
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/vibrator/drv2624/drv2624.bin:$(TARGET_COPY_OUT_RECOVERY)/root/system/etc/firmware/drv2624.bin

# ueventd
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/ueventd.hardware.rc:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/ueventd.rc

# OEM otacerts
PRODUCT_EXTRA_RECOVERY_KEYS += \
    $(DEVICE_PATH)/recovery/security/$(BOARD_VENDOR)

# Copy modules for depmod
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)-kernel/videobuf2-memops.ko:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/lib/modules/1.1/videobuf2-memops.ko \
    $(DEVICE_PATH)-kernel/videobuf2-vmalloc.ko:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/lib/modules/1.1/videobuf2-vmalloc.ko \
    $(DEVICE_PATH)-kernel/heatmap.ko:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/lib/modules/1.1/heatmap.ko \
    $(DEVICE_PATH)-kernel/ftm5.ko:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/lib/modules/1.1/ftm5.ko \
    $(DEVICE_PATH)-kernel/sec_touch.ko:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/lib/modules/1.1/sec_touch.ko
