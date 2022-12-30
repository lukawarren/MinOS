set(CMAKE_SYSTEM_NAME MinOS)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(triple i686-unknown-none)

# Assembly
set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_ASM_COMPILER_TARGET ${triple})
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "asm;nasm;S")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
<FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

# Rest of toolchain
set(CMAKE_C_COMPILER i686-minos-gcc)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER i686-minos-g++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(CMAKE_LINKER i686-minos-gcc)
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> -lgcc")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> -lgcc")

# Flags
set (WARNINGS "-Wall -Wextra -pedantic -Wpointer-arith -Wcast-align\
 -Wwrite-strings -Wredundant-decls -Winline\
 -Wno-long-long -Wconversion")

# Using -O0 because it makes compiling way faster - will turn off for release builds
set (COMMON_FLAGS "-O0 -g -ffreestanding -fdiagnostics-color=always")

set (CMAKE_CXX_FLAGS "-std=c++20 ${WARNINGS} ${COMMON_FLAGS} -fno-exceptions\
 -fno-rtti -fno-use-cxa-atexit -fno-sized-deallocation\
 -nostdinc++")

set (CMAKE_C_FLAGS "-std=gnu99 ${WARNINGS} ${COMMON_FLAGS}")

# Linking
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
add_link_options("-ffreestanding")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/toolchain/CMake")
