LOCAL_PATH:=$(call my-dir)

#====================================================================
#native audio library
include $(CLEAR_VARS)

LOCAL_SRC_FILES := native-audio-jni.c

LOCAL_MODULE := libnativeaudio-jni

LOCAL_C_INCLUDES := $(LOCAL_PATH)/android_clip.h \
					$(LOCAL_PATH)/hello_clip.h \

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libutils \
        libcutils \
		libwilhelm \
		libOpenSLES \
		libandroid \

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

#====================================================================


#====================================================================
#CanServer
include $(CLEAR_VARS)

LOCAL_SRC_FILES := main.cpp

LOCAL_C_INCLUDES += frameworks/wilhelm/include/SLES

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libutils \
        libcutils \
        libandroid_runtime \
		libwilhelm \
		libOpenSLES \

LOCAL_STRIP_MODULE := keep_symbols

LOCAL_MODULE := AudioTrackCallback

LOCAL_MODULE_TAGS := optional

LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += \
    -Wno-unused-parameter \
    -Wno-non-virtual-dtor \
    -Wno-maybe-uninitialized \
    -Wno-parentheses \
    -fno-rtti \
    -Wno-format \
    -Wno-delete-incomplete \
    -Wno-c++1z-extensions

LOCAL_CPPFLAGS += -Wno-conversion-null

include $(BUILD_EXECUTABLE)

#====================================================================
include $(call all-makefiles-under,$(LOCAL_PATH))