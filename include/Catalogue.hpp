/*
 * Catalogue.hpp
 *
 *  Created on: 22.02.2023
 *      Author: nickj
 */

#ifndef SRC_LIB_CATALOGUE_HPP_
#define SRC_LIB_CATALOGUE_HPP_

#include "Romset.hpp"

#include <string>
#include <filesystem>


class RomsetCatalogueNointro {
private:
	std::string m_url;
	RomsetCollection m_romsets;
public:
	RomsetCatalogueNointro(std::string url = "/home/nickj/Downloads/redump.org.html");
	void update();
	std::shared_ptr<Romset> find(std::string romset_name);
	std::filesystem::path download(std::shared_ptr<Romset> romset, std::filesystem::path path);
	auto begin() {return m_romsets.begin();}
	auto end() {return m_romsets.end();}
};

#endif /* SRC_LIB_CATALOGUE_HPP_ */
