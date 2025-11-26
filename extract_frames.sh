#!/bin/sh

mkdir -p images
ffmpeg -ss 0.1 -i "$1" -t "$(ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 $1 | awk '{print $1 - 0.2}')" images/%03d.png
