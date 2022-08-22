cmake_minimum_required(VERSION 3.1.3)
project(lua C)
include("${CMAKE_CURRENT_SOURCE_DIR}/../userspace.cmake")

# Ignore warnings
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-everything")

file(GLOB_RECURSE C_SRCS *.c)
add_library(${PROJECT_NAME} STATIC ${C_SRCS})