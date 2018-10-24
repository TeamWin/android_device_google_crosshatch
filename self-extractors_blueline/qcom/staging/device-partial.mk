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
    com.android.ims.rcsmanager.xml \
    com.android.ims.rcsmanager

#  blob(s) necessary for blueline hardware
PRODUCT_COPY_FILES := \
    vendor/qcom/blueline/proprietary/cneapiclient.xml:system/etc/permissions/cneapiclient.xml:qcom \
    vendor/qcom/blueline/proprietary/com.google.android.factoryota.xml:system/etc/permissions/com.google.android.factoryota.xml:qcom \
    vendor/qcom/blueline/proprietary/com.qualcomm.qti.imscmservice.xml:system/etc/permissions/com.qualcomm.qti.imscmservice.xml:qcom \
    vendor/qcom/blueline/proprietary/com.qualcomm.qti.uceservice-V2.0-java.jar:system/framework/com.qualcomm.qti.uceservice-V2.0-java.jar:qcom \
    vendor/qcom/blueline/proprietary/com.quicinc.cne.xml:system/etc/permissions/com.quicinc.cne.xml:qcom \
    vendor/qcom/blueline/proprietary/dnd.descriptor:system/etc/firmware/dnd.descriptor:qcom \
    vendor/qcom/blueline/proprietary/dnd.sound_model:system/etc/firmware/dnd.sound_model:qcom \
    vendor/qcom/blueline/proprietary/embmslibrary.jar:system/framework/embmslibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/embms.xml:system/etc/permissions/embms.xml:qcom \
    vendor/qcom/blueline/proprietary/init-persist.rc:system/etc/init/init-persist.rc:qcom \
    vendor/qcom/blueline/proprietary/lib64/libadsprpc_system.so:system/lib64/libadsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libcdsprpc_system.so:system/lib64/libcdsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libDiagService.so:system/lib64/libDiagService.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libdiag_system.so:system/lib64/libdiag_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libGPQTEEC_system.so:system/lib64/libGPQTEEC_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libGPTEE_system.so:system/lib64/libGPTEE_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libimscamera_jni.so:system/lib64/libimscamera_jni.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libimsmedia_jni.so:system/lib64/libimsmedia_jni.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvideocodec.so:system/lib64/lib-imsvideocodec.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvtextutils.so:system/lib64/lib-imsvtextutils.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvt.so:system/lib64/lib-imsvt.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/lib-imsvtutils.so:system/lib64/lib-imsvtutils.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/liblistenjni.so:system/lib64/liblistenjni.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/liblistensoundmodel2.so:system/lib64/liblistensoundmodel2.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libmdsprpc_system.so:system/lib64/libmdsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libQTEEConnector_system.so:system/lib64/libQTEEConnector_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/librcc.so:system/lib64/librcc.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsdm-disp-apis.so:system/lib64/libsdm-disp-apis.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsdsprpc_system.so:system/lib64/libsdsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsecureuisvc_jni.so:system/lib64/libsecureuisvc_jni.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsecureui_svcsock_system.so:system/lib64/libsecureui_svcsock_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsensorslog.so:system/lib64/libsensorslog.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsns_low_lat_stream_stub.so:system/lib64/libsns_low_lat_stream_stub.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libOpenCL_system.so:system/lib64/libOpenCL_system.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libprotobuf-cpp-full-rtti.so:system/lib64/libprotobuf-cpp-full-rtti.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libqcbor.so:system/lib64/libqcbor.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libsmcinvokecred.so:system/lib64/libsmcinvokecred.so:qcom \
    vendor/qcom/blueline/proprietary/lib64/libtzcom.so:system/lib64/libtzcom.so:qcom \
    vendor/qcom/blueline/proprietary/libadsprpc_system.so:system/lib/libadsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/libcdsprpc_system.so:system/lib/libcdsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/libdiag_system.so:system/lib/libdiag_system.so:qcom \
    vendor/qcom/blueline/proprietary/libGPQTEEC_system.so:system/lib/libGPQTEEC_system.so:qcom \
    vendor/qcom/blueline/proprietary/libGPTEE_system.so:system/lib/libGPTEE_system.so:qcom \
    vendor/qcom/blueline/proprietary/libimscamera_jni.so:system/lib/libimscamera_jni.so:qcom \
    vendor/qcom/blueline/proprietary/libimsmedia_jni.so:system/lib/libimsmedia_jni.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvideocodec.so:system/lib/lib-imsvideocodec.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvtextutils.so:system/lib/lib-imsvtextutils.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvt.so:system/lib/lib-imsvt.so:qcom \
    vendor/qcom/blueline/proprietary/lib-imsvtutils.so:system/lib/lib-imsvtutils.so:qcom \
    vendor/qcom/blueline/proprietary/liblistenjni.so:system/lib/liblistenjni.so:qcom \
    vendor/qcom/blueline/proprietary/liblistensoundmodel2.so:system/lib/liblistensoundmodel2.so:qcom \
    vendor/qcom/blueline/proprietary/liblogwrap.so:system/lib/liblogwrap.so:qcom \
    vendor/qcom/blueline/proprietary/libmdsprpc_system.so:system/lib/libmdsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/libprotobuf-cpp-full.so:system/lib/libprotobuf-cpp-full.so:qcom \
    vendor/qcom/blueline/proprietary/libqct_resampler.so:system/lib/libqct_resampler.so:qcom \
    vendor/qcom/blueline/proprietary/libQTEEConnector_system.so:system/lib/libQTEEConnector_system.so:qcom \
    vendor/qcom/blueline/proprietary/librcc.so:system/lib/librcc.so:qcom \
    vendor/qcom/blueline/proprietary/libsdm-disp-apis.so:system/lib/libsdm-disp-apis.so:qcom \
    vendor/qcom/blueline/proprietary/libsdsprpc_system.so:system/lib/libsdsprpc_system.so:qcom \
    vendor/qcom/blueline/proprietary/libsecureuisvc_jni.so:system/lib/libsecureuisvc_jni.so:qcom \
    vendor/qcom/blueline/proprietary/libsecureui_svcsock_system.so:system/lib/libsecureui_svcsock_system.so:qcom \
    vendor/qcom/blueline/proprietary/libsensorslog.so:system/lib/libsensorslog.so:qcom \
    vendor/qcom/blueline/proprietary/libsns_low_lat_stream_skel.so:system/lib/rfsa/adsp/libsns_low_lat_stream_skel.so:qcom \
    vendor/qcom/blueline/proprietary/libsns_low_lat_stream_stub.so:system/lib/libsns_low_lat_stream_stub.so:qcom \
    vendor/qcom/blueline/proprietary/libOpenCL_system.so:system/lib/libOpenCL_system.so:qcom \
    vendor/qcom/blueline/proprietary/libprotobuf-cpp-full-rtti.so:system/lib/libprotobuf-cpp-full-rtti.so:qcom \
    vendor/qcom/blueline/proprietary/libqcbor.so:system/lib/libqcbor.so:qcom \
    vendor/qcom/blueline/proprietary/libsmcinvokecred.so:system/lib/libsmcinvokecred.so:qcom \
    vendor/qcom/blueline/proprietary/libtzcom.so:system/lib/libtzcom.so:qcom \
    vendor/qcom/blueline/proprietary/lpa.xml:system/etc/permissions/lpa.xml:qcom \
    vendor/qcom/blueline/proprietary/LteDirectDiscoveryLibrary.jar:system/framework/LteDirectDiscoveryLibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/LteDirectDiscovery.xml:system/etc/permissions/LteDirectDiscovery.xml:qcom \
    vendor/qcom/blueline/proprietary/music_detector.descriptor:system/etc/firmware/music_detector.descriptor:qcom \
    vendor/qcom/blueline/proprietary/music_detector.sound_model:system/etc/firmware/music_detector.sound_model:qcom \
    vendor/qcom/blueline/proprietary/qcrilhook.jar:system/framework/qcrilhook.jar:qcom \
    vendor/qcom/blueline/proprietary/qcrilhook.xml:system/etc/permissions/qcrilhook.xml:qcom \
    vendor/qcom/blueline/proprietary/QtiTelephonyServicelibrary.jar:system/framework/QtiTelephonyServicelibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/radioconfiginterfacelibrary.jar:system/framework/radioconfiginterfacelibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/radioconfiginterface.xml:system/etc/permissions/radioconfiginterface.xml:qcom \
    vendor/qcom/blueline/proprietary/radioconfiglibrary.jar:system/framework/radioconfiglibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/radioconfig.xml:system/etc/permissions/radioconfig.xml:qcom \
    vendor/qcom/blueline/proprietary/RemoteSimlock.xml:system/etc/permissions/RemoteSimlock.xml:qcom \
    vendor/qcom/blueline/proprietary/telephonyservice.xml:system/etc/permissions/telephonyservice.xml:qcom \
    vendor/qcom/blueline/proprietary/uimlpalibrary.jar:system/framework/uimlpalibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/uimremoteclientlibrary.jar:system/framework/uimremoteclientlibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/uimremoteclient.xml:system/etc/permissions/uimremoteclient.xml:qcom \
    vendor/qcom/blueline/proprietary/uimremoteserverlibrary.jar:system/framework/uimremoteserverlibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/uimremoteserver.xml:system/etc/permissions/uimremoteserver.xml:qcom \
    vendor/qcom/blueline/proprietary/uimremotesimlocklibrary.jar:system/framework/uimremotesimlocklibrary.jar:qcom \
    vendor/qcom/blueline/proprietary/UimService.xml:system/etc/permissions/UimService.xml:qcom \
    vendor/qcom/blueline/proprietary/vendor.qti.hardware.alarm-V1.0-java.jar:system/framework/vendor.qti.hardware.alarm-V1.0-java.jar:qcom \
    vendor/qcom/blueline/proprietary/vendor.qti.hardware.data.latency-V1.0-java.jar:system/framework/vendor.qti.hardware.data.latency-V1.0-java.jar:qcom \
    vendor/qcom/blueline/proprietary/vendor.qti.hardware.soter-V1.0-java.jar:system/framework/vendor.qti.hardware.soter-V1.0-java.jar:qcom \

