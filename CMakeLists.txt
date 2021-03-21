cmake_minimum_required(VERSION 3.14)
project(test_ffms2)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(ffms2)

add_executable(test_ffms2 test.cpp)
target_link_libraries(test_ffms2 ffms2)
