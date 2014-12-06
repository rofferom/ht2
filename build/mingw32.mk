###############################################################################
## @file mingw32.mk
## @author Y.M. Morgan
## @date 2012/06/09
##
## Mingw32 specific rules.
###############################################################################

###############################################################################
# Additionnal rules for mingw32
###############################################################################
ifeq ("$(TARGET_OS)","MINGW32")

ALL_MODULES += mingw32-dll

MINGW32_DLL_NAME := mingwm10.dll
ifeq ("$(HOST_OS)","MINGW32")
  MINGW32_DLL_DIR := /usr/bin
else
  MINGW32_DLL_DIR := $(call fullpath,$(dir $(TARGET_CC_PATH)))
endif

.PHONY: clean-mingw32-dll
clean-mingw32-dll:
	@echo "Clean: mingw32-dll"
	$(Q)rm -f $(TARGET_OUT_FINAL)/$(MINGW32_DLL_NAME)

.PHONY: mingw32-dll
mingw32-dll:
	@echo "Copy: mingw32-dll"
	@mkdir -p $(TARGET_OUT_FINAL)
	$(Q)cp -fp $(MINGW32_DLL_DIR)/$(MINGW32_DLL_NAME) $(TARGET_OUT_FINAL)

.PHONY: install-mingw32-dll
install-mingw32-dll:
	@echo "Install: mingw32-dll"
	$(Q)install -d $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	$(Q)install $(TARGET_OUT_FINAL)/$(MINGW32_DLL_NAME) $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)

endif

