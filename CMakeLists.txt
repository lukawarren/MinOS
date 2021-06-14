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
set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_CXX_COMPILER i686-elf-g++)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# Set compile flags
set (WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Winline -Wno-long-long -Wconversion")
set (CMAKE_CXX_FLAGS "-std=c++17 ${WARNINGS} -ffreestanding -ffreestanding -O3 -fno-exceptions -fno-rtti -nostdlib -fno-use-cxa-atexit -fno-sized-deallocation")
set (CMAKE_C_FLAGS "-std=gnu99 ${WARNINGS} -ffreestanding -O3 -nostdlib")

# Add stuff
ADD_SUBDIRECTORY(kernel)

# Userland
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()
SUBDIRLIST(SUBDIRS ${PROJECT_SOURCE_DIR}/userland)
FOREACH(subdir ${SUBDIRS})
  ADD_SUBDIRECTORY(userland/${subdir})
ENDFOREACH()
