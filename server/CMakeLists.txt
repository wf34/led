set(PRJ "serv")
project("${PRJ}")
cmake_minimum_required(VERSION 2.8)

include_directories(-I/usr/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb")
file( GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h )
file( GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.cpp )
add_executable("${PRJ}" ${HEADERS} ${SOURCES})
target_link_libraries("${PRJ}" event pthread)

