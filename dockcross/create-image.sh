#!/bin/sh
# Create docker image for building intens.exe (using dockcross, MXE)
#
path=$(dirname "$(realpath $0)")
#
Dockerfile=Dockerfile
if [ $# -ne 1 ]; then
    echo "Usage $0 <tag-id>"
    exit 1
fi
count=$(echo $1 | sed 's/ /\n/g' | grep share | wc -l)
if [ $count -ne 0 ]; then
	Dockerfile=Dockerfile.shared
        cat Dockerfile | sed 's/static/shared/g' > $Dockerfile
        echo "build a shared Docker image."
fi
TAG_ID=$1
IMG_NAME=hub.semafor.ch/semafor/intens/dockcross-wintens-build

docker build --no-cache=true \
        --build-arg IMAGE=$IMG_NAME \
        --build-arg VERSION=$TAG_ID \
        --build-arg VCS_REF=$(git rev-parse --short HEAD) \
        --build-arg VCS_URL=$(git config --get remote.origin.url) \
        --build-arg BUILD_DATE=$(date -u +"%Y-%m-%dT%H:%M:%SZ") \
        -t ${IMG_NAME}:$TAG_ID -t ${IMG_NAME}:latest \
        -f $path/$Dockerfile $path
