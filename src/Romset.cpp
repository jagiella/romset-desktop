#include "Romset.hpp"
#include <iostream>
#include <filesystem>
#include <tinyxml2.h>

Rom::Rom(const char *name, const char *filename, int size, const char *crc,
		const char *md5) :
		m_name(name), m_filename(filename), m_size(size), m_crc(crc), m_md5(md5) {

}

std::ostream& operator<<(std::ostream &stream, const Rom &rom) {
	return stream << "Rom(" << rom.name() << ", size=" << rom.size() << ")";
}


Romset::Romset(std::string filename, std::string directory) :
		m_filename(filename), m_directory(directory) {
	std::cout << "Create romset: " << m_filename << std::endl;

	tinyxml2::XMLDocument doc;
	doc.LoadFile((const char*) m_filename.c_str());

	// header
	auto header = doc.FirstChildElement("datafile")->FirstChildElement(
			"header");
	const char *name = header->FirstChildElement("name")->GetText();
	std::cout << "-> name: " << name << std::endl;

	// games
	auto game = doc.FirstChildElement("datafile")->FirstChildElement("game");
	while (game != 0) {
		// game
		const char *name = game->Attribute("name");

		// rom file
		auto e_rom = game->FirstChildElement("rom");
		const char *filename = e_rom->Attribute("name");
		int size = atoi(e_rom->Attribute("size"));
		const char *crc = e_rom->Attribute("crc");
		const char *md5 = e_rom->Attribute("md5");

		//m_roms.emplace_back(name, filename, size, crc, md5);
		std::cout << "-> add: " << std::endl;
		//m_roms[md5] = Rom(name, filename, size, crc, md5);
		m_roms.insert(std::make_pair<std::string, Rom>(md5, { name, filename,
				size, crc, md5 }));
		//std::cout << "-> rom: " << m_roms.back() << std::endl;
		game = game->NextSiblingElement("game");
	}
}

Romset::~Romset() {
	std::cout << "Destroy romset: " << m_filename << std::endl;
}

const std::string Romset::name() {

	return m_filename;
}

std::unordered_map<std::string, Rom> Romset::roms() {
	return m_roms;
}

bool Romset::contains(Rom *other) {
	return m_roms.find(other->md5()) != m_roms.end();
}

void Romset::import(std::string filepath, Rom *other){
	auto match = m_roms.find(other->md5());
	if(match != m_roms.end()){
		std::filesystem::create_directories(m_directory);
		std::filesystem::rename(filepath, m_directory + match->second.filename());
	}
}
