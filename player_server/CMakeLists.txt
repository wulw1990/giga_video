set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/../caffe/lib ${CMAKE_SOURCE_DIR}/SDK/lib )

SET(NAME "recall_hack_train")
project({NAME})

file( GLOB LIB_SOURCES *.cpp )
file( GLOB LIB_HEADERS *.h )

add_executable( ${NAME} ${LIB_SOURCES} ${LIB_HEADERS} )

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_HEADERS})
target_link_libraries( ${NAME} ${OpenCV_LIBS} )

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/head)
include_directories(${CMAKE_SOURCE_DIR}/SDK/include)
include_directories(${CMAKE_SOURCE_DIR}/../caffe/include)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/../caffe/lib ${CMAKE_SOURCE_DIR}/SDK/lib )
target_link_libraries( ${NAME} caffe )
target_link_libraries( ${NAME} cvface_api )

include_directories(${CMAKE_SOURCE_DIR}/common_tool)
target_link_libraries( ${NAME} common_tool )

include_directories(${CMAKE_SOURCE_DIR}/common_base)
target_link_libraries( ${NAME} common_base )

include_directories(${CMAKE_SOURCE_DIR}/recall_hack_src)
target_link_libraries( ${NAME} live_recall_hack )
