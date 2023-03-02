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

class Response {
private:
	std::string m_text;
public:
	Response(std::string& text);
	std::string text(){return m_text;}
};

class Requests {
public:
	static Response get(std::string url);
	static std::string header(std::string url);

};

class RomsetCatalogueNointro {
private:
	std::string m_url;
	RomsetCollection m_romsets;
public:
	RomsetCatalogueNointro(std::string url = "/home/nickj/Downloads/redump.org.html");
	void update();
	Romset* find(std::string romset_name);
};

#endif /* SRC_LIB_CATALOGUE_HPP_ */
