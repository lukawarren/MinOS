cmake_minimum_required(VERSION 3.1.3)
project(minshell CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/../userspace.cmake")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lua")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(${PROJECT_NAME}.bin src/main.cpp src/framebuffer.cpp src/keyboard.cpp)
target_link_libraries(${PROJECT_NAME}.bin lua)