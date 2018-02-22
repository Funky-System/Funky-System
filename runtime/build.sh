#!/bin/bash
set -e

for file in $1/*.wckd
do
    echo Compiling runtime object $(basename -s .wckd "$file")
    wickedc "$file" -o $2/$(basename -s .wckd "$file").funk
done