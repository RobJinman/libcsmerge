cmake_minimum_required(VERSION 2.8)

find_package(CGAL QUIET COMPONENTS Core REQUIRED)

include(${CGAL_USE_FILE})

set(CMAKE_CXX_FLAGS "-std=c++11 -g -fPIC")

if(APPROX_BEZIERS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPROX_BEZIERS")
endif()

set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS, TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/include)

file(GLOB_RECURSE SRCS src/*.cpp)

add_library(csmerge STATIC ${SRCS})
