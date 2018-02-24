#!/bin/bash
set -e

ARCH=$3
if [ -z ${3+x} ]; then ARCH=32; fi

for file in $1/*.wckd
do
    echo Compiling runtime object $(basename -s .wckd "$file")
    wickedc --arch $ARCH -K "$file" -o $2/$(basename -s .wckd "$file").funk
done