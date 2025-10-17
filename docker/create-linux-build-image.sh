#!/bin/bash
# script to create docker image semafor/intens/linux-build
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

IMG_VERSION=$IMG_TAG

IMG_NAME=hub.semafor.ch/semafor/intens/linux-build
docker build \
       --label "org.label-schema.name=intens-$(basename $IMG_NAME)" \
       --label "org.label-schema.version=$IMG_VERSION" \
       --label "org.label-schema.build-date=$(date -u +'%Y-%m-%dT%H:%M:%SZ')" \
       --label "org.label-schema.vcs-ref=$VCS_REV" \
       -t ${IMG_NAME}:latest -f $path/Dockerfile-linux-build $parent

[ "${IMG_TAG}" = "latest" ] || docker tag ${IMG_NAME}:latest ${IMG_NAME}:${IMG_TAG}
