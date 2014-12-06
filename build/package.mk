###############################################################################
## @file package.mk
## @author Y.M. Morgan
## @date 2012/06/09
##
## Packaging rules.
###############################################################################

###############################################################################
# debian package generation.
###############################################################################

.PHONY: deb
deb:
	@chmod +x debian/rules
	$(Q)fakeroot debian/rules binary

.PHONY: clean-deb
clean-deb:
	@chmod +x debian/rules
	$(Q)fakeroot debian/rules clean

###############################################################################
# source/binary pakage generation.
###############################################################################

# Generate archive with source files from git
SRCZIP_NAME := $(PACKAGE)-src-$(VERSION)
.PHONY: src-zip
src-zip:
	@rm -f $(SRCZIP_NAME).tar
	@rm -f $(SRCZIP_NAME).tar.bz2
	$(Q)git archive --format=tar --prefix=$(SRCZIP_NAME)/ \
		-o $(SRCZIP_NAME).tar HEAD
	$(Q)bzip2 $(SRCZIP_NAME).tar

# Generate archive with binary files
ifeq ("$(TARGET_OS)","MINGW32")
BINZIP_NAME := $(PACKAGE)-bin-mingw32-$(VERSION)
else
BINZIP_NAME := $(PACKAGE)-bin-$(VERSION)
endif
.PHONY: bin-zip
bin-zip:
	@rm -f $(BINZIP_NAME).tar
	@rm -f $(BINZIP_NAME).tar.bz2
	$(Q)tar -cf $(BINZIP_NAME).tar -C $(TARGET_OUT_FINAL) \
		--transform "s/\.\(\/.*\)/$(BINZIP_NAME)\\1/" .
	$(Q)bzip2 $(BINZIP_NAME).tar

