LOCAL_PATH:=$(call my-dir)

#==================native idl so=====================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-Iaidl-files-under, aidl)

LOCAL_AIDL_INCLUDES := $(LOCAL_PATH)/aidl

LOCAL_MODULE := libaidl_native

# LOCAL_MODULE_CLASS := SHARED_LIBRARIES

#保持动态库符号表,及debug信息
LOCAL_STRIP_MODULE := keep_symbols

#导入头文件,比如parcel的class文件
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/aidl \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libbinder

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)
#==================native aidl so=====================================


#==================bin file===========================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := ./src/DeleteFile.cpp \
                   ./src/DeleteFileService.cpp

LOCAL_C_INCLUDES := ./src/DeleteFile.h \
                    $(JNI_H_INCLUDE)

LOCAL_SHARED_LIBRARIES := \
		liblog \
		libutils \
        libbinder \
		libcutils \
		libandroid_runtime \

LOCAL_STATIC_LIBRARIES := \
        libaidl_native \

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib-log

LOCAL_MODULE := XJTEST

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
#==================bin file===========================================


                     