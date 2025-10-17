#! /bin/sh
# configure, build and package intens executables for Windows
# using docker mxe dockcross image (https://mxe.cc)
# script for build intens executables for Windows
# using docker mxe dockcross image (https://mxe.cc)
#
# Author Ronald Tanner, Günther Amsler
#
path=$(dirname "$(realpath $0)")

DOCKCROSS_IMAGE=hub.semafor.ch/semafor/intens/dockcross-wintens-build:2025-09
TARGET_DIR=/work/build-windows-shared-x64
if [ "$1" = static ]; then
  echo "Static"
  DOCKCROSS_IMAGE=hub.semafor.ch/semafor/intens/dockcross-wintens-build:2025-09
  TARGET_DIR=/work/build-windows-static-x64
  PLATFORM_EXT="-static"
fi
SOURCE_DIR=$(cd $(dirname $(dirname $0));pwd)
REVISION=$(git -C ${SOURCE_DIR} describe)
_UID=$(id -u )
_GID=$(id -g )
DOCKCROSS="docker run --rm --name dockcross-$$RANDOM \
-v ${SOURCE_DIR}:/work \
-u ${_UID}:${_GID} \
${DOCKCROSS_IMAGE}"

# NOTE: -DUSE_LSP is off, cannot use matlab in shared config
${DOCKCROSS} bash -c "cmake${PLATFORM_EXT} -DUSE_OAUTH=True -B ${TARGET_DIR} -S . && \
    cmake${PLATFORM_EXT} --build $TARGET_DIR -j8 && (cd ${TARGET_DIR}; cpack${PLATFORM_EXT})"
