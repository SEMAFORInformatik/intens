#!/usr/bin/env sh
#
# change directory
export APPHOME=$(dirname "$(realpath $0)")
cd "${APPHOME}"

INTENS_REPLY_PORT=$1

rm -rf reality
mkdir -p reality

rm -rf reality
mkdir -p reality/i
mkdir -p reality/i-indented
mkdir -p reality/data
mkdir -p reality/data-indented

intens --replyPort ${INTENS_REPLY_PORT} etc/json.des

diff -r expected reality
