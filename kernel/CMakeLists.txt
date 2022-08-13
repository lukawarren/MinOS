cmake_minimum_required(VERSION 3.0)
project(kernel C CXX ASM)

# Assembly
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "asm;nasm;S")
enable_language(ASM_NASM)
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

# C/C++ toolchain
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# Compile flags
set (WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Winline -Wno-long-long -Wconversion")
set (COMMON_FLAGS "-ffreestanding -nostdlib -fuse-ld=lld --target=i686-pc-none-elf -march=i686")
set (CMAKE_CXX_FLAGS "-std=c++20 ${WARNINGS} ${COMMON_FLAGS} -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-sized-deallocation")
set (CMAKE_C_FLAGS "-std=gnu99 ${WARNINGS} ${COMMON_FLAGS}")

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add source files
file(GLOB_RECURSE C_SRCS src/*.c)
file(GLOB_RECURSE CPP_SRCS src/*.cpp)
file(GLOB_RECURSE ASM_SRCS src/*.S)

add_executable(${PROJECT_NAME}.bin ${ASM_SRCS} ${C_SRCS} ${CPP_SRCS})