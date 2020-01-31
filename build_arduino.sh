#!/bin/bash

set -euo pipefail

BOARDS="--board teensy31 --board adafruit_feather_m0 --board esp32dev"
LIBC_NEEDED_BOARDS="--board uno"


platformio ci ${BOARDS?} --lib=. examples/debounce
platformio ci ${BOARDS?} --lib=. examples/exponential-moving-average
platformio ci ${BOARDS?} --lib=. examples/median-filter

platformio ci ${LIBC_NEEDED_BOARDS?} --project-option="lib_deps=ArduinoSTL" --lib=. examples/debounce
platformio ci ${LIBC_NEEDED_BOARDS?} --project-option="lib_deps=ArduinoSTL" --lib=. examples/exponential-moving-average
platformio ci ${LIBC_NEEDED_BOARDS?} --project-option="lib_deps=ArduinoSTL" --lib=. examples/median-filter
