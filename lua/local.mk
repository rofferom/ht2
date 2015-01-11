LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lua

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)/src

LOCAL_C_INCLUDES := \
	$(LOCAL_EXPORT_C_INCLUDES)

LOCAL_SRC_FILES := \
	src/lapi.c \
	src/lbitlib.c \
	src/lctype.c \
	src/ldo.c \
	src/lgc.c \
	src/llex.c \
	src/loadlib.c \
	src/loslib.c \
	src/lstring.c \
	src/ltablib.c \
	src/lzio.c \
	src/lauxlib.c \
	src/lcode.c \
	src/ldblib.c \
	src/ldump.c \
	src/linit.c \
	src/lmathlib.c \
	src/lobject.c \
	src/lparser.c \
	src/lstrlib.c \
	src/ltm.c \
	src/lundump.c \
	src/lbaselib.c \
	src/lcorolib.c \
	src/ldebug.c \
	src/lfunc.c \
	src/liolib.c \
	src/lmem.c \
	src/lopcodes.c \
	src/lstate.c \
	src/ltable.c \
	src/lua.c \
	src/lvm.c \

include $(BUILD_SHARED_LIBRARY)
