#!/bin/bash

set -euo pipefail

platformio ci --board uno --board teensy31 --board adafruit_feather_m0 --board esp32dev --lib=. examples/debounce
