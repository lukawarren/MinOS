cmake_minimum_required(VERSION 3.1.3)
project(hello-world C)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/../lua")
add_executable(${PROJECT_NAME}.bin main.c)
target_link_libraries(${PROJECT_NAME}.bin lua)