#!/bin/sh

mkdir -p images
for f in "media_raw/$1"*.jpg; do
	cp "$f" "images/${f#media_raw/$1}"
done
