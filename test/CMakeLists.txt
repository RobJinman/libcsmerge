cmake_minimum_required(VERSION 2.8)

find_package(CGAL COMPONENTS Core)
include(${CGAL_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libcsmerge/include)

link_directories(${CMAKE_BINARY_DIR}/libcsmerge)
if(APPLE)
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/lib/osx)
else()
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/lib)
endif()

set(CMAKE_CXX_FLAGS "-std=c++11 -g")

if(APPROX_BEZIERS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPROX_BEZIERS")
endif()

set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS, TRUE)

file(GLOB_RECURSE SRCS src/*.cpp)
add_executable(unitTests ${SRCS})

target_link_libraries(unitTests csmerge gtest gtest_main -pthread -lm)

if(CGAL_AUTO_LINK_ENABLED)
  target_link_libraries(unitTests ${CGAL_3RD_PARTY_LIBRARIES} )
else()
  target_link_libraries(unitTests ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
endif()
