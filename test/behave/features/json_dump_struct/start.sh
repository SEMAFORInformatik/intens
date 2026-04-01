#!/usr/bin/env sh

# change directory
P="`dirname \"$0\"`";
cd "$P"

# bug in MessageQueueReply.inc
export REST_SERVICE_BASE="dummy"

INTENS_REPLY_PORT=${1:-12345}

intens --replyPort ${INTENS_REPLY_PORT} test.des > /dev/null 2>&1