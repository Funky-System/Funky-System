#!/bin/bash

set -e

rm -rf emcc_build
mkdir emcc_build
cd emcc_build

git clone https://github.com/Funky-System/Funky-VM.git

python ../../fonts/font_builder.py

for file in ../../runtime/*.wckd
do
    wickedc "$file" -o $(basename -s .wckd "$file").funk
done

emcc -O3 ../../*.c ../../runtime/*.c -s USE_SDL=2 -o funky.html \
    -I. \
    -I../../fonts \
    -I../../include \
    -IFunky-VM/include \
    Funky-VM/src/libvm/*.c \
    Funky-VM/src/libvm/instructions/*.c \
    --embed-file kernel.funk \
    --embed-file draw.funk \
    --embed-file color.funk \
    --embed-file input.funk \
    --embed-file stdlib.funk \
    --embed-file timer.funk \
    --embed-file console.funk \
    --embed-file math.funk \
    --embed-file test_app.funk \
    --embed-file editor.funk \
    --embed-file editor_code.funk \
    --embed-file editor_gfx.funk \
    \
    -s WASM=0 -s PRECISE_F32=1 \
    --shell-file ../shell.html

