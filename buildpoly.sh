#!/bin/bash

mkdir -p objects

clang-22 -O2 -S -emit-llvm \
  -I./PolyBenchC-4.2.1/utilities/ \
  -I./PolyBenchC-4.2.1/linear-algebra/blas/gemm \
  -c ./PolyBenchC-4.2.1/linear-algebra/blas/gemm/gemm.c \
  -o ./objects/gemm.ll

# generate MIR
llc-22 -stop-after=prologepilog ./objects/gemm.ll -o ./objects/gemm.mir

# run on MIR
llc-22 -load ./build/RegisterAccessPostRAPass.so \
  -run-pass=reg-access-postra ./objects/gemm.mir -o /dev/null
