# Modify compiler flags so as to include minlib
set (USERSPACE_FLAGS "-I ${CMAKE_CURRENT_SOURCE_DIR}/../minlib/include")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USERSPACE_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USERSPACE_FLAGS}")
