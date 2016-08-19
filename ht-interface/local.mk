LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-interface

LOCAL_CFLAGS := -std=c++11

LOCAL_SRC_FILES := \
	ht-interface.cpp \

ifdef HT2_SUPPORT_HISTEDIT
LOCAL_CFLAGS += -DHT2_SUPPORT_HISTEDIT

LOCAL_LDFLAGS += \
	-ledit -ltermcap
endif

LOCAL_SHARED_LIBRARIES := \
	ht-common \
	ht-lua \
	ht-legacy-lua \
	lua \

include $(BUILD_EXECUTABLE)
