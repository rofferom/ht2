#!/bin/bash

# Configure build system
if [ "${TARGET_ARCH}" == "" ]
then
	echo "TARGET_ARCH is not defined (X86 or X64)"
	exit 1
elif [ "${TARGET_ARCH}" == "X86" ]
then
	export TOOLCHAIN=i686-w64-mingw32
	OUTDIR=out/final_mingw32_gcc_release
elif [ "${TARGET_ARCH}" == "X64" ]
then
	export TOOLCHAIN=x86_64-w64-mingw32
	OUTDIR=out/final_mingw32_gcc_x64_release
else
	echo "Arch ${TARGET_ARCH} is unknown"
	exit 1
fi

export TARGET_CROSS=${TOOLCHAIN}-
export TARGET_OS=MINGW32
export TARGET_GLOBAL_LDFLAGS=-static

# Internal variables to prepare libraries copy
TOOLCHAIN_PATH=`which ${TARGET_CROSS}gcc | xargs dirname`

# Launch build
make $*

# Copy libiconv-2.dll
cp ${TOOLCHAIN_PATH}/../${TOOLCHAIN}/bin/libiconv-2.dll ${OUTDIR}

