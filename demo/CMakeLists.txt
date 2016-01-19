cmake_minimum_required(VERSION 2.8)

if(APPLE)
  set(CMAKE_PREFIX_PATH /usr/local/Cellar/qt5/5.5.1_2)
endif()

find_package(CGAL COMPONENTS Core)
include(${CGAL_USE_FILE})

find_package(Qt5 COMPONENTS Widgets Script)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall")

if(CMAKE_BUILD_TYPE EQUAL Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(APPROX_BEZIERS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPROX_BEZIERS")
endif()

set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS, TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libcsmerge/include)
include_directories(${Qt5Widgets_INCLUDES})
include_directories(${GMP_INCLUDE_DIR})

link_directories(${CMAKE_BINARY_DIR}/libcsmerge)

file(GLOB_RECURSE SRCS src/*.cpp)

qt5_generate_moc("${CMAKE_CURRENT_SOURCE_DIR}/include/Demo.hpp" "${CMAKE_CURRENT_BINARY_DIR}/Demo.moc")
list(APPEND SRCS Demo.moc)

add_executable(demo ${SRCS})

qt5_use_modules(demo Script)
target_link_libraries(demo ${Qt5Widgets_LIBRARIES} gmp csmerge)

if(CGAL_AUTO_LINK_ENABLED)
  target_link_libraries(demo ${CGAL_3RD_PARTY_LIBRARIES} )
else()
  target_link_libraries(demo ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
endif()
