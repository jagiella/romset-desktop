find_package(PkgConfig REQUIRED)
pkg_check_modules(TCLAP REQUIRED tclap)

add_executable(romset-cli 
	main.cpp 
	)
	
target_include_directories(romset-cli PRIVATE ${CMAKE_SOURCE_DIR}/include  ${TCLAP_INCLUDE_DIRS})
target_link_libraries(romset-cli PRIVATE romset )
set_property(TARGET romset-cli PROPERTY CXX_STANDARD 20)

install(TARGETS romset-cli
        RUNTIME DESTINATION bin)