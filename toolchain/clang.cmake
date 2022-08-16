set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(triple i686-unknown-none)

# Assembly
set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_ASM_COMPILER_TARGET ${triple})
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "asm;nasm;S")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

# C
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})

# C++
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

# Flags
set (WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align\
                -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Winline\
                -Wno-long-long -Wconversion -Wno-gnu-zero-variadic-macro-arguments\
                -Wno-gnu-statement-expression")

set (COMMON_FLAGS "-ffreestanding -nostdlib --target=i686-pc-none-elf\
                    -march=i686 -nostdinc")

set (CMAKE_CXX_FLAGS "-std=c++20 ${WARNINGS} ${COMMON_FLAGS} -fno-exceptions\
                    -fno-rtti -fno-use-cxa-atexit -fno-sized-deallocation\
                    -nostdinc++")

set (CMAKE_C_FLAGS "-std=gnu99 ${WARNINGS} ${COMMON_FLAGS}")

# Linking flags
add_link_options("-fuse-ld=lld")
add_link_options("--target=i686-pc-none-elf")
add_link_options("-march=i686")
add_link_options("-Wno-unused-command-line-argument")
add_link_options("-nostdlib")
add_link_options("-ffreestanding")

# Linking
set(CMAKE_LINKER ld.lld)
set(CMAKE_C_LINK_EXECUTABLE "clang++ <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/toolchain/CMake")
