cmake_minimum_required(VERSION 3.1.3)
project(hello-world CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/../userspace.cmake")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lua")
file(GLOB_RECURSE C_SRCS main.cpp)

add_executable(${PROJECT_NAME}.bin ${C_SRCS})
target_link_libraries(${PROJECT_NAME}.bin lua)