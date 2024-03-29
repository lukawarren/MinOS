cmake_minimum_required(VERSION 3.1.3)
project(kernel C CXX ASM)
enable_language(ASM_NASM)

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../userspace/musl/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../userspace/musl/obj/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../userspace/musl/arch/i386)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../userspace/musl/arch/generic)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../userspace/minlib/include)

# Add source files
file(GLOB_RECURSE C_SRCS src/*.c src/*/*.c)
file(GLOB_RECURSE CPP_SRCS src/*.cpp src/*/*.cpp)
file(GLOB_RECURSE ASM_SRCS src/*.S src/*/*.S)

# linker.ld
add_link_options("-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

# Make freestanding and disable Musl "shadowing" warning
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -nostdlib -nostdinc -Wno-redundant-decls")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -nostdlib -nostdinc -Wno-redundant-decls")
add_link_options("-nostdlib")

# For klib macro magic
string(LENGTH "${CMAKE_SOURCE_DIR}/kernel/src/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# For minlib macro magic
add_definitions(-DKERNEL)

# Kernel target
add_executable(${PROJECT_NAME}.bin ${ASM_SRCS} ${C_SRCS} ${CPP_SRCS})

# clang-tidy
#find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
#set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--use-color")
#set_target_properties(kernel.bin PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")