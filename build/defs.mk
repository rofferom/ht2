###############################################################################
## @file defs.mk
## @author Y.M. Morgan
## @date 2011/05/14
##
## This file contains macros used by other makefiles.
###############################################################################

###############################################################################
## Some useful macros.
###############################################################################
empty :=
space := $(empty) $(empty)
space4 := $(space)$(space)$(space)$(space)
true := T
false :=

# Return negation of argument.
# $1 : input boolean argument.
not = $(if $1,$(false),$(true))

# Return the first element of a list.
# $1 : input list.
first = $(firstword $1)

# Return the list with the first element removed.
# $1 : input list.
rest = $(wordlist 2,$(words $1),$1)

# Compare 2 strings for equality.
# $1 : first string.
# $2 : second string.
streq = $(if $(filter-out xx,x$(subst $1,,$2)$(subst $2,,$1)x),$(false),$(true))

# Compare 2 strings for inequality.
# $1 : first string.
# $2 : second string.
strneq = $(call not,$(call streq,$1,$2))

# Check that a version is at least the one given.
# $1 : version.
# $2 : minimum version.
#check-version = $(streq,0,$$(shell expr $1 \>= $2))
check-version = $(call strneq,0,$(shell expr $1 \>= $2))

###############################################################################
## Modules database.
## For each module 'mod', __modules.mod.<field> is used to store
## module-specific information.
###############################################################################
__modules := $(empty)

###############################################################################
## Figure out where we are.
###############################################################################
my-dir = $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST),$(MAKEFILE_LIST))))

###############################################################################
## Get full path.
###############################################################################

ifeq ("$(HOST_OS)","MINGW32")
  fullpath = $(realpath $(1))
else
  fullpath = $(shell readlink -m -n $(1))
endif

###############################################################################
## Clear a list of variables.
###############################################################################
clear-vars = $(foreach __varname,$1,$(eval $(__varname) := $(empty)))

###############################################################################
## List of LOCAL_XXX variables that can be set by makefiles.
###############################################################################
modules-LOCALS :=

# Path of the root of module
modules-LOCALS += PATH

# Name of what's supposed to be generated
modules-LOCALS += MODULE

# Source files to compile
# All files are relative to LOCAL_PATH
modules-LOCALS += SRC_FILES

# Files to copy verbatim
modules-LOCALS += COPY_FILES

# Static libraries that you want to include in your module
# Names of modules in the build system, without path/prefix/suffix
modules-LOCALS += STATIC_LIBRARIES

# Static libraries that you want to include as a whole in your module
# To generate a .so for ex
# Names of modules in the build system, without path/prefix/suffix
modules-LOCALS += WHOLE_STATIC_LIBRARIES

# Libraries you directly link against
# Names of modules in the build system, without path/prefix/suffix
modules-LOCALS += SHARED_LIBRARIES

# External libraries (not built directly by the build system rules)
# Used as dependencies to trigger indirect build.
modules-LOCALS += EXTERNAL_LIBRARIES

# Additional include directories to pass into the C/C++ compilers
# Format : -I<fullpath>
modules-LOCALS += C_INCLUDES

# Additional flags to pass into the C or C++ compiler
modules-LOCALS += CFLAGS

# Additional flags to pass into only the C++ compiler
modules-LOCALS += CPPFLAGS

# Additional flags to pass into the static library generator
modules-LOCALS += ARFLAGS

# Additional flags to pass into the resource compiler (Windows only)
modules-LOCALS += RCFLAGS

# Additional flags to pass into the linker
modules-LOCALS += LDFLAGS

# Additional libraries to pass into the linker
# Format : -l<name>
modules-LOCALS += LDLIBS

# Resource list file
modules-LOCALS += RESLIST

# Xrc file
modules-LOCALS += XRC

# Additionnal dependencies for rc files
modules-LOCALS += RC_DEPS

# Precompiled file
# Relative to LOCAL_PATH
modules-LOCALS += PRECOMPILED_FILE

# Exported stuff (will be added in modules depending on this one)
modules-LOCALS += EXPORT_C_INCLUDES
modules-LOCALS += EXPORT_CFLAGS
modules-LOCALS += EXPORT_CPPFLAGS
modules-LOCALS += EXPORT_LDLIBS

