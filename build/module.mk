###############################################################################
## @file module.mk
## @author Y.M. Morgan
## @date 2012/04/17
##
## Build a module.
###############################################################################

# Bring back all LOCAL_XXX variables defined by LOCAL_MODULE
$(call module-restore-locals,$(LOCAL_MODULE))

# Setup rules
ifneq ("$(LOCAL_BUILDING_EXTERNAL)", "1")
include $(RULES)
endif

# Additional rules
ifeq ("$(LOCAL_BUILDING_EXECUTABLE)", "1")

$(LOCAL_BUILT_MODULE): $(all_objects) $(all_libraries)
	$(transform-o-to-executable)

else ifeq ("$(LOCAL_BUILDING_STATIC_LIBRARY)", "1")

$(LOCAL_BUILT_MODULE): $(all_objects)
	$(transform-o-to-static-lib)

else ifeq ("$(LOCAL_BUILDING_SHARED_LIBRARY)", "1")

# Generate .dll.a file under mingw32
ifeq ("$(TARGET_OS)","MINGW32")
$(LOCAL_BUILT_MODULE)$(TARGET_STATIC_LIB_SUFFIX): $(LOCAL_BUILT_MODULE)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_LDFLAGS += \
	-Wl,--out-implib -Wl,$(LOCAL_BUILT_MODULE)$(TARGET_STATIC_LIB_SUFFIX)
endif

$(LOCAL_BUILT_MODULE): $(all_objects) $(all_libraries)
	$(transform-o-to-shared-lib)

endif
