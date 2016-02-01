cmake_minimum_required(VERSION 2.8)

if(APPLE)
  find_package(Boost COMPONENTS python3 REQUIRED)
  execute_process(COMMAND python3-config --includes OUTPUT_VARIABLE PYTHON_INCLUDES OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND python3-config --ldflags OUTPUT_VARIABLE PYTHON_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(CMAKE_CXX_FLAGS "${PYTHON_INCLUDES}")
else()
  find_package(Boost COMPONENTS python-py34 REQUIRED)
  set(Python_ADDITIONAL_VERSIONS 3.4)
  find_package(PythonLibs 3.4 REQUIRED)

  include_directories(${PYTHON_INCLUDE_DIR})
endif()

find_package(CGAL COMPONENTS Core)
include(${CGAL_USE_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall")

if(CMAKE_BUILD_TYPE EQUAL Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(APPROX_BEZIERS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPROX_BEZIERS")
endif()

set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS, TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libcsmerge/include)
include_directories(${Boost_INCLUDE_DIR})

file(GLOB_RECURSE SRCS *.cpp)

set(LIB_CSMERGE_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}csmerge${CMAKE_STATIC_LIBRARY_SUFFIX}")

add_library(pycsmerge SHARED ${SRCS})
set_target_properties(pycsmerge PROPERTIES PREFIX "_")
set_target_properties(pycsmerge PROPERTIES SUFFIX ".so")
target_link_libraries(pycsmerge ${CMAKE_BINARY_DIR}/libcsmerge/${LIB_CSMERGE_NAME})
target_link_libraries(pycsmerge mpfr gmp ${Boost_LIBRARIES} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

if(APPLE)
  set_target_properties(pycsmerge PROPERTIES LINK_FLAGS "${PYTHON_LD_FLAGS}")
  set(MACOSX_RPATH "ON")
else()
  target_link_libraries(pycsmerge "${PYTHON_LIBRARY}")
endif()
