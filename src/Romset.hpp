/*
 * Romset.hpp
 *
 *  Created on: 17.02.2023
 *      Author: nickj
 */

#ifndef SRC_ROMSET_HPP_
#define SRC_ROMSET_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>

class Rom {
private:
	friend std::ostream& operator<<(std::ostream &strm, const Rom &rom);
protected:
	std::string m_name, m_filename, m_crc, m_md5;
	int m_size;
public:
	Rom(const char *name = "unnamed", const char *filename = 0, int size = 0,
			const char *crc = 0, const char *md5 = 0);
	virtual std::string md5() {
		return m_md5;
	}
	virtual int size() const {
		return m_size;
	}
	virtual std::string name() const {
		return m_name;
	}
	virtual std::string filename() const {
		return m_filename;
	}

};

//std::ostream& operator<<(std::ostream &strm, const Rom &rom)

class Romset {
private:
	std::string m_filename;
	std::string m_directory;
	std::unordered_map<std::string, Rom> m_roms;

public:
	Romset(std::string filename, std::string directory);
	~Romset();

	const std::string name();
	std::unordered_map<std::string, Rom> roms();
	bool contains(Rom *rom);
	void import(std::string filepath, Rom *rom);
};

#endif /* SRC_ROMSET_HPP_ */
