###############################################################################
## @file warnings.mk
## @author Y.M. Morgan
## @date 2012/06/09
##
## Setup warning flags.
###############################################################################

COMMON_FLAGS_WARNINGS :=
CFLAGS_WARNINGS :=
CPPFLAGS_WARNINGS :=

# show option associated with warning (clang or gcc >= 4.0.0)
ifeq ("$(USE_CLANG)","1")
  COMMON_FLAGS_WARNINGS += -fdiagnostics-show-option
else ifneq ("$(call check-version,$(TARGET_CC_VERSION),4.0.0)","")
  COMMON_FLAGS_WARNINGS += -fdiagnostics-show-option
endif

###############################################################################
## Common flags.
###############################################################################

COMMON_FLAGS_WARNINGS += -Wall
COMMON_FLAGS_WARNINGS += -Wextra
COMMON_FLAGS_WARNINGS += -Wno-unused -Wno-unused-parameter -Wunused-value -Wunused-variable -Wunused-label
COMMON_FLAGS_WARNINGS += -Wshadow
COMMON_FLAGS_WARNINGS += -Wswitch-default
COMMON_FLAGS_WARNINGS += -Wwrite-strings
COMMON_FLAGS_WARNINGS += -Wundef
COMMON_FLAGS_WARNINGS += -Wpointer-arith
COMMON_FLAGS_WARNINGS += -Wformat-security
COMMON_FLAGS_WARNINGS += -Winit-self
COMMON_FLAGS_WARNINGS += -Wformat-nonliteral

# Too many false positives with clang compiler
ifneq ("$(USE_CLANG)","1")
  COMMON_FLAGS_WARNINGS += -Wcast-align
endif

# clang or gcc >= 4.5.0 (too many false positives with previous versions)
ifeq ("$(USE_CLANG)","1")
  COMMON_FLAGS_WARNINGS += -Wunreachable-code
else ifneq ("$(call check-version,$(TARGET_CC_VERSION),4.5.0)","")
  COMMON_FLAGS_WARNINGS += -Wunreachable-code
endif

# gcc >= 4.5.2
ifneq ("$(USE_CLANG)","1")
ifneq ("$(call check-version,$(TARGET_CC_VERSION),4.5.2)","")
  COMMON_FLAGS_WARNINGS += -Wlogical-op
endif
endif

###############################################################################
## Specific flags.
###############################################################################

# C specific
CFLAGS_WARNINGS += -Wmissing-declarations
CFLAGS_WARNINGS += -Wmissing-prototypes
ifneq ("$(USE_CLANG)","1")
  CFLAGS_WARNINGS += -Wjump-misses-init
endif

# c++ specific
CPPFLAGS_WARNINGS += -Wctor-dtor-privacy
CPPFLAGS_WARNINGS += -Wnon-virtual-dtor
CPPFLAGS_WARNINGS += -Wreorder
CPPFLAGS_WARNINGS += -Woverloaded-virtual

###############################################################################
## Extra warnings.
###############################################################################

ifeq ("$(W)","1")

COMMON_FLAGS_WARNINGS += -Wconversion
COMMON_FLAGS_WARNINGS += -Wswitch-enum
COMMON_FLAGS_WARNINGS += -Wcast-qual

# gcc >= 4.4.0
ifneq ("$(USE_CLANG)","1")
ifneq ("$(call check-version,$(TARGET_CC_VERSION),4.4.0)","")
  COMMON_FLAGS_WARNINGS += -Wframe-larger-than=1024
endif
endif

endif

###############################################################################
## Add common flags to specific flags.
###############################################################################

CFLAGS_WARNINGS += $(COMMON_FLAGS_WARNINGS)
CPPFLAGS_WARNINGS += $(COMMON_FLAGS_WARNINGS)

