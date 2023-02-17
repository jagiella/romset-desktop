/*
 * RomFile.hpp
 *
 *  Created on: 17.02.2023
 *      Author: nickj
 */

#ifndef SRC_ROMFILE_HPP_
#define SRC_ROMFILE_HPP_

#include <string>

#include "Romset.hpp"

class RomFile: public Rom {
private:
	const char* m_path;
public:
	RomFile( const char *path);
	int size() const;
	std::string md5();
	std::string path() {return m_path;};
};



#endif /* SRC_ROMFILE_HPP_ */
