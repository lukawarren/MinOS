cmake_minimum_required(VERSION 3.1.3)
project(minwm C CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/../userspace.cmake")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../scalable-font2")

add_executable(${PROJECT_NAME}.bin src/font.c src/main.cpp src/compositor.cpp)
target_link_libraries(${PROJECT_NAME}.bin)