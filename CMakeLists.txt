cmake_minimum_required(VERSION 3.0.0)
project(cmsketch VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(cmsketch 
    src/main.cpp
    src/count_min_sketch.c
    src/utils.cpp
    src/exporter.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
target_include_directories(cmsketch PRIVATE ${RAPIDCSV_INCLUDE_DIRS})

find_package(tabulate CONFIG REQUIRED)
target_link_libraries(cmsketch PRIVATE tabulate::tabulate)