set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
#set(FlyCapture_HEADERS "/usr/include/flycapture")
#set(FlyCapture_LIBS "flycapture")

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/../caffe/lib ${CMAKE_SOURCE_DIR}/SDK/lib )

SET(NAME "master")
project({NAME})

file( GLOB LIB_SOURCES *.cpp )
file( GLOB LIB_HEADERS *.h )

add_executable( ${NAME} ${LIB_SOURCES} ${LIB_HEADERS} )

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_HEADERS}${FlyCapture_HEADERS})
target_link_libraries( ${NAME} ${OpenCV_LIBS} ${FlyCapture_LIBS})

include_directories(${CMAKE_SOURCE_DIR}/common)
target_link_libraries( ${NAME} common )

include_directories(${CMAKE_SOURCE_DIR}/player_src)
target_link_libraries( ${NAME} player )
