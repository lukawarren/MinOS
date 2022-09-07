cmake_minimum_required(VERSION 3.1.3)
project(scalable-font2 C)
include("${CMAKE_CURRENT_SOURCE_DIR}/../userspace.cmake")
add_library(${PROJECT_NAME} STATIC ssfn.c)