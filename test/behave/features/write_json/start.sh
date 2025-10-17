#!/usr/bin/env sh
#
# change directory
export APPHOME=$(dirname "$(realpath $0)")
cd "${APPHOME}"

INTENS_REPLY_PORT=$1

rm -rf reality
mkdir -p reality

intens --replyPort ${INTENS_REPLY_PORT} etc/json.des
diff -rq expected reality
