
file(GLOB_RECURSE SRCS "*.c")

add_library(mtParser ${SRCS})
target_include_directories(mtParser PRIVATE ../include)
