cmake_minimum_required(VERSION 2.8)

find_package(CGAL COMPONENTS Core)
include(${CGAL_USE_FILE})

find_package(Boost COMPONENTS python-py34 REQUIRED)
set(Python_ADDITIONAL_VERSIONS 3.4)
find_package(PythonLibs 3.4 REQUIRED)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS, TRUE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libcsmerge/include)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIR})

file(GLOB_RECURSE SRCS *.cpp)

add_library(pycsmerge SHARED ${SRCS})
set_target_properties(pycsmerge PROPERTIES PREFIX "_")
target_link_libraries(pycsmerge ${CMAKE_BINARY_DIR}/libcsmerge/libcsmerge.a)
target_link_libraries(pycsmerge gmp ${Boost_LIBRARIES} ${PYTHON_LIBRARY} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
