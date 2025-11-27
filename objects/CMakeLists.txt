
file(GLOB_RECURSE SRCS "*.c")

add_library(mtObjects ${SRCS})
target_include_directories(mtObjects PRIVATE ../include)
