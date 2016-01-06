cmake_minimum_required(VERSION 2.8)

find_package(CGAL COMPONENTS Core)
include(${CGAL_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libcsmerge/include)

link_directories(${CMAKE_BINARY_DIR}/libcsmerge)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/lib)

add_compile_options(-std=c++11)
add_compile_options(-g)

file(GLOB_RECURSE SRCS src/*.cpp)
add_executable(unitTests ${SRCS})

target_link_libraries(unitTests csmerge gtest gtest_main)

if(CGAL_AUTO_LINK_ENABLED)
  target_link_libraries(unitTests ${CGAL_3RD_PARTY_LIBRARIES} )
else()
  target_link_libraries(unitTests ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
endif()
