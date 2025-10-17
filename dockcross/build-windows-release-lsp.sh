#! /bin/sh
# configure, build and package intens executables for Windows
# using docker mxe dockcross image (https://mxe.cc)
# script for build intens executables for Windows
# using docker mxe dockcross image (https://mxe.cc)
#
# Author Ronald Tanner, Günther Amsler
#
path=$(dirname "$(realpath $0)")

DOCKCROSS_IMAGE=hub.semafor.ch/semafor/intens/dockcross-wintens-build:treesitter
TARGET_DIR=/work/build-windows-shared-x64
SOURCE_DIR=$(cd $(dirname $(dirname $0));pwd)/etc
REVISION=$(git -C ${SOURCE_DIR} describe)
_UID=$(id -u )
_GID=$(id -g )
DOCKCROSS="docker run --rm --name dockcross-$$RANDOM \
-v ${SOURCE_DIR}:/work \
-u ${_UID}:${_GID} \
-e WINEPREFIX=/tmp/wineuser
${DOCKCROSS_IMAGE}"

# NOTE: -DUSE_LSP is off, cannot use matlab in shared config
${DOCKCROSS} bash -c "cp -r /tmp/wine /tmp/wineuser && cmake -DUSE_LSP=ON -B ${TARGET_DIR} -S . && \
    cmake --build $TARGET_DIR --target lsp -j8"
