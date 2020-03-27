# Copyright 2018 The Android Open Source Project
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

PRODUCT_SOONG_NAMESPACES += \
    vendor/qcom/blueline/proprietary \

# AOSP packages required by the blobs
PRODUCT_PACKAGES := \
    ims \
    libprotobuf-cpp-full \
    QtiTelephonyService

#  blob(s) necessary for blueline hardware
PRODUCT_PACKAGES += \
    libimscamera_jni \
    libimsmedia_jni \
    lib-imsvideocodec \
    lib-imsvtextutils \
    lib-imsvt \
    lib-imsvtutils \
    librcc \
    libsdm-disp-apis \
    libsecureui_svcsock_system \

PRODUCT_COPY_FILES := \
    vendor/qcom/blueline/proprietary/com.qualcomm.qti.imscmservice.xml:system/etc/permissions/com.qualcomm.qti.imscmservice.xml:qcom \
    vendor/qcom/blueline/proprietary/qcrilhook.jar:system/framework/qcrilhook.jar:qcom \
    vendor/qcom/blueline/proprietary/qcrilhook.xml:system/etc/permissions/qcrilhook.xml:qcom \
    vendor/qcom/blueline/proprietary/QtiTelephonyServicelibrary.jar:system/framework/QtiTelephonyServicelibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/radioconfiginterfacelibrary.jar:system/framework/radioconfiginterfacelibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/radioconfiginterface.xml:system/etc/permissions/radioconfiginterface.xml:qcom \
    vendor/qcom/blueline/proprietary/radioconfiglibrary.jar:system/framework/radioconfiglibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/radioconfig.xml:system/etc/permissions/radioconfig.xml:qcom \
    vendor/qcom/blueline/proprietary/telephonyservice.xml:system/etc/permissions/telephonyservice.xml:qcom \
    vendor/qcom/blueline/proprietary/uimremoteclientlibrary.jar:system/framework/uimremoteclientlibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/uimremoteclient.xml:system/etc/permissions/uimremoteclient.xml:qcom \
    vendor/qcom/blueline/proprietary/uimremoteserverlibrary.jar:system/framework/uimremoteserverlibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/uimremoteserver.xml:system/etc/permissions/uimremoteserver.xml:qcom \
    vendor/qcom/blueline/proprietary/UimService.xml:system/etc/permissions/UimService.xml:qcom \

