cmake_minimum_required(VERSION 3.1.3)
project(kernel C CXX ASM)
enable_language(ASM_NASM)

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add source files
file(GLOB_RECURSE C_SRCS src/*.c src/*/*.c)
file(GLOB_RECURSE CPP_SRCS src/*.cpp src/*/*.cpp)
file(GLOB_RECURSE ASM_SRCS src/*.S src/*/*.S)

# linker.ld
add_link_options("-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

# For klib macro magic
string(LENGTH "${CMAKE_SOURCE_DIR}/kernel/src/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# Kernel target
add_executable(${PROJECT_NAME}.bin ${ASM_SRCS} ${C_SRCS} ${CPP_SRCS})

# clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--use-color")
set_target_properties(kernel.bin PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")