#!/bin/bash
# script to create docker image semafor/intens
#

path=$(dirname "$(realpath $0)")
parent=$(realpath $path/..)
#
if [ $# -eq 1 ]; then
    IMG_TAG=$1
else
    IMG_TAG=latest
fi

pushd $parent
VCS_REV=$(git describe --dirty)

TAG_NAME=$IMG_TAG docker buildx bake
