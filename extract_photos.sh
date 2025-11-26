#!/bin/sh

mkdir -p media_raw
for f in "media_raw/$1"*.jpg; do
	cp "$f" "images/${f#media_raw/$1}"
done
