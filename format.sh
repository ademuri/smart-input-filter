#!/bin/bash

# Formats all of the source files using clang-format.
clang-format -i -style=Google $(find . -name "*.cc" -o -name "*.h" -o -name "*.ino")
