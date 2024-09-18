#!/bin/sh

if [ "$1" = "-r" ]; then
  if [ -f "./wind" ]; then
    rm "./wind"
  fi
  cd ..; make; cd target
fi

set -e

XEPHYR=$(command -v Xephyr)
xinit ./xinitrc -- \
  "$XEPHYR" \
  :100 \
  -ac \
  -screen 1280x720 \
  -host-cursor
