find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtkmm-4.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

add_executable(hello 
	main.cpp 
	helloworld.cpp
	Sidebar.cpp
	)
	
target_include_directories(hello PRIVATE ${CMAKE_SOURCE_DIR}/include ${GTK_INCLUDE_DIRS})
target_link_libraries(hello PRIVATE ${GTK_LIBRARIES} ${JSONCPP_LIBRARIES} libromset )