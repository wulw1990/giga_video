cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
PROJECT(giga_video)

option (FLY_CAPTRUE "Use provided math implementation" OFF)
if(FLY_CAPTRUE)
	add_definitions(-DENABLE_FLY_CAPTRUE)
	set(FlyCapture_HEADERS "/usr/include/flycapture")
	set(FlyCapture_LIBS "flycapture")
endif(FLY_CAPTRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

SET(PATH_INC ${giga_video_SOURCE_DIR}/include)
SET(PATH_SRC ${giga_video_SOURCE_DIR}/src)

AUX_SOURCE_DIRECTORY(${PATH_SRC} INTERNALAPI_SRC_LIST)
INCLUDE_DIRECTORIES(${PATH_INC})

SET(MODULE_SRCS)

MACRO(ADD_MODULE module_list name)
	INCLUDE_DIRECTORIES(${PATH_INC}/${name})
	FILE(GLOB_RECURSE MODULE_SRC ${PATH_SRC}/${name}/*.cpp)
	LIST(APPEND ${module_list} ${MODULE_SRC})
ENDMACRO()

ADD_MODULE(MODULE_SRCS common)
ADD_MODULE(MODULE_SRCS construct)
ADD_MODULE(MODULE_SRCS server)
ADD_MODULE(MODULE_SRCS client)

add_executable( giga_video ${INTERNALAPI_SRC_LIST} ${MODULE_SRCS} )

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_HEADERS} ${FlyCapture_HEADERS})
target_link_libraries( giga_video ${OpenCV_LIBS} ${FlyCapture_LIBS})
