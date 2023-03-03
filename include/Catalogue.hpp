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


class RomsetCatalogueNointro {
private:
	std::string m_url;
	RomsetCollection m_romsets;
public:
	RomsetCatalogueNointro(std::string url = "/home/nickj/Downloads/redump.org.html");
	void update();
	Romset* find(std::string romset_name);
	void download(Romset* romset, std::string path);
};

#endif /* SRC_LIB_CATALOGUE_HPP_ */
