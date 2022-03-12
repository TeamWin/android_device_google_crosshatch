DEVICE_PATH := device/google/crosshatch

# Inherit from common AOSP config
$(call inherit-product, $(SRC_TARGET_DIR)/product/base.mk)

# Enable project quotas and casefolding for emulated storage without sdcardfs
$(call inherit-product, $(SRC_TARGET_DIR)/product/emulated_storage.mk)

# Qcom Decryption
PRODUCT_PACKAGES += \
    qcom_decrypt \
    qcom_decrypt_fbe

# DRV2624 Haptics Waveform
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/vibrator/drv2624/drv2624.bin:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/firmware/drv2624.bin

# CS40L20 Haptics Waveform & Firmware
#PRODUCT_COPY_FILES += \
#    $(DEVICE_PATH)/vibrator/cs40l20/cs40l20.wmfw:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/firmware/cs40l20.wmfw \
#    $(DEVICE_PATH)/vibrator/cs40l20/cs40l20.bin:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/firmware/cs40l20.bin

# ueventd
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/ueventd.hardware.rc:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/ueventd.rc

# OEM otacerts
PRODUCT_EXTRA_RECOVERY_KEYS += \
    $(DEVICE_PATH)/crosshatch/recovery/security/google
