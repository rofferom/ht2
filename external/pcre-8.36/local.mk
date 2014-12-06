LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libpcre

LOCAL_CFLAGS := -DHAVE_CONFIG_H -fvisibility=hidden

LOCAL_EXPORT_C_INCLUDES := \
	-I $(LOCAL_PATH)

LOCAL_SRC_FILES := \
	pcre_byte_order.c \
	pcre_chartables.c \
	pcre_compile.c \
	pcre_config.c \
	pcre_dfa_exec.c \
	pcre_exec.c \
	pcre_fullinfo.c \
	pcre_get.c \
	pcre_globals.c \
	pcre_jit_compile.c \
	pcre_maketables.c \
	pcre_newline.c \
	pcre_ord2utf8.c \
	pcre_refcount.c \
	pcre_string_utils.c \
	pcre_study.c \
	pcre_tables.c \
	pcre_ucd.c \
	pcre_valid_utf8.c \
	pcre_version.c \
	pcre_xclass.c \
	pcre32_byte_order.c \
	pcre32_chartables.c \
	pcre32_compile.c \
	pcre32_config.c \
	pcre32_dfa_exec.c \
	pcre32_exec.c \
	pcre32_fullinfo.c \
	pcre32_get.c \
	pcre32_globals.c \
	pcre32_jit_compile.c \
	pcre32_maketables.c \
	pcre32_newline.c \
	pcre32_ord2utf32.c \
	pcre32_refcount.c \
	pcre32_string_utils.c \
	pcre32_study.c \
	pcre32_tables.c \
	pcre32_ucd.c \
	pcre32_valid_utf32.c \
	pcre32_version.c \
	pcre32_xclass.c \

include $(BUILD_SHARED_LIBRARY)
