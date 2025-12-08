#! /bin/sh
# configure, build and package intens executables for Linux
#
# Author Ronald Tanner, Günther Amsler
#
path=$(dirname "$(realpath $0)")

DOCKCROSS_IMAGE=ghcr.io/semaforinformatik/intens/linux-build:latest
if [ "$1" = alma ]; then
    DOCKCROSS_IMAGE=ghcr.io/semaforinformatik/intens/almalinux-build:10.1
    PLATFORM_PREFIX="alma-"
fi
TARGET_DIR=/work/build-${PLATFORM_PREFIX}linux-shared-x64
if [ "$1" = ubuntu ]; then
  echo "ubuntu"
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
${DOCKCROSS} bash -c "cmake -DUSE_OAUTH=True -DUSE_LSP=OFF -B ${TARGET_DIR} -S . && \
    cmake --build $TARGET_DIR -j8 && (cd ${TARGET_DIR}; cpack)"
