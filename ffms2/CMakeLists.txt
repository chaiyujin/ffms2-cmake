cmake_minimum_required(VERSION 3.14)
project(ffms2)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# check dependency
find_package(ZLIB   REQUIRED)
find_package(FFmpeg REQUIRED QUIET)

# fetch codes
file(GLOB_RECURSE _ffms2_headers ${PROJECT_SOURCE_DIR}/*.h   ${PROJECT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE _ffms2_sources ${PROJECT_SOURCE_DIR}/*.cxx ${PROJECT_SOURCE_DIR}/*.cpp)

GitHelper(backward https://github.com/bombela/backward-cpp.git v1.5 TRUE "" "")
# add library
# add_definitions("-DFFMS_WITH_DEPRECATED")  # enbale DEPRECATED
add_library               (${PROJECT_NAME} ${_ffms2_headers} ${_ffms2_sources} ${BACKWARD_ENABLE})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src/core)
target_link_libraries     (${PROJECT_NAME} PUBLIC FFmpeg::FFmpeg ZLIB::ZLIB)
