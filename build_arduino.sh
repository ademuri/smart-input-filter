#!/bin/bash

set -euo pipefail

if [ "${1-}" == "-h" ] || [ "${1-}" == "--help" ]; then
  echo "usage: build_arduino.sh <board>"
  exit 0
fi

BUILD_ALL=true
BOARD_TO_BUILD=
if [ "${1-}" != "" ]; then
  BUILD_ALL=false
  BOARD_TO_BUILD="$1"
fi

BOARDS="--board teensy31 --board adafruit_feather_m0 --board esp32dev"
LIBC_NEEDED_BOARDS="--board uno"

for dir in examples/*; do
  if [ "$BUILD_ALL" == "true" ]; then
    platformio ci ${BOARDS?} --lib=. "${dir?}"
    platformio ci ${LIBC_NEEDED_BOARDS?} --project-option="lib_deps=ArduinoSTL" --lib=. "${dir?}"
  else
    platformio ci --board "${BOARD_TO_BUILD?}" --lib=. "${dir?}"
  fi
done
