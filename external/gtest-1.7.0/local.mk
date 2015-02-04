LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gtest

LOCAL_C_INCLUDES := \
	-I $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
	src/gtest.cc \
	src/gtest-death-test.cc \
	src/gtest-filepath.cc \
	src/gtest-port.cc \
	src/gtest-printers.cc \
	src/gtest-test-part.cc \
	src/gtest-typed-test.cc \
	src/gtest_main.cc

LOCAL_CFLAGS := \
	-DGTEST_HAS_PTHREAD=1 \
	-DGTEST_OS_LINUX=1 \
	-Wno-undef \
	-Wno-missing-field-initializers \
	-Wno-ctor-dtor-privacy \
	-Wno-sign-compare \

LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)

ifeq ("$(TARGET_OS)","LINUX")
	LOCAL_EXPORT_LDLIBS := -lpthread
endif

include $(BUILD_STATIC_LIBRARY)
