#!/bin/bash
set -e

if command -v wickedc &> /dev/null; then
    WICKEDC=wickedc
elif test -f "../../bin/wickedc"; then
    WICKEDC=../../bin/wickedc
elif test -f "../../bin/wickedc.exe"; then
    WICKEDC=../../bin/wickedc.exe
else
    >&2 echo "Couldn't find wickedc"
    exit 1
fi

ARCH=$3
if [ -z ${3+x} ]; then ARCH=32; fi

for file in "$1"/*.wckd
do
    echo Compiling runtime object $(basename -s .wckd "$file")
    "$WICKEDC" --arch $ARCH -K "$file" -o "$2/$(basename -s .wckd "$file").funk"
done