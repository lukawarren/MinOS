cmake_minimum_required(VERSION 3.1.3)
project(doom C)
include("${CMAKE_CURRENT_SOURCE_DIR}/../minlib.cmake")

# Add sources
file(GLOB_RECURSE C_SRCS *.c)

# Ignore warnings
add_definitions(-w)

# Include minwm
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../minwm/include")

add_executable(${PROJECT_NAME}.bin ${C_SRCS})
target_link_libraries(${PROJECT_NAME}.bin)