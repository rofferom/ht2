###############################################################################
## @file main.mk
## @author Y.M. Morgan
## @date 2011/05/14
##
## Main Makefile.
###############################################################################

# Make sure SHELL is correctly set
SHELL := /bin/bash

# Turns off suffix rules built into make
.SUFFIXES:

# Turns off the RCS / SCCS implicit rules of GNU Make
% : RCS/%,v
% : RCS/%
% : %,v
% : s.%
% : SCCS/s.%

# If a rule fails, delete $@.
.DELETE_ON_ERROR:

# Installation variables
DESTDIR :=
PREFIX := /usr/local

# Overridable settings
V := 0
W := 0
DEBUG := 0
STATIC := 0
USE_CLANG := 0
USE_CCACHE := 0

# Directories
TOP_DIR := .
BUILD_SYSTEM := $(TOP_DIR)/build

# Detect OS
include $(BUILD_SYSTEM)/os.mk

# Tools
ifneq ("$(USE_CLANG)","1")

TARGET_CROSS ?=
TARGET_CC ?= $(TARGET_CROSS)gcc
TARGET_CXX ?= $(TARGET_CROSS)g++
TARGET_AR ?= $(TARGET_CROSS)ar
TARGET_LD ?= $(TARGET_CROSS)ld
TARGET_STRIP ?= $(TARGET_CROSS)strip

ifneq ("$(TARGET_OS)","MACOSX")
    TARGET_STRIP_SHARED ?= $(TARGET_STRIP)
else
    TARGET_STRIP_SHARED ?= $(TARGET_STRIP) -x
endif

TARGET_WINDRES ?= $(TARGET_CROSS)windres

HOST_CC := gcc
HOST_CXX := g++
HOST_AR := ar
HOST_LD := ld
HOST_STRIP := strip

ifneq ("$(TARGET_OS)","MACOSX")
    HOST_STRIP_SHARED := $(HOST_STRIP)
else
    HOST_STRIP_SHARED := $(HOST_STRIP) -x
endif

else

# llvm-ar causes issues, so use ar
TARGET_CC := clang
TARGET_CXX := clang++
TARGET_AR := ar
TARGET_LD := llvm-ld
TARGET_STRIP := strip

ifneq ("$(TARGET_OS)","MACOSX")
    TARGET_STRIP_SHARED := $(TARGET_STRIP)
else
    TARGET_STRIP_SHARED := $(TARGET_STRIP) -x
endif

HOST_CC := $(TARGET_CC)
HOST_CXX := $(TARGET_CXX)
HOST_AR := $(TARGET_AR)
HOST_LD := $(TARGET_LD)
HOST_STRIP := $(TARGET_STRIP)
HOST_STRIP_SHARED := $(TARGET_STRIP_SHARED)

endif

# Quiet command if V is 0
ifeq ("$(V)","0")
  Q := @
endif

# This is the default target.  It must be the first declared target.
all:

# Global variables
TARGET_GLOBAL_C_INCLUDES :=
TARGET_GLOBAL_CFLAGS := -fno-exceptions -fstrict-aliasing
TARGET_GLOBAL_CPPFLAGS :=
TARGET_GLOBAL_RCFLAGS :=
TARGET_GLOBAL_ARFLAGS := rcs
TARGET_GLOBAL_LDFLAGS ?=
TARGET_GLOBAL_LDLIBS ?=
ALL_MODULES :=

# Some flags are not available on all systems
ifneq ("$(TARGET_OS)","MACOSX")
    TARGET_GLOBAL_LDFLAGS += -Wl,--gc-sections -Wl,--warn-common -Wl,--warn-constructors
endif

# To be able to load automatically .so libraries located in the same
# folder than the application, we need to add $ORIGIN to DT_RPATH
# '\' is to escape the '$' in the shell command
# '$$' is to escape the '$' in the makefile
ifneq ("$(TARGET_OS)","MACOSX")
    TARGET_GLOBAL_LDFLAGS_RPATH += -Wl,-rpath,\$$ORIGIN
endif

###############################################################################
###############################################################################

# Setup configuration
include $(BUILD_SYSTEM)/config.mk

# Setup macros definitions
include $(BUILD_SYSTEM)/defs.mk

# Setup warnings flags
include $(BUILD_SYSTEM)/warnings.mk

# Names of makefiles that can be included by "local.mk" Makefiles
CLEAR_VARS := $(BUILD_SYSTEM)/clearvars.mk
BUILD_STATIC_LIBRARY := $(BUILD_SYSTEM)/static.mk
BUILD_SHARED_LIBRARY := $(BUILD_SYSTEM)/shared.mk
BUILD_EXECUTABLE := $(BUILD_SYSTEM)/executable.mk
RULES := $(BUILD_SYSTEM)/rules.mk

###############################################################################
###############################################################################

# Get the list of all makefiles available and include them
makefiles += $(shell find $(TOP_DIR) -name local.mk)
include $(makefiles)

# Recompute all dependencies between modules
$(call modules-compute-dependencies)

# Now, really build the modules, the second pass allows to deal with exported values
$(foreach __module,$(__modules), \
    $(eval LOCAL_MODULE := $(__module)) \
    $(eval include $(BUILD_SYSTEM)/module.mk) \
)

###############################################################################
# Additionnal include
###############################################################################

# package generation
ifneq ("$(PACKAGE)","")
include $(BUILD_SYSTEM)/package.mk
endif

# mingw32 rules
ifeq ("$(TARGET_OS)","MINGW32")
include $(BUILD_SYSTEM)/mingw32.mk
endif

###############################################################################
# Main rules.
###############################################################################

.PHONY: all
all: $(foreach m,$(ALL_MODULES),$(m))

.PHONY: clean
clean: $(foreach m,$(ALL_MODULES),$(m)-clean)

.PHONY: distclean
distclean: clean

.PHONY: test
test:

.PHONY: install
install: $(foreach m,$(ALL_MODULES),install-$(m))


###############################################################################
# Display configuration.
###############################################################################

$(info ----------------------------------------------------------------------)
$(info + PACKAGE = $(PACKAGE))
$(info + VERSION = $(VERSION))
$(info + DEBUG = $(DEBUG))
$(info + STATIC = $(STATIC))
ifneq ("$(HOST_OS)","$(TARGET_OS)")
$(info + HOST_OS = $(HOST_OS))
endif
$(info + TARGET_OS = $(TARGET_OS))
$(info + TARGET_ARCH = $(TARGET_ARCH))
$(info + TARGET_OUT_BUILD = $(TARGET_OUT_BUILD))
$(info + TARGET_OUT_FINAL = $(TARGET_OUT_FINAL))
$(info + TARGET_CC_PATH = $(TARGET_CC_PATH))
$(info + TARGET_CC_VERSION = $(TARGET_CC_VERSION))
$(info ----------------------------------------------------------------------)
