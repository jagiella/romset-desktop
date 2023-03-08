find_package(PkgConfig REQUIRED)
pkg_check_modules(TCLAP REQUIRED tclap)

add_executable(romset-cli 
	main.cpp 
	)
	
target_include_directories(romset-cli PRIVATE ${CMAKE_SOURCE_DIR}/include  ${TCLAP_INCLUDE_DIRS})
target_link_libraries(romset-cli PRIVATE romset )