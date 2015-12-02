cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/../caffe/lib ${CMAKE_SOURCE_DIR}/SDK/lib )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused")


SET(NAME "player")

file( GLOB LIB_SOURCES *.cpp )
file( GLOB LIB_HEADERS *.h )

add_library( ${NAME} ${LIB_SOURCES} ${LIB_HEADERS} )

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_HEADERS})
target_link_libraries( ${NAME} ${OpenCV_LIBS} )