# Linker
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")
set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../wm/include)

# Stdlib
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/i686-minos/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/minlib/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/i686-minos/lib)
FILE(GLOB_RECURSE MINLIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/minlib/src/*.cpp)

# Add source files
FILE(GLOB_RECURSE C_SRCS src/*.c)
FILE(GLOB_RECURSE CPP_SRCS src/*.cpp)
FILE(GLOB_RECURSE ASM_SRCS src/*.S)

ADD_EXECUTABLE(snake.bin ${ASM_SRCS} ${C_SRCS} ${CPP_SRCS} ${MINLIB_SRCS})
target_link_libraries(snake.bin libm.a libc.a libg.a)
