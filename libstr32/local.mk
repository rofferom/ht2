LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libstr32

LOCAL_CFLAGS := -std=c11

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)/include 

LOCAL_C_INCLUDES := \
	$(LOCAL_EXPORT_C_INCLUDES)

LOCAL_SRC_FILES := \
	src/string32.c \
	src/vfprintf32.c \

include $(BUILD_SHARED_LIBRARY)
