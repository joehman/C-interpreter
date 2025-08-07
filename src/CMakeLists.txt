
file(GLOB_RECURSE SRCS "*.c")

link_libraries("m")
add_executable(interpreter ${SRCS})
