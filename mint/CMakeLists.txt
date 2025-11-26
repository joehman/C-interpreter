
file(GLOB_RECURSE SRCS "*.c")

link_libraries("m")
add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ../include)
target_include_directories(${PROJECT_NAME} PRIVATE ../include/internal)
