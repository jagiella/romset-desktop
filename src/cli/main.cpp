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
#include "Requests.hpp"

typedef struct {
	char key;
	const char* option;
} MenuItem;


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

	MenuItem menu[] = {
			{'1', "list"},
			{'2', "update"},
			{'3', "download (updatable)"},
			{'4', "download (missing)"},
			{'q', "exit"},
	};

	bool _exit = false;
	while (not _exit) {
		std::cout << "Options:" << std::endl;
		for(auto item : menu)
			std::cout << item.key << ") " << item.option << std::endl;

		std::string s;

		// Take input using cin
		std::cin >> s;

		switch (s.c_str()[0]) {
		case '1':
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

		case '2':
			std::cout << "update" << std::endl;
			catalogue.update();
			break;

		case '3':
			std::cout << "list" << std::endl;
			Requests::download("http://redump.org/datfile/ps2/", "/home/nick/Downloads/test.zip");
			/*for(auto &dat : collection){
				auto dat_online = catalogue.find(dat.name());
				if(dat_online){
					//std::cout << dat.name() << ": " << dat.version() << " -[update]-> "<< dat_online->version() << std::endl;
					catalogue.download(dat_online, "/home/nick/Downloads/test.zip");
				}
			}*/
			break;

		case 'q':
			_exit = true;
			break;

		default:
			std::cout << "no valid option" << std::endl;
			break;
		}
	}
	return 0;
}
