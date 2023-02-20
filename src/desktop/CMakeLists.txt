find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtkmm-4.0)

message(STATUS "GTK_INCLUDE_DIRS: ${GTK_INCLUDE_DIRS}")
message(STATUS "GTK_LIBRARIES: ${GTK_LIBRARIES}")

add_executable(hello 
	main.cpp 
	helloworld.cpp
	Sidebar.cpp
	)
	
message(STATUS ${CMAKE_SOURCE_DIR}/include)
target_include_directories(hello PRIVATE ${CMAKE_SOURCE_DIR}/include ${GTK_INCLUDE_DIRS})
target_link_libraries(hello PRIVATE ${GTK_LIBRARIES} libromset jsoncpp)