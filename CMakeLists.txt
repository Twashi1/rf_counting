cmake_minimum_required(VERSION 3.13)
project(RISCVRegFilePass LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

find_package(LLVM 17 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)  

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINTIONS})

# TODO: more cmake way of doing this?
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_library(CountRFAcc MODULE "count.cpp")

set_target_properties(CountRFAcc PROPERTIES COMPILE_FLAGS "-fno-rtti")

# disable specific warnings from LLVM headers
target_compile_options(CountRFAcc PRIVATE -Wno-unused-command-line-argument)

# explicitly set output dir
set_target_properties(CountRFAcc PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

