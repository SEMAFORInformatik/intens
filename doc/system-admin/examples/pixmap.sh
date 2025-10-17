#!/bin/sh
#
# $Id: $
#

P="`dirname \"$0\"`";
export APPHOME="`(cd \"$P\";pwd)`"

export INTENS_HOME=/usr/local/intens/4.0

"${INTENS_HOME}/bin/intens" \
  --helpmsg \
  --maxlines 10000 \
  --qtGuiStyle Plastique \
  --withoutWheelEvent \
  "pixmap.des"
