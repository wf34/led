set(PRJ "nc")
project("${PRJ}")
cmake_minimum_required(VERSION 2.8)

include_directories(-I/usr/include)

file( GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h )
file( GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.cpp )
add_executable("${PRJ}" ${HEADERS} ${SOURCES})
target_link_libraries("${PRJ}" ncurses)

