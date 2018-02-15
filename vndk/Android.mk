LOCAL_PATH := $(call my-dir)

# b/69526027: This VNDK-SP install routine must be removed. Instead, we must
# build vendor variants of the VNDK-SP modules.

ifndef BOARD_VNDK_VERSION
# The libs with "vndk: {enabled: true, support_system_process: true}" will be
# added VNDK_SP_LIBRARIES automatically. And the core variants of the VNDK-SP
# libs will be copied to vndk-sp directory.
# However, some of those libs need FWK-ONLY libs, which must be listed here
# manually.
VNDK_SP_LIBRARIES := \
    libdexfile

install_in_hw_dir := \
   android.hidl.memory@1.0-impl

define define-vndk-sp-lib
include $$(CLEAR_VARS)
LOCAL_MODULE := $1.vndk-sp-gen
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PREBUILT_MODULE_FILE := $$(call intermediates-dir-for,SHARED_LIBRARIES,$1,,,,)/$1.so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := first
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := $1.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_RELATIVE_PATH := vndk-sp$(if $(filter $1,$(install_in_hw_dir)),/hw)
include $$(BUILD_PREBUILT)

ifneq ($$(TARGET_2ND_ARCH),)
ifneq ($$(TARGET_TRANSLATE_2ND_ARCH),true)
include $$(CLEAR_VARS)
LOCAL_MODULE := $1.vndk-sp-gen
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PREBUILT_MODULE_FILE := $$(call intermediates-dir-for,SHARED_LIBRARIES,$1,,,$$(TARGET_2ND_ARCH_VAR_PREFIX),)/$1.so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := $1.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_RELATIVE_PATH := vndk-sp$(if $(filter $1,$(install_in_hw_dir)),/hw)
include $$(BUILD_PREBUILT)
endif # TARGET_TRANSLATE_2ND_ARCH is not true
endif # TARGET_2ND_ARCH is not empty
endef

# Add VNDK-SP libs to the list if they are missing
$(foreach lib,$(VNDK_SAMEPROCESS_LIBRARIES),\
    $(if $(filter $(lib),$(VNDK_SP_LIBRARIES)),,\
    $(eval VNDK_SP_LIBRARIES += $(lib))))

# Remove libz from the VNDK-SP list (b/73296261)
VNDK_SP_LIBRARIES := $(filter-out libz,$(VNDK_SP_LIBRARIES))

$(foreach lib,$(VNDK_SP_LIBRARIES),\
    $(eval $(call define-vndk-sp-lib,$(lib))))

install_in_hw_dir :=

include $(CLEAR_VARS)
LOCAL_MODULE := vndk-sp
LOCAL_MODULE_OWNER := google
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES := $(addsuffix .vndk-sp-gen,$(VNDK_SP_LIBRARIES))
include $(BUILD_PHONY_PACKAGE)
endif
