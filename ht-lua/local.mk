LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-lua

LOCAL_CFLAGS := -fvisibility=hidden -fvisibility-inlines-hidden

LOCAL_CPPFLAGS := -std=c++11

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)/include

LOCAL_C_INCLUDES := \
	$(LOCAL_EXPORT_C_INCLUDES)

LOCAL_SRC_FILES := \
	src/LuaBuffer.cpp \
	src/LuaFile.cpp \
	src/LuaPackage.cpp \
	src/LuaPointer.cpp \
	src/LuaTable.cpp \
	src/LuaText.cpp \
	src/ht-lua.cpp \

LOCAL_SHARED_LIBRARIES := \
	ht-common \
	lua \

include $(BUILD_SHARED_LIBRARY)
