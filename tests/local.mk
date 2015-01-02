LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-test-logs

LOCAL_CFLAGS := -std=c++11

LOCAL_SRC_FILES := \
	test-logs.cpp \

LOCAL_SHARED_LIBRARIES := \
	ht-common \

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-test

LOCAL_CFLAGS := -std=c++11

LOCAL_SRC_FILES := \
	test-charsetconverter.cpp \
	test-legacy-table.cpp \
	test-legacy-text.cpp \
	test-legacy-tokenizer.cpp \
	test-libstr32.cpp \
	test-main.cpp \
	test-table.cpp \
	test-tableparser.cpp \
	test-text.cpp \
	test-textfile.cpp \

LOCAL_STATIC_LIBRARIES := \
	gtest

LOCAL_SHARED_LIBRARIES := \
	ht-common \
	ht-legacy \
	libstr32 \

include $(BUILD_EXECUTABLE)
