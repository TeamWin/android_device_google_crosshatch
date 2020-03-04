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

# AOSP packages required by the blobs
PRODUCT_PACKAGES := \
    ims \
    libprotobuf-cpp-full \
    QtiTelephonyService

#  blob(s) necessary for blueline hardware
PRODUCT_COPY_FILES := \
    vendor/qcom/blueline/proprietary/cneapiclient.xml:system/etc/permissions/cneapiclient.xml:qcom \
    vendor/qcom/blueline/proprietary/com.qualcomm.qti.imscmservice.xml:system/etc/permissions/com.qualcomm.qti.imscmservice.xml:qcom \
    vendor/qcom/blueline/proprietary/com.quicinc.cne.xml:system/etc/permissions/com.quicinc.cne.xml:qcom \
    vendor/qcom/blueline/proprietary/lib64/libGPQTEEC_system.so:system/lib64/libGPQTEEC_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libGPTEE_system.so:system/lib64/libGPTEE_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libimscamera_jni.so:system/lib64/libimscamera_jni.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libimsmedia_jni.so:system/lib64/libimsmedia_jni.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvideocodec.so:system/lib64/lib-imsvideocodec.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvtextutils.so:system/lib64/lib-imsvtextutils.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvt.so:system/lib64/lib-imsvt.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvtutils.so:system/lib64/lib-imsvtutils.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libQTEEConnector_system.so:system/lib64/libQTEEConnector_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/librcc.so:system/lib64/librcc.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsdm-disp-apis.so:system/lib64/libsdm-disp-apis.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsecureui_svcsock_system.so:system/lib64/libsecureui_svcsock_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsmcinvokecred.so:system/lib64/libsmcinvokecred.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libtzcom.so:system/lib64/libtzcom.so:qcom \
    vendor/qcom/blueline/proprietary/libGPQTEEC_system.so:system/lib/libGPQTEEC_system.so:qcom \
    vendor/qcom/blueline/proprietary/libGPTEE_system.so:system/lib/libGPTEE_system.so:qcom \
    vendor/qcom/blueline/proprietary/libimscamera_jni.so:system/lib/libimscamera_jni.so:qcom \
    vendor/qcom/blueline/proprietary/libimsmedia_jni.so:system/lib/libimsmedia_jni.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvideocodec.so:system/lib/lib-imsvideocodec.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvtextutils.so:system/lib/lib-imsvtextutils.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvt.so:system/lib/lib-imsvt.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvtutils.so:system/lib/lib-imsvtutils.so:qcom \
    vendor/qcom/blueline/proprietary/libQTEEConnector_system.so:system/lib/libQTEEConnector_system.so:qcom \
    vendor/qcom/blueline/proprietary/librcc.so:system/lib/librcc.so:qcom \
    vendor/qcom/blueline/proprietary/libsdm-disp-apis.so:system/lib/libsdm-disp-apis.so:qcom \
    vendor/qcom/blueline/proprietary/libsecureui_svcsock_system.so:system/lib/libsecureui_svcsock_system.so:qcom \
    vendor/qcom/blueline/proprietary/libsns_low_lat_stream_skel.so:system/lib/rfsa/adsp/libsns_low_lat_stream_skel.so:qcom \
    vendor/qcom/blueline/proprietary/libsmcinvokecred.so:system/lib/libsmcinvokecred.so:qcom \
    vendor/qcom/blueline/proprietary/libtzcom.so:system/lib/libtzcom.so:qcom \
    vendor/qcom/blueline/proprietary/LteDirectDiscoveryLibrary.jar:system/framework/LteDirectDiscoveryLibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/LteDirectDiscovery.xml:system/etc/permissions/LteDirectDiscovery.xml:qcom \
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

