#!/usr/bin/env sh

# change directory
export APPHOME=$(dirname "$(realpath $0)")
cd "${APPHOME}"

INTENS_REPLY_PORT=$1

export REST_SERVICE_BASE="http://localhost:15580"
export FLASK_APP=workbookDummy

flask run --port 15580&
API_PID=$!
trap "kill ${API_PID}" EXIT TERM KILL
intens --replyPort ${INTENS_REPLY_PORT} $2 etc/idle.des
