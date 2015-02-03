LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-legacy-lua

LOCAL_CFLAGS := -fvisibility=hidden -fvisibility-inlines-hidden

LOCAL_CPPFLAGS := -std=c++11

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)/include

LOCAL_C_INCLUDES := \
	$(LOCAL_EXPORT_C_INCLUDES)

LOCAL_SRC_FILES := \
	src/LuaTable.cpp \
	src/LuaText.cpp \

LOCAL_SHARED_LIBRARIES := \
	ht-legacy \
	ht-lua \
	lua \

include $(BUILD_SHARED_LIBRARY)
