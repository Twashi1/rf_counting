cmake_minimum_required(VERSION 3.13)
project(RegisterAccessPostRAPass LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

find_package(LLVM 22 REQUIRED CONFIG)

message(STATUS "Using LLVM_DIR=${LLVM_DIR}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(RegisterAccessPostRAPass MODULE RegisterAccessPostRAPass.cpp)

set_target_properties(RegisterAccessPostRAPass PROPERTIES
  PREFIX ""
  SUFFIX ".so"
  POSITION_INDEPENDENT_CODE ON
)
