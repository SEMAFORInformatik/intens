#! /bin/sh
# build System Admin Manual (PDF)
# using docker image hub.semafor.ch/semafor/intens/ebnf-latex
#
# Author Ronald Tanner
#
SOURCE_DIR=$(dirname $(dirname $(cd $(dirname $0);pwd)))
_UID=$(id -u)
_GID=$(id -g)
DOCBUILD="docker run --rm --name docbuild-$$RANDOM \
-v ${SOURCE_DIR}:/work \
-u ${_UID}:${_GID} \
hub.semafor.ch/semafor/intens/ebnf-latex"

#echo $SOURCE_DIR
${DOCBUILD} bash -c "TEXMFVAR=/work/doc/system-admin/.texmf-var make"
