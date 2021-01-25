LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src) 

LOCAL_JNI_SHARED_LIBRARIES := libnativeaudio-jni

LOCAL_MODULE_INCLUDE_LIBRARY := true

LOCAL_PACKAGE_NAME := NativeAudio

LOCAL_CERTIFICATE := platform

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_ASSET_DIR += $(LOCAL_PATH)/assets

include $(BUILD_PACKAGE)