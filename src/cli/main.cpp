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

void copy_rom(std::filesystem::path src, std::filesystem::path dst) {
	if (!std::filesystem::exists(dst.parent_path())) {
		std::cout << GREEN << "create path: " << END << dst.parent_path()
				<< std::endl;
		std::filesystem::create_directories(dst.parent_path());
	}

	const auto modify_time = std::filesystem::last_write_time(src);
	std::cout << GREEN << "copy rom: " << END << dst << std::endl;
	try {
		std::filesystem::copy(src, dst,
				std::filesystem::copy_options::update_existing);
	} catch (std::filesystem::filesystem_error const &ex) {
		std::cerr << "error while copying: " << ex.what() << std::endl;
		return;
	}

	try {
		std::filesystem::last_write_time(dst, modify_time);
	} catch (std::filesystem::filesystem_error const &ex) {
		std::cerr << "error while setting write time: " << ex.what()
				<< std::endl;
		return;
	}
}

void remove_rom(std::filesystem::path src) {
	std::cout << "remove rom: " << src << std::endl;
	try {
		std::filesystem::remove(src);
	} catch (std::filesystem::filesystem_error const &ex) {
		std::cerr << "error while removing: " << ex.what() << std::endl;
		return;
	}
}

void move_rom(std::filesystem::path src, std::filesystem::path dst) {
	//std::cout << "found " << p << " in " << romset->name() << std::endl;

	if (!std::filesystem::exists(dst.parent_path())) {
		std::cout << GREEN << "create path: " << END << dst.parent_path()
				<< std::endl;
		std::filesystem::create_directories(dst.parent_path());
	}

	std::cout << GREEN << "move rom: " << END << dst << std::endl;
	std::error_code ec;
	std::filesystem::rename(src, dst, ec);
	if (ec.value() == 18) {
		copy_rom(src, dst);
		remove_rom(src);
	} else if (ec.value()) {
		std::cout << "code().value():    " << ec.value() << '\n'
				<< "code().message():  " << ec.message() << '\n'
				<< "code().category(): " << ec.category().name() << '\n';
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
					[target_directory](std::filesystem::path src_path,
							std::list<Match> matches) {
						//matches.back().second.

						auto last = --matches.end();
						for (auto match =matches.begin(); match!=matches.end(); match++) {
							std::filesystem::path dst_path;
							auto romset = match->first;
							auto rom = match->second;
							auto game = rom->game();

							if (game->roms().size() == 1)
								dst_path = target_directory / romset->name()
										/ rom->filename();
							else if (game->roms().size() > 1)
								dst_path = target_directory / romset->name()
										/ game->name() / rom->filename();

							if(match == last){
								move_rom(src_path, dst_path);
							}else{
								copy_rom(src_path, dst_path);
							}
						}
						//remove_rom(src_path);
						/*if (matches.size() == 1) {
						 move_rom(p,
						 target_directory
						 / matches.back().first->name()
						 / matches.back().second->filename());
						 } else {
						 for (auto match : matches) {
						 copy_rom(p,
						 target_directory / match.first->name()
						 / match.second->filename());
						 }
						 remove_rom(p);
						 }*/
						//std::cout << "found " << p << " in " << romset->name()
						//<< std::endl;
						//if (target_directory != "")
						//move_rom(p, target_directory / romset->name() / rom.filename());
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
