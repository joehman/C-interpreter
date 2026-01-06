file(GLOB_RECURSE SRCS "*.c")

add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ../include)
target_include_directories(${PROJECT_NAME} PRIVATE ../include/internal)

target_link_libraries(${PROJECT_NAME} PRIVATE mtUtilities)
target_link_libraries(${PROJECT_NAME} PRIVATE mtObjects)
target_link_libraries(${PROJECT_NAME} PRIVATE mtUtilities)
target_link_libraries(${PROJECT_NAME} PRIVATE mtParser)
