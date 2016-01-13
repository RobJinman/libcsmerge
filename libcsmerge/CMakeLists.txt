cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -fPIC")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/include)

file(GLOB_RECURSE SRCS src/*.cpp)

add_library(csmerge STATIC ${SRCS})