# Other variables used internally
modules-LOCALS += BUILT_MODULE
modules-LOCALS += INSTALLED_MODULE
modules-LOCALS += INTERMEDIATE_TARGETS
modules-LOCALS += MODULE_SUFFIX
modules-LOCALS += BUILDING_STATIC_LIBRARY
modules-LOCALS += BUILDING_SHARED_LIBRARY
modules-LOCALS += BUILDING_EXECUTABLE
modules-LOCALS += BUILDING_EXTERNAL

# the list of managed fields per module
modules-fields := \
	depends \
	$(modules-LOCALS)

###############################################################################
## Dump all module information. Only use this for debugging.
###############################################################################
modules-dump-database = \
	$(info Modules: $(__modules)) \
	$(foreach __mod,$(__modules), \
		$(info $(space4)$(__mod):) \
		$(foreach __field,$(modules-fields), \
			$(eval __fieldval := $(strip $(__modules.$(__mod).$(__field)))) \
			$(if $(__fieldval), \
				$(if $(filter 1,$(words $(__fieldval))), \
					$(info $(space4)$(space4)$(__field): $(__fieldval)), \
					$(info $(space4)$(space4)$(__field): ) \
					$(foreach __fielditem,$(__fieldval), \
						$(info $(space4)$(space4)$(space4)$(__fielditem)) \
					) \
				) \
			) \
		) \
	) \
	$(info --- end of modules list)

###############################################################################
## Add a module in the build system and save its LOCAL_xxx variables.
## $1 : name of module to add. All LOCAL_xxx variables will be saved in
## module database.
###############################################################################
module-add = \
	$(eval __modules += $1) \
	$(foreach __local,$(modules-LOCALS), \
		$(eval __modules.$1.$(__local) := $(LOCAL_$(__local))) \
	)

###############################################################################
## Restore the recorded LOCAL_XXX definitions for a given module. Called
## for each module once they have all been registered and their dependencies
## have been computed to actually define rules.
## $1 : name of module to restore.
###############################################################################
module-restore-locals = \
	$(foreach __local,$(modules-LOCALS), \
		$(eval LOCAL_$(__local) := $(__modules.$1.$(__local))) \
	)

###############################################################################
## Used to recompute all dependencies once all module information has been
## recorded.
###############################################################################

# Compute dependencies of all modules
modules-compute-dependencies = \
	$(foreach __module,$(__modules), \
		$(eval __modules.$(__module).depends := $(empty)) \
		$(call __module-compute-depends,$(__module)) \
	)

# Compute dependencies of a single module
# $1 : module name.
__module-compute-depends = \
	$(call __module-add-depends,$1,$(__modules.$1.STATIC_LIBRARIES)) \
	$(call __module-add-depends,$1,$(__modules.$1.WHOLE_STATIC_LIBRARIES)) \
	$(call __module-add-depends,$1,$(__modules.$1.SHARED_LIBRARIES)) \
	$(call __module-add-depends,$1,$(__modules.$1.EXTERNAL_LIBRARIES))

# Add dependencies to a module
# $1 : module name.
# $2 : list of modules to add in dependency list.
__module-add-depends = \
	$(eval __modules.$1.depends += $(filter-out $(__modules.$1.depends),$2))

###############################################################################
## Automatic extraction from dependencies of a module.
###############################################################################

# Return the recorded value of LOCAL_EXPORT_$2, if any, for module $1.
# $1 : module name.
# $2 : export variable name without LOCAL_EXPORT_ prefix (e.g. 'CFLAGS').
module-get-export = $(__modules.$1.EXPORT_$2)

# Return the recorded value of LOCAL_EXPORT_$2, if any, for modules listed in $1.
# $1 : list of module names.
# $2 : export variable name without LOCAL_EXPORT_ prefix (e.g. 'CFLAGS').
module-get-listed-export = \
	$(strip $(foreach __mod,$1, \
		$(call module-get-export,$(__mod),$2) \
	))

# Return list all the <local-type> modules $1 depends on transitively.
# $1 : list of module names.
# $2 : local module type (e.g. SHARED_LIBRARIES).
module-get-depends = $(strip $(call __modules-get-closure,$1,$2))

# Return list of all the modules $1 depends on transitively.
# $1: list of module names.
module-get-all-dependencies = \
	$(strip $(call __modules-get-closure,$1,depends))

# Recursively get dependency of a modules
__modules-get-closure = \
	$(eval __closure_deps := $(empty)) \
	$(eval __closure_wq := $(strip $1)) \
	$(eval __closure_field := $(strip $2)) \
	$(if $(__closure_wq), $(call __modules-closure)) \
	$(strip $(__closure_deps))

