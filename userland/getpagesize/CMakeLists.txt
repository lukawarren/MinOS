# Linker
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")
set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Stdlib
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/i686-minos/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/i686-minos/lib)

# Add source files
FILE(GLOB_RECURSE C_SRCS src/*.c)
FILE(GLOB_RECURSE CPP_SRCS src/*.cpp)
FILE(GLOB_RECURSE ASM_SRCS src/*.S)

ADD_EXECUTABLE(getpagesize.bin ${ASM_SRCS} ${C_SRCS} ${CPP_SRCS})
target_link_libraries(getpagesize.bin libm.a libc.a libg.a)
