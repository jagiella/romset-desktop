find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtkmm-4.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

add_executable(romset-desktop 
	main.cpp 
	MainWindow.cpp
	Sidebar.cpp
	Headerbar.cpp
	)
	
target_include_directories(romset-desktop PRIVATE ${CMAKE_SOURCE_DIR}/include ${GTK_INCLUDE_DIRS})
target_link_libraries(romset-desktop PRIVATE ${GTK_LIBRARIES} ${JSONCPP_LIBRARIES} romset )
set_property(TARGET romset-desktop PROPERTY CXX_STANDARD 20)
