cmake_minimum_required(VERSION 3.10.0)
project(mint VERSION 0.1.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)