# Used internally by modules-get-all-dependencies. Note the tricky use of
# conditional recursion to work around the fact that the GNU Make language does
# not have any conditional looping construct like 'while'.
__modules-closure = \
	$(eval __closure_mod := $(call first,$(__closure_wq))) \
	$(eval __closure_wq  := $(call rest,$(__closure_wq))) \
	$(eval __closure_val := $(__modules.$(__closure_mod).$(__closure_field))) \
	$(eval __closure_new := $(filter-out $(__closure_deps),$(__closure_val))) \
	$(eval __closure_deps += $(__closure_new)) \
	$(eval __closure_wq  := $(strip $(__closure_wq) $(__closure_new))) \
	$(if $(__closure_wq),$(call __modules-closure))

###############################################################################
## Commands to generate a precompiled file.
###############################################################################

define transform-h-to-gch
@mkdir -p $(dir $@)
@echo "Precompile: $(PRIVATE_MODULE) <= $<"
$(Q)$(CCACHE) $(TARGET_CXX) \
	$(TARGET_GLOBAL_C_INCLUDES) $(PRIVATE_C_INCLUDES) \
	$(TARGET_GLOBAL_CFLAGS) $(TARGET_GLOBAL_CPPFLAGS) $(CPPFLAGS_WARNINGS) \
	$(PRIVATE_CFLAGS) $(PRIVATE_CPPFLAGS) \
	$(TARGET_PCH_FLAGS) -MMD -MP -o $@ $<
endef

###############################################################################
## Commands to compile a C++ file.
###############################################################################

define transform-cpp-to-o
@mkdir -p $(dir $@)
@echo "C++: $(PRIVATE_MODULE) <= $<"
$(Q)$(CCACHE) $(TARGET_CXX) \
	$(TARGET_GLOBAL_C_INCLUDES) $(PRIVATE_C_INCLUDES) \
	$(TARGET_GLOBAL_CFLAGS) $(TARGET_GLOBAL_CPPFLAGS) $(CPPFLAGS_WARNINGS) \
	$(PRIVATE_CFLAGS) $(PRIVATE_CPPFLAGS) \
	-c -MMD -MP -o $@ $<
endef

###############################################################################
## Commands to compile a C file.
###############################################################################

define transform-c-to-o
@echo "C: $(PRIVATE_MODULE) <= $<"
@mkdir -p $(dir $@)
$(Q)$(CCACHE) $(TARGET_CC) \
	$(TARGET_GLOBAL_C_INCLUDES) $(PRIVATE_C_INCLUDES) \
	$(TARGET_GLOBAL_CFLAGS) $(CFLAGS_WARNINGS) \
	$(PRIVATE_CFLAGS) \
	-c -MMD -MP -o $@ $<
endef

###############################################################################
## Commands for compiling rc files.
###############################################################################
define transform-rc-to-o
@echo "RC: $(PRIVATE_MODULE) <= $<"
@mkdir -p $(dir $@)
@rm -f $@
$(Q)$(TARGET_WINDRES) \
	--use-temp-file -i$< -o$@ -I$(dir $<) \
	$(filter -D%, $(TARGET_GLOBAL_CFLAGS)) \
	$(filter -D%, $(PRIVATE_CFLAGS)) \
	$(TARGET_GLOBAL_RCFLAGS) $(PRIVATE_RCFLAGS)
endef

###############################################################################
## Commands for running ar.
###############################################################################

# Explicitly delete the archive first so that ar doesn't
# try to add to an existing archive.
define transform-o-to-static-lib
@mkdir -p $(dir $@)
@echo "StaticLib: $(PRIVATE_MODULE) ($@)"
@rm -f $@
$(Q)$(TARGET_AR) $(TARGET_GLOBAL_ARFLAGS) $(PRIVATE_ARFLAGS) $@ $^
endef

###############################################################################
## Commands to link a shared library.
###############################################################################

ifneq ("$(TARGET_OS)","MACOSX")

define transform-o-to-shared-lib
@mkdir -p $(dir $@)
@echo "SharedLib: $(PRIVATE_MODULE) ($@)"
$(Q)$(TARGET_CXX) \
	$(TARGET_GLOBAL_LDFLAGS) \
	$(TARGET_GLOBAL_LDFLAGS_RPATH) \
	-Wl,-Map -Wl,$(basename $@).map \
	-shared \
	-Wl,-soname -Wl,$(notdir $@) \
	-Wl,--unresolved-symbols=ignore-in-shared-libs \
	$(PRIVATE_LDFLAGS) \
	$(PRIVATE_ALL_OBJECTS) \
	-Wl,--whole-archive \
	$(PRIVATE_ALL_WHOLE_STATIC_LIBRARIES) \
	-Wl,--no-whole-archive \
	-Wl,--as-needed \
	$(PRIVATE_ALL_STATIC_LIBRARIES) \
	$(PRIVATE_ALL_SHARED_LIBRARIES) \
	-o $@ \
	$(PRIVATE_LDLIBS) \
	$(TARGET_GLOBAL_LDLIBS)
