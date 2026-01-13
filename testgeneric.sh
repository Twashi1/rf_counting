#!/bin/bash

filename=$(basename "$1")

mkdir -p objects

clang-22 -O2 -S -emit-llvm \
  $1 \
  -o ./objects/$filename.ll

# generate MIR
#   after prologue/epilogue insertion, just about the latest stage we can go
llc-22 -stop-after=prologepilog ./objects/$filename.ll -o ./objects/$filename.mir

# run on MIR
llc-22 -debug-pass=Structure -load ./build/RegisterAccessPostRAPass.so \
  -run-pass=reg-access-postra ./objects/$filename.mir -o /dev/null
