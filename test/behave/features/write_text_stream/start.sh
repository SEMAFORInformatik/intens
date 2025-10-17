#!/usr/bin/env sh

# change directory
export APPHOME=$(dirname "$(realpath $0)")
cd "${APPHOME}"

INTENS_REPLY_PORT=$1

mkdir -p reality
rm -r reality/*

intens --replyPort ${INTENS_REPLY_PORT} etc/streamWriteReal.des
