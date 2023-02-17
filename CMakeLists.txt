project(hello-world C CXX)

cmake_minimum_required(VERSION 3.25)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtkmm-4.0)
pkg_search_module(OPENSSL REQUIRED openssl)

message(STATUS "GTK_INCLUDE_DIRS: ${GTK_INCLUDE_DIRS}")
message(STATUS "GTK_LIBRARIES: ${GTK_LIBRARIES}")
message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")

add_executable(hello 
	src/main.cpp 
	src/helloworld.cpp
	src/Romset.cpp
	src/RomFile.cpp
	)
target_include_directories(hello PRIVATE ${GTK_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIRS})
target_link_libraries(hello PRIVATE ${GTK_LIBRARIES} tinyxml2  ${OPENSSL_LIBRARIES})