#===================服务端=============================
LOCAL_PATH:= $(call my-dir)
#LOCAL_CFLAGS_ALL :=-I. -I$(LOCAL_PATH)/..
 
include $(CLEAR_VARS)
 
LOCAL_SRC_FILES:= \
	main.cpp \
	testbinder.cpp \
	../interface/Itestbinder.cpp \
	../interface/Icallback.cpp
 
LOCAL_SHARED_LIBRARIES := \
        libui libcutils libutils libbinder libsonivox libicuuc libexpat \
	libdl
 
 
LOCAL_MODULE:= server
LOCAL_MODULE_TAGS := optional

#===================================================

#===================客户端=============================
LOCAL_PATH:= $(call my-dir)
#LOCAL_CFLAGS_ALL :=-I. -I$(LOCAL_PATH)/..

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	client.cpp \
	main.cpp \
	../interface/Itestbinder.cpp \
	callback.cpp \
	../interface/Icallback.cpp \

LOCAL_SHARED_LIBRARIES := \
        libui libcutils libutils libbinder libsonivox libicuuc libexpat \
	libdl


LOCAL_MODULE:= client
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

#===================================================