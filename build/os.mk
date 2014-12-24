#############################################################################
## @file os.mk
## @author R. Roffé
## @date 2014/12/25
###############################################################################

###############################################################################
## Host/Target OS.
###############################################################################

# Host OS
ifneq ("$(shell echo $$OSTYPE | grep msys)","")
  HOST_OS := MINGW32
else ifneq ("$(shell echo $$OSTYPE | grep darwin)","")
  HOST_OS := MACOSX
else
  HOST_OS := LINUX
endif

# Target OS : default to HOST_OS unless set
ifndef TARGET_OS
  TARGET_OS := $(HOST_OS)
endif