cmake_minimum_required(VERSION 2.8)
project(libcsmerge)

add_subdirectory(libcsmerge)
add_subdirectory(pycsmerge)
if(NOT APPLE)
  add_subdirectory(test)
endif()
add_subdirectory(demo)
