#!/usr/bin/env bash

set -euo pipefail

INFILES="rengine.c"
OUTFILE="rengine"
CFLAGS=""

if [[ ${1:-""} == '-h' ]]; then
    cat << EOF 
    build.sh
        -h: Help documentation
        -d: Generate debug symbols
EOF
    exit 0
fi

if [[ ${1:-""} == '-d' ]]; then
    CFLAGS="${CFLAGS} -g"
fi
gcc ${INFILES} ${CFLAGS} -o ${OUTFILE}