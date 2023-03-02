/*
 * main.cpp
 *
 *  Created on: 01.03.2023
 *      Author: nick
 */

#include <iostream>
#include <ostream>
#include <filesystem>

#include "Catalogue.hpp"

int main(int argc, char **argv) {
	std::cout << "hello world" << std::endl;

	RomsetCatalogueNointro catalogue;
	RomsetCollection collection;

	std::filesystem::path path = "/home/nick/Downloads/";
	for (auto datfile : std::filesystem::directory_iterator(path)) {
		if (datfile.path().extension() == ".dat") {
			std::cout << "Found:" << datfile << std::endl;
			collection.add(path / datfile, "");
		}
	}

	bool _exit = false;
	while (not _exit) {
		std::cout << "Options:" << std::endl << "1) list" << std::endl
				<< "2) exit" << std::endl;

		std::string s;

		// Take input using cin
		std::cin >> s;

		switch (std::atoi(s.c_str())) {
		case 1:
			std::cout << "update" << std::endl;
			catalogue.update();
			break;
		case 2:
			std::cout << "list" << std::endl;
			for(auto &dat : collection){
				auto dat_online = catalogue.find(dat.name());
				if(dat_online){
					std::cout << dat.name() << ": " << dat.version() << " -[update]-> "<< dat_online->version() << std::endl;
				}
				else
					std::cout << dat.name() << ": " << dat.version() << std::endl;
			}
			break;
		case 3:
			_exit = true;
			break;
		default:
			std::cout << "no valid option" << std::endl;
			break;
		}
	}
	return 0;
}
