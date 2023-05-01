cmake_minimum_required(VERSION 3.1.3)
project(snake C CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/../minlib.cmake")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../minwm/include/")

add_executable(${PROJECT_NAME}.bin src/main.cpp)
target_link_libraries(${PROJECT_NAME}.bin)