endef

else

define transform-o-to-shared-lib
@mkdir -p $(dir $@)
@echo "SharedLib: $(PRIVATE_MODULE) ($@)"
$(Q)$(TARGET_CXX) \
	$(TARGET_GLOBAL_LDFLAGS) \
	-shared \
	-install_name @rpath/$(notdir $@) \
	$(PRIVATE_LDFLAGS) \
	$(PRIVATE_ALL_OBJECTS) \
	$(PRIVATE_ALL_STATIC_LIBRARIES) \
	$(PRIVATE_ALL_SHARED_LIBRARIES) \
	-o $@ \
	$(PRIVATE_LDLIBS) \
	$(TARGET_GLOBAL_LDLIBS)
endef

endif

########################################################################
## Commands to link an executable.
###############################################################################

ifneq ("$(TARGET_OS)","MACOSX")

define transform-o-to-executable
@mkdir -p $(dir $@)
@echo "Executable: $(PRIVATE_MODULE) ($@)"
$(Q)$(TARGET_CXX) \
	$(TARGET_GLOBAL_LDFLAGS) \
	$(TARGET_GLOBAL_LDFLAGS_RPATH) \
	-Wl,-Map -Wl,$(basename $@).map \
	$(PRIVATE_LDFLAGS) \
	$(PRIVATE_ALL_OBJECTS) \
	-Wl,--whole-archive \
	$(PRIVATE_ALL_WHOLE_STATIC_LIBRARIES) \
	-Wl,--no-whole-archive \
	-Wl,--as-needed \
	$(PRIVATE_ALL_STATIC_LIBRARIES) \
	$(PRIVATE_ALL_SHARED_LIBRARIES) \
	-o $@ \
	$(PRIVATE_LDLIBS) \
	$(TARGET_GLOBAL_LDLIBS)
endef

else

define transform-o-to-executable
@mkdir -p $(dir $@)
@echo "Executable: $(PRIVATE_MODULE) ($@)"
$(Q)$(TARGET_CXX) \
	$(TARGET_GLOBAL_LDFLAGS) \
	-Wl,-rpath,. \
	$(PRIVATE_LDFLAGS) \
	$(PRIVATE_ALL_OBJECTS) \
	$(PRIVATE_ALL_WHOLE_STATIC_LIBRARIES) \
	$(PRIVATE_ALL_STATIC_LIBRARIES) \
	$(PRIVATE_ALL_SHARED_LIBRARIES) \
	-o $@ \
	$(PRIVATE_LDLIBS) \
	$(TARGET_GLOBAL_LDLIBS)
endef

endif

###############################################################################
## Commands to generate resource list
## Use host cc, not target cc.
###############################################################################

# $(1) : ResListMaker full path
# $(2) : source directory
# $(3) : destination directory
define transform-reslist
@mkdir -p $(dir $@)
@echo "ResList: $(PRIVATE_MODULE) ($@)"
$(Q)$(1) $(2)/ResList.txt $(3)/ResList.h $(3)/ResList.c $(2)
$(Q)date | cat > $@
endef

# $(1) : source directory
# $(2) : destination directory
define generate-reslistmaker
@mkdir -p $(dir $@)
@echo "ResListMaker: $(PRIVATE_MODULE) ($@)"
$(Q)$(HOST_CC) -O2 -g0 -c -o $(2)/ResListMaker.o $(1)/ResListMaker.c
$(Q)$(HOST_CC) -o $@ $(2)/ResListMaker.o
endef

###############################################################################
## Commands for copying files.
###############################################################################

# Copy a single file from one place to another, preserving permissions and
# overwriting any existing file.
define copy-file-to-target
@mkdir -p $(dir $@)
$(Q)cp -fp $< $@
endef

# Define a rule to copy a file. For use via $(eval).
# $(1) : source file
# $(2) : destination file
define copy-one-file
$(2): $(1)
	@echo "Copy: $$@"
	$$(copy-file-to-target)
endef

