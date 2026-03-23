#!/bin/sh
#
# $Id: $
#

P="`dirname \"$0\"`";
export APPHOME="`(cd \"$P\";pwd)`"
# default bitmap search path
#export BITMAP_PATH=${APPHOME}/bitmap

intens \
    --resfile navigator.ini \
    navigator.des
