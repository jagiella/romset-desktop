/*
 * main.cpp
 *
 *  Created on: 01.03.2023
 *      Author: nick
 */

#include <iostream>
#include <ostream>
#include <filesystem>
#include <stdlib.h>
#include <tclap/CmdLine.h>

#include "Catalogue.hpp"
#include "Requests.hpp"

typedef struct {
	char key;
	const char *option;
} MenuItem;

#define GREEN "\x1b[32m"
#define END   "\x1b[0m"
#define CHECK "\u2713"

void move_rom(std::filesystem::path p, std::filesystem::path destdir) {
	//std::cout << "found " << p << " in " << romset->name() << std::endl;

	std::cout << "create path: " << destdir << std::endl;
	std::filesystem::create_directories(destdir);

	std::cout << "move rom: " << p << std::endl;
	std::error_code ec;
	std::filesystem::rename(p, destdir, ec);
	if (ec.value() == 18) {
		/*std::cout << "code().value():    " << ec.value() << '\n'
		 << "code().message():  " << ec.message() << '\n'
		 << "code().category(): " << ec.category().name() << '\n';*/
		const auto modify_time = std::filesystem::last_write_time(p);
		std::cout << "copy rom: " << p << std::endl;
		try {
			std::filesystem::copy(p, destdir,
					std::filesystem::copy_options::update_existing);
		} catch (std::filesystem::filesystem_error const &ex) {
			std::cerr << "error while copying: " << ex.what() << std::endl;
			return;
		}

		try {
			std::filesystem::path p_new_location = destdir / p.filename();
			std::filesystem::last_write_time(p_new_location, modify_time);
		} catch(std::filesystem::filesystem_error const& ex) {
			std::cerr << "error while setting write time: " << ex.what() << std::endl;
			return;
		}

		std::cout << "remove rom: " << p << std::endl;
		try {
			std::filesystem::remove(p);
		} catch (std::filesystem::filesystem_error const &ex) {
			std::cerr << "error while removing: " << ex.what() << std::endl;
			return;
		}
	}
}

int main(int argc, char **argv) {
	std::cout << GREEN << "hello world" << CHECK << END << std::endl;

	TCLAP::CmdLine cmd("Command description message", ' ', "0.9");
	TCLAP::UnlabeledValueArg<std::string> scan_directoryArg("scandir",
			"scan directory", true,
			"/media/nick/7C3E14583E140E30/Games/Consoles/ps1/Theme Hospital (Europe) (En,Fr,De,Es,It,Sv)",
			"SCAN_PATH");
	TCLAP::UnlabeledValueArg<std::string> target_directoryArg("targetdir",
			"target directory", false, "", "TARGET_PATH");
	cmd.add(scan_directoryArg);
	cmd.add(target_directoryArg);
	cmd.parse(argc, argv);

	std::filesystem::path scan_directory = scan_directoryArg.getValue();
	std::cout << "scan_directory: " << scan_directory << std::endl;
	std::filesystem::path target_directory = target_directoryArg.getValue();
	std::cout << "target_directory: " << target_directory << std::endl;

	RomsetCatalogueNointro catalogue;
	RomsetCollection collection;

	std::filesystem::path home = getenv("HOME");
	std::filesystem::path path = home / ".config" / "romset";
	std::filesystem::create_directories(path);

	for (auto datfile : std::filesystem::directory_iterator(path)) {
		if (datfile.path().extension() == ".dat") {
			std::cout << "Found:" << datfile << std::endl;
			collection.add(path / datfile, "");
		}
	}

	MenuItem menu[] = { { '1', "list" }, { '2', "update" }, { '3',
			"download (updatable)" }, { '4', "download (missing)" }, { '5',
			"scan" }, { 'q', "exit" }, };

	bool _exit = false;
	while (not _exit) {
		std::cout << "Options:" << std::endl;
		for (auto item : menu)
			std::cout << item.key << ") " << item.option << std::endl;

		std::string s;

		// Take input using cin
		std::cin >> s;

		switch (s.c_str()[0]) {
		case '1':
			std::cout << "list" << std::endl;
			for (auto &dat : collection) {
				auto dat_online = catalogue.find(dat->name());
				if (dat_online) {
					if (dat->version() != dat_online->version()) {
						std::cout << dat->name() << ": " << dat->version()
								<< " -[update]-> " << dat_online->version()
								<< std::endl;
					} else {
						std::cout << dat->name() << ": " << dat->version()
								<< " -[up-to-date] " << std::endl;
					}
				} else
					std::cout << dat->name() << ": " << dat->version()
							<< std::endl;
			}
			break;

		case '2':
			std::cout << "update" << std::endl;
			catalogue.update();
			break;

		case '3': {
			std::cout << "list" << std::endl;
			/*{
			 Romset tmprs("Dummy");
			 tmprs.set_url("http://redump.org/datfile/ps2/");
			 catalogue.download(&tmprs, path);
			 }*/

			std::list<std::filesystem::path> new_files;
			for (auto dat = collection.begin(); dat != collection.end();) {
				//for (auto dat : collection) {
				auto dat_online = catalogue.find((*dat)->name());
				if (dat_online and (*dat)->version() != dat_online->version()) {
					std::cout << (*dat)->name() << ": " << (*dat)->version()
							<< " -[update]-> " << dat_online->version()
							<< std::endl;
					std::filesystem::path filename = catalogue.download(
							dat_online, path);
					std::cout << "remove old" << std::endl;
					std::filesystem::remove((*dat)->filename());
					dat = collection.erase(dat);
					std::cout << "add new" << std::endl;
					//collection.add(filename, "");
					new_files.push_back(filename);
					std::cout << "done" << std::endl;
				} else {
					dat++;
				}
			}

			for (auto filename : new_files) {
				collection.add(filename, "");
			}
		}
			break;

		case '4': {
			std::cout << "list" << std::endl;
			/*{
			 Romset tmprs("Dummy");
			 tmprs.set_url("http://redump.org/datfile/ps2/");
			 catalogue.download(&tmprs, path);
			 }*/

			std::list<std::filesystem::path> new_files;
			for (auto dat_online : catalogue) {
				//for (auto dat : collection) {
				auto dat = collection.find(dat_online->name());
				if (dat == 0) {
					std::cout << " -[download]-> " << dat_online->version()
							<< std::endl;
					std::filesystem::path filename = catalogue.download(
							dat_online, path);
					new_files.push_back(filename);
					std::cout << "done" << std::endl;
				}
			}

			for (auto filename : new_files) {
				collection.add(filename, "");
			}
		}
			break;

		case '5': {
			std::cout << "scan: " << scan_directory << std::endl;
			collection.scan(scan_directory,
					[target_directory](std::filesystem::path p,
							std::shared_ptr<Romset> romset) {
						std::cout << "found " << p << " in " << romset->name()
								<< std::endl;
						move_rom(p, target_directory / romset->name());

					});
		}
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
