cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0048 NEW)


project(romset
	VERSION 0.1.0
	LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -O0 -ggdb")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -g")

add_subdirectory(src)