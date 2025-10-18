#!/bin/bash

mkdir build
cd build
cmake -DLLVM_DIR=$(llvm-config-17 --cmakedir) ..
make -j$(nproc)
