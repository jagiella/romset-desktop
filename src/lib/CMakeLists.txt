find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)
pkg_check_modules(TINYXML2 REQUIRED tinyxml2)
#pkg_check_modules(LIBXMLXX REQUIRED libxml++-2.6)
pkg_check_modules(GUMBO REQUIRED gumbo)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(ZIP REQUIRED libzip)

add_library(romset SHARED
	Romset.cpp
	RomFile.cpp
	Requests.cpp
	Catalogue.cpp
	#XML.cpp
	)

target_include_directories(romset PRIVATE ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIRS} ${TINYXML2_INCLUDE_DIRS} ${GUMBO_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${ZIP_INCLUDE_DIRS})
target_link_libraries(romset PRIVATE ${OPENSSL_LIBRARIES} ${TINYXML2_LIBRARIES} ${GUMBO_LIBRARIES} ${CURL_LIBRARIES} ${ZIP_LIBRARIES})

set_target_properties(romset PROPERTIES
	CXX_STANDARD 20
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS romset
        LIBRARY DESTINATION lib)