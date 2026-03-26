#!/usr/bin/env sh
#
# change directory
export APPHOME=$(dirname "$(realpath $0)")
cd "${APPHOME}"

INTENS_REPLY_PORT=${1:-15560}
# unitManager with comboboxes only if you have a selection
intens --unitManager=comboBox_hide_single --replyPort ${INTENS_REPLY_PORT} etc/unitmgr.des
