#include "Romset.hpp"
#include "RomFile.hpp"
#include <iostream>
#include <filesystem>
#include <tinyxml2.h>

Rom::Rom(std::shared_ptr<Game> game, const char *filename, int size,
		const char *crc, const char *md5) :
		m_game(game), m_filename(filename), m_size(size), m_crc(crc), m_md5(md5) {

}

std::ostream& operator<<(std::ostream &stream, const Rom &rom) {
	return stream << "Rom(" << rom.filename() << ", size=" << rom.size() << ")";
}

Romset::Romset(std::string name) :
		m_name(name) {
	std::cout << "Create romset: " << std::endl;
	std::cout << "-> name: " << name << std::endl;
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
	m_name = name;

	const char *version = header->FirstChildElement("version")->GetText();
	std::cout << "-> version: " << version << std::endl;
	m_version = version;

	// games
	auto e_datfile = doc.FirstChildElement("datafile");
	for (auto e_game = e_datfile->FirstChildElement("game"); e_game != 0; e_game =
			e_game->NextSiblingElement("game")) {
		// game
		const char *name = e_game->Attribute("name");

		std::shared_ptr<Game> p_game = std::make_shared<Game>(name);

		for (auto e_rom = e_game->FirstChildElement("rom"); e_rom != 0; e_rom =
				e_rom->NextSiblingElement("rom")) {

			// rom file
			const char *filename = e_rom->Attribute("name");
			const char *size = e_rom->Attribute("size");
			const char *crc = e_rom->Attribute("crc");
			const char *md5 = e_rom->Attribute("md5");
			if (size and crc and md5) {
				//std::cout << "-> add: " << filename << std::endl;
				m_roms.insert(std::make_pair<std::string, Rom>(md5, { p_game,
						filename, atoi(size), crc, md5 }));
			} else {
				std::cout << "-> skip: " << filename << std::endl;
			}
		}
	}
	std::cout << "-> added: " << m_roms.size() << " games" << std::endl;
}

Romset::~Romset() {
	std::cout << "Destroy romset: " << m_filename << std::endl;
}

const std::string Romset::name() {

	return m_name;
}
const std::string Romset::version() {
	//std::cout << m_version << std::endl;
	return m_version;
}

void Romset::set_version(std::string version) {

	m_version = version;
	//std::cout << "-> version: " << version << std::endl;
}

const std::string Romset::url() {
	//std::cout << m_url << std::endl;
	return m_url;
}

void Romset::set_url(std::string url) {

	m_url = url;
	//std::cout << "-> url: " << url << std::endl;
}

std::unordered_map<std::string, Rom> Romset::roms() {
	return m_roms;
}

bool Romset::contains(Rom *other) {
	return m_roms.find(other->md5()) != m_roms.end();
}

void Romset::import(std::string filepath, Rom *other) {
	auto match = m_roms.find(other->md5());
	if (match != m_roms.end()) {
		std::filesystem::create_directories(m_directory);
		std::filesystem::rename(filepath,
				m_directory + match->second.filename());
	}
}

void RomsetCollection::scan(std::filesystem::path path) {
	for (auto file : std::filesystem::directory_iterator(path)) {
		std::cout << "analyze: " << file << std::endl;
		RomFile rom(file.path().c_str());

		for (auto set : m_romsets) {
			if (set->contains(&rom)) {
				std::cout << "found!" << std::endl;
			}
		}
	}
}
