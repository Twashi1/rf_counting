#!/bin/bash

mkdir -p objects

# Note for PolyBench its run on x86-64
#   otherwise we'd need a libc for RISC-V
#   We also haven't modified the code to account for any special cases required for x86-64, so results might be inacurrate

clang-17 -O2 -emit-llvm \
  -I./PolyBenchC-4.2.1/utilities/ \
  -I./PolyBenchC-4.2.1/linear-algebra/blas/gemm \
  -c ./PolyBenchC-4.2.1/linear-algebra/blas/gemm/gemm.c \
  -o ./objects/gemm.bc

# generate MIR
llc-17 -march=x86-64 \
  -stop-after=finalize-isel ./objects/gemm.bc -o ./objects/gemm.mir

# run on MIR
llc-17 -march=x86-64 \
  -load ./build/libCountRFAcc.so \
  -run-pass=count-riscv-rf ./objects/gemm.mir
