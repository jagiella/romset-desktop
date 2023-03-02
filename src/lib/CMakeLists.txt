find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)
pkg_check_modules(TINYXML2 REQUIRED tinyxml2)
pkg_check_modules(LIBXMLXX REQUIRED libxml++-2.6)
pkg_check_modules(GUMBO REQUIRED gumbo)
pkg_check_modules(CURL REQUIRED libcurl)

add_library(libromset SHARED
	Romset.cpp
	RomFile.cpp
	Catalogue.cpp
	#XML.cpp
	)

target_include_directories(libromset PRIVATE ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIRS} ${LIBXMLXX_INCLUDE_DIRS} )
target_link_libraries(libromset PRIVATE ${OPENSSL_LIBRARIES} ${TINYXML2_LIBRARIES} ${LIBXMLXX_LIBRARIES} ${GUMBO_LIBRARIES}  ${CURL_LIBRARIES})