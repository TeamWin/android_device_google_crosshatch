LOCAL_PATH := $(call my-dir)

#----------------------------------------------------------------------
# Generate persist image (persist.img)
#----------------------------------------------------------------------
TARGET_OUT_PERSIST := $(PRODUCT_OUT)/persist

INTERNAL_PERSISTIMAGE_FILES := \
    $(foreach pair,$(PRODUCT_COPY_FILES),\
        $(if $(filter persist/%,$(call word-colon,2,$(pair))),\
            $(call word-colon,1,$(pair)):$(PRODUCT_OUT)/$(call word-colon,2,$(pair))))

INSTALLED_PERSISTIMAGE_FILES := $(call copy-many-files,$(INTERNAL_PERSISTIMAGE_FILES))
INSTALLED_PERSISTIMAGE_TARGET := $(PRODUCT_OUT)/persist.img

$(INSTALLED_PERSISTIMAGE_TARGET): $(MKEXTUSERIMG) $(MAKE_EXT4FS) $(INSTALLED_PERSISTIMAGE_FILES)
	$(call pretty,"Target persist fs image: $(INSTALLED_PERSISTIMAGE_TARGET)")
	@mkdir -p $(TARGET_OUT_PERSIST)
	$(hide) PATH=$(HOST_OUT_EXECUTABLES):$(PATH) $(MKEXTUSERIMG) -s $(TARGET_OUT_PERSIST) $@ ext4 persist $(BOARD_PERSISTIMAGE_PARTITION_SIZE)
	$(hide) chmod a+r $@
	$(hide) $(call assert-max-image-size,$@,$(BOARD_PERSISTIMAGE_PARTITION_SIZE))

ALL_DEFAULT_INSTALLED_MODULES += $(INSTALLED_PERSISTIMAGE_TARGET)
ALL_MODULES.$(LOCAL_MODULE).INSTALLED += $(INSTALLED_PERSISTIMAGE_TARGET)
INSTALLED_RADIOIMAGE_TARGET += $(INSTALLED_PERSISTIMAGE_TARGET)

persistimage: $(INSTALLED_PERSISTIMAGE_TARGET)

droidcore: $(INSTALLED_PERSISTIMAGE_TARGET)

#----------------------------------------------------------------------
# build and sign the final stage of bootloader
#----------------------------------------------------------------------
.PHONY: aboot
ifeq ($(USESECIMAGETOOL), true)
aboot: gensecimage_target gensecimage_install
else
aboot: $(INSTALLED_BOOTLOADER_MODULE)
endif
