cmake_minimum_required(VERSION 3.1.3)
project(lua C)

# Ignore warnings
add_definitions(-w)

file(GLOB_RECURSE C_SRCS *.c)
add_library(${PROJECT_NAME} STATIC ${C_SRCS})