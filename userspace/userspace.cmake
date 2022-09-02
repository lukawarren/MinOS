add_link_options("-T${CMAKE_CURRENT_SOURCE_DIR}/../linker.ld")
add_link_options("${CMAKE_CURRENT_SOURCE_DIR}/../musl/lib/libc.a")
add_link_options("${CMAKE_CURRENT_SOURCE_DIR}/../musl/lib/crt1.o")
add_link_options("-static-libgcc")
add_link_options("-lgcc")

set (USERSPACE_FLAGS "-I ${CMAKE_CURRENT_SOURCE_DIR}/../musl/include -I ${CMAKE_CURRENT_SOURCE_DIR}/../musl/obj/include -I ${CMAKE_CURRENT_SOURCE_DIR}/../musl/arch/i386/ -I ${CMAKE_CURRENT_SOURCE_DIR}/../musl/arch/generic/")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USERSPACE_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USERSPACE_FLAGS}")
