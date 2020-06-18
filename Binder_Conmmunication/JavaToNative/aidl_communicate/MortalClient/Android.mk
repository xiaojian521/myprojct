
LOCAL_PATH:= $(call my-dir)

//====================================================================
include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/priv-app

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := MortalClient

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res 

LOCAL_STATIC_JAVA_LIBRARIES := libaidl_java

LOCAL_PRIVILEGED_MODULE := true

include $(BUILD_PACKAGE)

//====================================================================


//aidl java lib=======================================================

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-Iaidl-files-under, aidl)

LOCAL_AIDL_INCLUDES += \
    $(LOCAL_PATH)/aidl

LOCAL_MODULE := libaidl_java

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_JAVA_LIBRARY)

//aidl java lib=======================================================