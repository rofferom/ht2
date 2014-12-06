###############################################################################
## @file shared.mk
## @author Y.M. Morgan
## @date 2011/05/14
##
## Build a shared library.
###############################################################################

LOCAL_MODULE_SUFFIX := $(TARGET_SHARED_LIB_SUFFIX)
LOCAL_BUILDING_SHARED_LIBRARY := 1

$(call module-add,$(LOCAL_MODULE))
