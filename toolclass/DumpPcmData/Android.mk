
LOCAL_PATH := $(call my-dir)

#===================================================
include $(CLEAR_VARS)
LOCAL_MODULE := libaudiodatadump

LOCAL_SRC_FILES := DumpThread.cpp DumpAdapter.cpp

LOCAL_C_INCLUDES += DumpThread.h \
                    DumpAdapter.h \
                    CommonDefine.h \

LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_CFLAGS += -Werror -Wno-macro-redefined -Wunused-parameter

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

#===================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := dump.c

LOCAL_SHARED_LIBRARIES := liblog libcutils libtinyalsa libaudiodatadump

LOCAL_MODULE := audiodump

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -Werror

include $(BUILD_EXECUTABLE)

#=================================================