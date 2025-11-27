
file(GLOB_RECURSE SRCS "*.c")

add_library(mtUtilities ${SRCS})
target_include_directories(mtUtilities PRIVATE ../include)
