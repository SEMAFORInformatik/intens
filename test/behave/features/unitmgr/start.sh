#!/usr/bin/env sh
#
# change directory
export APPHOME=$(dirname "$(realpath $0)")
cd "${APPHOME}"

INTENS_REPLY_PORT=$1
intens --unitManager --replyPort ${INTENS_REPLY_PORT} etc/unitmgr.des
