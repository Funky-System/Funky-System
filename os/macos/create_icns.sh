#!/bin/sh

mkdir -p "$2.iconset"
sips -z 16 16     "$1" --out "$2.iconset/icon_16x16.png" > /dev/null
sips -z 32 32     "$1" --out "$2.iconset/icon_16x16@2x.png" > /dev/null
sips -z 32 32     "$1" --out "$2.iconset/icon_32x32.png" > /dev/null
sips -z 64 64     "$1" --out "$2.iconset/icon_32x32@2x.png" > /dev/null
sips -z 128 128   "$1" --out "$2.iconset/icon_128x128.png" > /dev/null
sips -z 256 256   "$1" --out "$2.iconset/icon_128x128@2x.png" > /dev/null
sips -z 256 256   "$1" --out "$2.iconset/icon_256x256.png" > /dev/null
sips -z 512 512   "$1" --out "$2.iconset/icon_256x256@2x.png" > /dev/null
sips -z 512 512   "$1" --out "$2.iconset/icon_512x512.png" > /dev/null
cp "$1" "$2.iconset/icon_512x512@2x.png"
iconutil --convert icns "$2.iconset"
rm -R "$2.iconset"
