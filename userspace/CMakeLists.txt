# Linker to force alignment of sections to page boundaries
# TODO: patch binutils directly
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/userspace/linker.ld")

add_subdirectory(lua)
add_subdirectory(hello-world)
add_subdirectory(minwm)
add_subdirectory(minshell)
add_subdirectory(doomgeneric)