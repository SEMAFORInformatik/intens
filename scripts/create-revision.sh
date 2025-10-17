#!/usr/bin/env bash
# create intens revision file
# arg filename rev
#
if [ $# -lt 1 ]; then
     echo "Usage $0 <filename> [rev]"
     exit 1
fi
if [ -z "$2" ]; then
    # desc:
    # R5-2-15 : no commit since tag
    # R5-2-15-dirty : with local modifications
    # R5-2-15-2-g3bb98464 : 2 commits since tag
    # R5-2-15-2-g3bb98464-dirty : with local modifications
    desc=$(git describe --dirty)
    # activate below line to cheat: remove dirty
    # desc=$(git describe)
    # rev: desc without tag (R5-2-15-)
    rev=$(echo $desc | sed -r 's/^[^g]*-([0-9]+-g[^-]+|dirty)/\1/')
    if [ "$desc" == "$rev" ]; then
        # no commit since tag, no dirty: tag not yet removed
        rev=""
    fi
    date=$(git log -1 --format=%cd --date=short)
else
    echo "SETTING REV to $2"
    rev=$2
    date=""
fi

cat > $1 <<EOF
#define RevisionString "${rev}"
#define RevisionDateString "${date}"
EOF
