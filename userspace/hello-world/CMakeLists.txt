cmake_minimum_required(VERSION 3.1.3)
project(hello-world C)

add_link_options("-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
add_link_options("${CMAKE_CURRENT_SOURCE_DIR}/../musl/lib/libc.a")
add_link_options("${CMAKE_CURRENT_SOURCE_DIR}/../musl/lib/crt1.o")
add_link_options("-lgcc")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I ${CMAKE_CURRENT_SOURCE_DIR}/../musl/include -I ${CMAKE_CURRENT_SOURCE_DIR}/../musl/obj/include")

add_executable(${PROJECT_NAME}.bin main.c)