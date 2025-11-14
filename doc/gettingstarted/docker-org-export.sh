#!/bin/sh

# Launch the org-mode exporter inside docker

# example: ./docker-org-export org-html-export-to-html slides.org
# example: ./docker-org-export org-latex-export-to-pdf handbook.org
# example: ./docker-org-export org-reveal-export-to-html slides.org

# Note: based on https://gitlab.com/olberger/docker-org-teaching-export
#
SCRIPTNAME="$(basename "$0")"
USAGE=`sed "s/__SCRIPTNAME__/$SCRIPTNAME/g" <<"EOF"
__SCRIPTNAME__ [-h] [-c] [-d] ORG_EXPORT_FUNCNAME document.org -- converts org-mode file using org-mode exporter function ORG_EXPORT_FUNCNAME

where:
    -h : show this help text
    ORG_EXPORT_FUNCNAME : name of the function used for export (org-html-export-to-html|org-reveal-export-to-html|org-latex-export-to-pdf|...)
    document.org : source org-mode document to export
    -d : debug : does'nt quit emacs and start it in full console, to allow debugging export
EOF
`
DEBUG=""

while getopts 'hd' option; do
  case "$option" in
    h) echo "$USAGE"
       exit
       ;;
    d) DEBUG=1
        ;;
  esac
done
shift $((OPTIND - 1))

if [ $# -lt 2 ]; then
    echo "Error: I need 2 args" >&2
    echo >&2
    echo "$USAGE" >&2
    exit 1
fi

docker_image=ghcr.io/semaforinformatik/docker-org-export:latest

# Handling -d option allowing to easily start the container and not quit emacs after export
if [ "x$DEBUG" != "x" ]; then
    KILLARG=
else
    KILLARG="--kill"
fi

emacs_export_command=$1

docker run --rm -i -t -v $(pwd):$(pwd) --workdir=$(pwd) -u $UID:$UID $docker_image emacs $2 --batch -l publish.el -f $emacs_export_command $KILLARG
