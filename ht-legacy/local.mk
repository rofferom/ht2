LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ht-legacy

LOCAL_CPPFLAGS := -std=c++11

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)/include 

LOCAL_C_INCLUDES := \
	$(LOCAL_EXPORT_C_INCLUDES)  

LOCAL_SRC_FILES := \
	src/TableLoader.cpp \
	src/Text.cpp \
	src/TextTokenizer.cpp \

LOCAL_SHARED_LIBRARIES := \
	ht-common \
	libstr32 \

include $(BUILD_SHARED_LIBRARY)
