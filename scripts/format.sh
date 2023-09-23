#!/usr/bin/env bash

cd $(git rev-parse --show-toplevel)
find src test submissions \
    -type f \
    -regex '.*\.\(cpp\|h\|hpp\)' \
    -exec clang-format -style=file -i {} \;