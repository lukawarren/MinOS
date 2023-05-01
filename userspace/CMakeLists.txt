# Toggle POSIX includes for Musl
add_definitions(-D_POSIX_SOURCE)

add_subdirectory(lua)
add_subdirectory(hello-world)
add_subdirectory(minwm)
add_subdirectory(minshell)
add_subdirectory(doomgeneric)
add_subdirectory(snake)