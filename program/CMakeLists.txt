cmake_minimum_required(VERSION 3.0.0)
SET( CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INSTALL_MESSAGE LAZY)
project(Slim VERSION 0.0.0)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-as-needed -ldl -pthread -std=c++2a")
add_compile_options(-DV8_COMPRESS_POINTERS)
set(EXECUTABLE_OUTPUT_PATH "bin")
include_directories(
    ${CMAKE_SOURCE_DIR}../third_party_slim/libuv/include
    ${CMAKE_SOURCE_DIR}../third_party_slim/google/v8/include
)
link_libraries(
    ${CMAKE_SOURCE_DIR}/lib/libuv.a
    ${CMAKE_SOURCE_DIR}/lib/libv8_monolith.a
)
add_executable(slim main.cpp)
