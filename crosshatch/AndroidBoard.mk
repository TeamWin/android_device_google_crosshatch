LOCAL_PATH := $(call my-dir)

#A/B builds require us to create the mount points at compile time.
#Just creating it for all cases since it does not hurt.
FIRMWARE_MOUNT_POINT := $(TARGET_OUT_VENDOR)/firmware_mnt
ALL_DEFAULT_INSTALLED_MODULES += $(FIRMWARE_MOUNT_POINT)

$(FIRMWARE_MOUNT_POINT):
	@echo "Creating $(FIRMWARE_MOUNT_POINT)"
	@mkdir -p $(TARGET_OUT_VENDOR)/firmware_mnt

#----------------------------------------------------------------------
# Generate persist image (persist.img)
#----------------------------------------------------------------------
TARGET_OUT_PERSIST_IMG_PATH := $(PRODUCT_OUT)/persist

INTERNAL_PERSISTIMAGE_FILES := \
    $(foreach pair,$(PRODUCT_COPY_FILES),\
        $(if $(filter persist/%,$(call word-colon,2,$(pair))),\
            $(call word-colon,1,$(pair)):$(PRODUCT_OUT)/$(call word-colon,2,$(pair))))

INSTALLED_PERSISTIMAGE_FILES := $(call copy-many-files,$(INTERNAL_PERSISTIMAGE_FILES))
INSTALLED_PERSISTIMAGE_TARGET := $(PRODUCT_OUT)/persist.img

$(INSTALLED_PERSISTIMAGE_TARGET): $(MKEXTUSERIMG) $(MAKE_EXT4FS) $(INSTALLED_PERSISTIMAGE_FILES)
	$(call pretty,"Target persist fs image: $(INSTALLED_PERSISTIMAGE_TARGET)")
	@mkdir -p $(TARGET_OUT_PERSIST_IMG_PATH)
	$(hide) PATH=$(HOST_OUT_EXECUTABLES):$${PATH} $(MKEXTUSERIMG) -s $(TARGET_OUT_PERSIST_IMG_PATH) $@ ext4 persist $(BOARD_PERSISTIMAGE_PARTITION_SIZE)
	$(hide) chmod a+r $@
	$(hide) $(call assert-max-image-size,$@,$(BOARD_PERSISTIMAGE_PARTITION_SIZE))

ALL_DEFAULT_INSTALLED_MODULES += $(INSTALLED_PERSISTIMAGE_TARGET)
ALL_MODULES.$(LOCAL_MODULE).INSTALLED += $(INSTALLED_PERSISTIMAGE_TARGET)
INSTALLED_RADIOIMAGE_TARGET += $(INSTALLED_PERSISTIMAGE_TARGET)


INTERNAL_KERNEL_MODULES := \
	$(foreach file,$(wildcard device/google/crosshatch-kernel/*.ko \
	    device/google/crosshatch-kernel/modules.dep),\
	    $(file):$(TARGET_OUT_VENDOR)/lib/modules/$(notdir $(file)))

INSTALLED_KERNEL_MODULES := $(call copy-many-files,$(INTERNAL_KERNEL_MODULES))

.PHONY: kernel-modules
kernel-modules: $(INSTALLED_KERNEL_MODULES)
	@echo Kernel modules installed

.PHONY: persistimage
persistimage: $(INSTALLED_PERSISTIMAGE_TARGET)

droidcore: $(INSTALLED_PERSISTIMAGE_TARGET) $(INSTALLED_KERNEL_MODULES)


# copy kernel headers to the build tree
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr: $(wildcard $(PRODUCT_VENDOR_KERNEL_HEADERS)/*)
	rm -rf $@
	mkdir -p $@/include
	cp -a $(PRODUCT_VENDOR_KERNEL_HEADERS)/. $@/include

#----------------------------------------------------------------------
# build and sign the final stage of bootloader
#----------------------------------------------------------------------
.PHONY: aboot
ifeq ($(USESECIMAGETOOL), true)
aboot: gensecimage_target gensecimage_install
else
aboot: $(INSTALLED_BOOTLOADER_MODULE)
endif
