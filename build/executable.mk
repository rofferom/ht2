###############################################################################
## @file executable.mk
## @author Y.M. Morgan
## @date 2011/05/14
##
## Build an executable.
###############################################################################

LOCAL_MODULE_SUFFIX := $(TARGET_EXE_SUFFIX)
LOCAL_BUILDING_EXECUTABLE := 1

$(call module-add,$(LOCAL_MODULE))
