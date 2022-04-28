cmake_minimum_required(VERSION 3.0.0)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INSTALL_MESSAGE LAZY)
project(Slim VERSION 0.0.0)
file(GLOB_RECURSE SOURCE "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-as-needed -ldl -pthread")
add_compile_options(-DV8_COMPRESS_POINTERS -std=c++2a -fconcepts)
if(DEFINED DEBUG)
    add_compile_options(-g -O0)
endif()
set(EXECUTABLE_OUTPUT_PATH "../bin")
include_directories("include" "include/v8" "include/libuv")
link_libraries(
    ${CMAKE_SOURCE_DIR}/lib/libuv.a
    ${CMAKE_SOURCE_DIR}/lib/libv8_monolith.a
)
add_executable(slim ${SOURCE})
