cmake_minimum_required(VERSION 3.1.3)
project(hello-world C)
add_link_options("-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
add_executable(${PROJECT_NAME}.bin main.c)