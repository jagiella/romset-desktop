find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)
pkg_check_modules(TINYXML2 REQUIRED tinyxml2)

add_library(libromset SHARED
	Romset.cpp
	RomFile.cpp)

target_include_directories(libromset PRIVATE ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIRS} )
target_link_libraries(libromset PRIVATE ${OPENSSL_LIBRARIES} ${TINYXML2_LIBRARIES})