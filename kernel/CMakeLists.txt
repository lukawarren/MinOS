cmake_minimum_required(VERSION 3.1.3)
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

# linker.ld
add_link_options("-fuse-ld=lld")
add_link_options("-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
add_link_options("--target=i686-pc-none-elf")
add_link_options("-march=i686")
add_link_options("-Wno-unused-command-line-argument") # because all the C / C++ flags below apply to us too

# Compile flags
set (WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Winline -Wno-long-long -Wconversion")
set (COMMON_FLAGS "-ffreestanding -nostdlib --target=i686-pc-none-elf -march=i686 -nostdinc")
set (CMAKE_CXX_FLAGS "-std=c++20 ${WARNINGS} ${COMMON_FLAGS} -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-sized-deallocation -nostdinc++")
set (CMAKE_C_FLAGS "-std=gnu99 ${WARNINGS} ${COMMON_FLAGS}")

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add source files
file(GLOB_RECURSE C_SRCS src/*.c src/*/*.c)
file(GLOB_RECURSE CPP_SRCS src/*.cpp src/*/*.cpp)
file(GLOB_RECURSE ASM_SRCS src/*.S src/*/*.S)

# For klib macro magic
string(LENGTH "${CMAKE_SOURCE_DIR}/kernel/src/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

add_executable(${PROJECT_NAME}.bin ${ASM_SRCS} ${C_SRCS} ${CPP_SRCS})

# clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--use-color")
set_target_properties(kernel.bin PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")