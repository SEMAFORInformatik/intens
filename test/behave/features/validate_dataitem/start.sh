#!/usr/bin/env sh

# change directory
P="`dirname \"$0\"`";
cd "$P"

export APPHOME="."

INTENS_REPLY_PORT=${1:-15560}

intens --replyPort ${INTENS_REPLY_PORT} valid.des > /dev/null 2>&1
