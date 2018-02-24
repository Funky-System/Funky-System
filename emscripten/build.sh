#!/bin/bash

set -e

rm -rf emcc_build
mkdir emcc_build
cd emcc_build

git clone https://github.com/Funky-System/Funky-VM.git

python ../../fonts/font_builder.py

for file in ../../runtime/*.wckd
do
    wickedc --arch 32 "$file" -o $(basename -s .wckd "$file").funk
done

emcc -O3 ../../*.c ../../runtime/*.c -DVM_ARCH_BITS=32 -DVM_NATIVE_MALLOC=1 -s USE_SDL=2 -o funky.html \
    -I. \
    -I../../fonts \
    -I../../include \
    -IFunky-VM/include \
    Funky-VM/src/libvm/*.c \
    Funky-VM/src/libvm/instructions/*.c \
    $(ls *.funk | awk '{printf " --embed-file %s",$0}') \
    \
    -s WASM=0 -s PRECISE_F32=1 \
    --shell-file ../shell.html

