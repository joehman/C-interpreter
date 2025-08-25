
file(GLOB_RECURSE SRCS "*.c")

link_libraries("m")
add_executable(${PROJECT_NAME} ${SRCS})
