#!/bin/bash

# Formats all of the source files using clang-format.
clang-format -i -style=Google $(find src test examples -name "*.cpp" -o -name "*.h" -o -name "*.ino")
