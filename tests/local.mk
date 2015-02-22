LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-test-logs

LOCAL_CFLAGS := -std=c++11

LOCAL_SRC_FILES := \
	ht-common/test-logs.cpp \

LOCAL_SHARED_LIBRARIES := \
	ht-common \

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-test-luabinder

LOCAL_CFLAGS := -std=c++11

LOCAL_SRC_FILES := \
	ht-common/test-luabinder.cpp \

LOCAL_SHARED_LIBRARIES := \
	ht-common \
	ht-lua \
	ht-legacy-lua \
	lua \

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-test

LOCAL_CFLAGS := -std=c++11

LOCAL_SRC_FILES := \
	ht-common/test-charsetconverter.cpp \
	ht-common/test-table.cpp \
	ht-common/test-tableparser.cpp \
	ht-common/test-text.cpp \
	ht-common/test-textfile.cpp \
	\
	ht-legacy/test-legacy-table.cpp \
	ht-legacy/test-legacy-text.cpp \
	ht-legacy/test-legacy-tokenizer.cpp \
	\
	libstr32/test-libstr32.cpp \
	\
	test-main.cpp \

LOCAL_COPY_FILES := \
	res/ht-common/test-table-emptykey.txt:test-res/ht-common/test-table-emptykey.txt \
	res/ht-common/test-table-emptyvalue.txt:test-res/ht-common/test-table-emptyvalue.txt \
	res/ht-common/test-table-invalidkeychar.txt:test-res/ht-common/test-table-invalidkeychar.txt \
	res/ht-common/test-table-noseparator.txt:test-res/ht-common/test-table-noseparator.txt \
	res/ht-common/test-table.txt:test-res/ht-common/test-table.txt \
	res/ht-common/test-textfile-utf32.txt:test-res/ht-common/test-textfile-utf32.txt \
	res/ht-common/test-textfile-utf8.txt:test-res/ht-common/test-textfile-utf8.txt \
	\
	res/ht-legacy/test-legacy-table.txt:test-res/ht-legacy/test-legacy-table.txt \
	res/ht-legacy/test-legacytext.txt:test-res/ht-legacy/test-legacytext.txt \

LOCAL_STATIC_LIBRARIES := \
	gtest

LOCAL_SHARED_LIBRARIES := \
	ht-common \
	ht-legacy \
	libstr32 \

include $(BUILD_EXECUTABLE)
