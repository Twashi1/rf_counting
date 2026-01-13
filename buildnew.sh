#!/bin/bash

mkdir -p build
cd build

cmake -DLLVM_ENABLE_PROJECTS="..." -G Ninja ../llvm
ninja # maybe should just use cmake --build .
