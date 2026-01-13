
#!/bin/bash

filename=$(basename "$1")

mkdir -p objects

clang-22 -O2 -c -emit-llvm \
  $1 \
  -o ./objects/$filename.bc

llc-22 ./objects/$filename.bc -o /dev/null
