LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-common

LOCAL_CPPFLAGS := -std=c++11

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)/include 

LOCAL_C_INCLUDES := \
	$(LOCAL_EXPORT_C_INCLUDES)  

LOCAL_SRC_FILES := \
	src/Buffer.cpp \
	src/CharsetConverter.cpp \
	src/Log.cpp \
	src/Table.cpp \
	src/TableParser.cpp \
	src/Text.cpp \
	src/TextFile.cpp \
	src/Utils.cpp \
	src/crypto/int64.c \
	src/crypto/md5.c \
	src/crypto/sha1.c \
	src/crypto/sha256.c \
	src/crypto/sha512.c \

LOCAL_SHARED_LIBRARIES := \
	libstr32 \

include $(BUILD_SHARED_LIBRARY)
