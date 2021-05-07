CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(MinOS)

# Assembly
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "asm;nasm;S")
ENABLE_LANGUAGE(ASM_NASM)
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

# Rest of toolchain
set(tools $ENV{HOME}/opt/cross/bin/)
set(CMAKE_C_COMPILER ${tools}i686-elf-gcc)
set(CMAKE_CXX_COMPILER ${tools}i686-elf-g++)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# Set compile flags
set (WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Winline -Wno-long-long -Wconversion")
set (CMAKE_CXX_FLAGS "-std=c++17 ${WARNINGS} -ffreestanding -ffreestanding -O2 -fno-exceptions -fno-rtti -nostdlib")
set (CMAKE_C_FLAGS "-std=gnu99 ${WARNINGS} -ffreestanding -O2 -nostdlib -lgcc")

# Add stuff
ADD_SUBDIRECTORY(kernel)

