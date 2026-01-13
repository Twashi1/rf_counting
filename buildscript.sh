#!/bin/bash

mkdir -p build
cd build
cmake -DLLVM_DIR=$(llvm-config-22 --cmakedir) .. -G Ninja
cmake --build .
