LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-common

LOCAL_CFLAGS := -fvisibility=hidden -fvisibility-inlines-hidden

LOCAL_CPPFLAGS := -std=c++11

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)/include 

LOCAL_C_INCLUDES := \
	$(LOCAL_EXPORT_C_INCLUDES)  

LOCAL_SRC_FILES := \
	src/Buffer.cpp \
	src/CharsetConverter.cpp \
	src/File.cpp \
	src/Log.cpp \
	src/Pointer.cpp \
	src/Table.cpp \
	src/TableParser.cpp \
	src/Text.cpp \
	src/TextFile.cpp \
	src/Utils.cpp \

LOCAL_SHARED_LIBRARIES := \
	libstr32 \

include $(BUILD_SHARED_LIBRARY)
