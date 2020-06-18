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
    vendor/qcom/crosshatch/proprietary \

# AOSP packages required by the blobs
PRODUCT_PACKAGES := \
    ims \
    libprotobuf-cpp-full \
    QtiTelephonyService

#  blob(s) necessary for crosshatch hardware
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
    vendor/qcom/crosshatch/proprietary/com.qualcomm.qti.imscmservice.xml:system/etc/permissions/com.qualcomm.qti.imscmservice.xml:qcom \
    vendor/qcom/crosshatch/proprietary/qcrilhook.jar:system/framework/qcrilhook.jar:qcom \
    vendor/qcom/crosshatch/proprietary/qcrilhook.xml:system/etc/permissions/qcrilhook.xml:qcom \
    vendor/qcom/crosshatch/proprietary/QtiTelephonyServicelibrary.jar:system/framework/QtiTelephonyServicelibrary.jar:qcom \
    vendor/qcom/crosshatch/proprietary/radioconfiginterfacelibrary.jar:system/framework/radioconfiginterfacelibrary.jar:qcom \
    vendor/qcom/crosshatch/proprietary/radioconfiglibrary.jar:system/framework/radioconfiglibrary.jar:qcom \
    vendor/qcom/crosshatch/proprietary/telephonyservice.xml:system/etc/permissions/telephonyservice.xml:qcom \
    vendor/qcom/crosshatch/proprietary/uimremoteclientlibrary.jar:system/framework/uimremoteclientlibrary.jar:qcom \
    vendor/qcom/crosshatch/proprietary/uimremoteclient.xml:system/etc/permissions/uimremoteclient.xml:qcom \
    vendor/qcom/crosshatch/proprietary/uimremoteserverlibrary.jar:system/framework/uimremoteserverlibrary.jar:qcom \
    vendor/qcom/crosshatch/proprietary/uimremoteserver.xml:system/etc/permissions/uimremoteserver.xml:qcom \
    vendor/qcom/crosshatch/proprietary/UimService.xml:system/etc/permissions/UimService.xml:qcom \

