###############################################################################
## @file config.mk
## @author Y.M. Morgan
## @date 2011/05/14
###############################################################################

###############################################################################
## Make sure that there are no spaces in the absolute path; the build system
## can't deal with them.
###############################################################################

ifneq ("$(words $(shell pwd))","1")
$(error Top directory contains space characters)
endif

###############################################################################
## Host/Target OS.
###############################################################################

# Host OS
ifneq ("$(shell echo $$OSTYPE | grep msys)","")
  HOST_OS := MINGW32
  HOST_EXE_SUFFIX := .exe
  HOST_SHARED_LIB_SUFFIX := .dll
else
  HOST_OS := LINUX
  HOST_EXE_SUFFIX :=
  HOST_SHARED_LIB_SUFFIX := .so
endif

# Target OS : default to HOST_OS unless set
ifndef TARGET_OS
  TARGET_OS := $(HOST_OS)
endif

# Exe/dll suffix under mingw
TARGET_STATIC_LIB_SUFFIX := .a
ifeq ("$(TARGET_OS)","MINGW32")
  TARGET_EXE_SUFFIX := .exe
  TARGET_SHARED_LIB_SUFFIX := .dll
else
  TARGET_EXE_SUFFIX :=
  TARGET_SHARED_LIB_SUFFIX := .so
endif

ifeq ("$(TARGET_OS)","MINGW32")
  # may be overridden in make command line
  STATIC := 1
  TARGET_GLOBAL_CFLAGS += -D__MINGW_FEATURES__=0
  TARGET_GLOBAL_LDFLAGS += -Wl,--enable-auto-import
  ifeq ("$(STATIC)","1")
    TARGET_GLOBAL_LDFLAGS += -Wl,-Bstatic
  endif
endif

# To be able to use ccache with pre-complied headers, some env variables are required
CCACHE := 
ifeq ("$(USE_CCACHE)","1")
  ifneq ("$(shell which ccache)","")
    CCACHE := CCACHE_SLOPPINESS=time_macros ccache
    TARGET_GLOBAL_CFLAGS += -fpch-preprocess
  endif
endif

# Pre-compiled header generation flag
ifneq ("$(USE_CLANG)","1")
  TARGET_PCH_FLAGS := -c
else
  TARGET_PCH_FLAGS := -x c++-header
endif

# Architecture
ifndef TARGET_ARCH
  ifneq ("$(shell $(TARGET_CC) -dumpmachine | grep 64)","")
    TARGET_ARCH := X64
  else
    TARGET_ARCH := X86
  endif
endif

# Update flags based on architecture
# 64-bit requires -fPIC to build shared libraries
ifeq ("$(TARGET_ARCH)","X64")
  TARGET_GLOBAL_CFLAGS += -m64 -fPIC
else
  TARGET_GLOBAL_CFLAGS += -m32
endif

# Update flags based on DEBUG.
ifeq ("$(DEBUG)","0")
  TARGET_GLOBAL_CFLAGS += -O2 -g -DNDEBUG
else
  TARGET_GLOBAL_CFLAGS += -O0 -g -DDEBUG -D_DEBUG
endif

###############################################################################
## Setup directory suffix.
###############################################################################

DIR_SUFFIX :=

ifeq ("$(USE_WX_VERSION)","2.9")
  DIR_SUFFIX := $(DIR_SUFFIX)_wx29
endif

ifeq ("$(TARGET_OS)","MINGW32")
  DIR_SUFFIX := $(DIR_SUFFIX)_mingw32
endif

ifeq ("$(USE_CLANG)","1")
  DIR_SUFFIX := $(DIR_SUFFIX)_clang
else
  DIR_SUFFIX := $(DIR_SUFFIX)_gcc
endif

ifeq ("$(TARGET_ARCH)","X64")
  DIR_SUFFIX := $(DIR_SUFFIX)_x64
endif

ifeq ("$(DEBUG)","0")
  DIR_SUFFIX := $(DIR_SUFFIX)_release
else
  DIR_SUFFIX := $(DIR_SUFFIX)_debug
endif

###############################################################################
## Setup output directories.
###############################################################################

TARGET_OUT_BUILD := $(TOP_DIR)/out/build$(DIR_SUFFIX)
TARGET_OUT_FINAL := $(TOP_DIR)/out/final$(DIR_SUFFIX)

###############################################################################
## Determine gcc path and version.
###############################################################################

TARGET_CC_PATH := $(shell which $(TARGET_CC))

ifneq ("$(USE_CLANG)","1")
TARGET_CC_VERSION := $(shell $(TARGET_CC) --version | head -1 | sed "s/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/")
else
TARGET_CC_VERSION := $(shell $(TARGET_CC) --version | head -1 | sed "s/.*\([0-9]\.[0-9]\).*/\1/")
endif
