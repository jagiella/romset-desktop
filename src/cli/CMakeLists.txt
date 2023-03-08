find_package(PkgConfig REQUIRED)

add_executable(romset-cli 
	main.cpp 
	)
	
target_include_directories(romset-cli PRIVATE ${CMAKE_SOURCE_DIR}/include )
target_link_libraries(romset-cli PRIVATE romset )