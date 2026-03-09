#!/usr/bin/env sh
#
# change directory
export APPHOME=$(dirname "$(realpath $0)")
cd "${APPHOME}"

INTENS_REPLY_PORT=${1:-14560}
export INTENS_REPLY_B_PORT=${2:-24560}

# start api gateway
python mqrep.py &
API_PID=$!

# consistency check
intens --replyPort ${INTENS_REPLY_PORT} etc/consistency_check.des

trap "kill -9 ${API_PID}" SIGTERM SIGINT EXIT
