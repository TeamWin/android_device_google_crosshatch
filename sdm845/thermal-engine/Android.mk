################################################################################
# @file device/qcom/common/thermal-engine/Android.mk
# @Makefile for installing thermal-engine client header on Android.
################################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libThermal_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
LOCAL_VENDOR_MODULE := true
include $(BUILD_HEADER_LIBRARY)
