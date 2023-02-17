find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")

add_library(libromset SHARED
	Romset.cpp
	RomFile.cpp)

target_include_directories(libromset PRIVATE ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIRS} )
target_link_libraries(libromset PRIVATE ${OPENSSL_LIBRARIES} tinyxml2)