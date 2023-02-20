project(hello-world C CXX)

cmake_minimum_required(VERSION 3.25)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -O0 -ggdb")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -g")

add_subdirectory(src)