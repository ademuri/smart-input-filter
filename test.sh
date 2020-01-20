#!/bin/bash

set -euo pipefail

mkdir -p build
pushd build
cmake ..
make
./filtertest
popd
