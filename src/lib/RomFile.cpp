#include "RomFile.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <openssl/evp.h>

RomFile::RomFile(const char *path) :
		Rom(std::shared_ptr<Game>(), "", 0, "", ""), m_path(path) {

}

int RomFile::size() const {
	return std::filesystem::file_size(m_path);
}

std::string RomFile::md5() {

	if (m_md5 == "") {
		const EVP_MD *evp_md = EVP_md5();
		EVP_MD_CTX *ctx = EVP_MD_CTX_new();
		EVP_MD_CTX_init(ctx);
		EVP_DigestInit_ex(ctx, evp_md, NULL);

		std::ifstream fin(m_path, std::ios::binary);
		char buffer[1024];
		while (!fin.eof()) {
			fin.read(buffer, sizeof(buffer));
			std::streamsize s = fin.gcount();

			///do with buffer
			EVP_DigestUpdate(ctx, buffer, fin.gcount());
		}

		unsigned int hash_length = 0;
		unsigned char hash[EVP_MAX_MD_SIZE];
		EVP_DigestFinal_ex(ctx, hash, &hash_length);
		EVP_MD_CTX_destroy(ctx);

		//std::string hash_str = "";
		std::stringstream hash_str;
		hash_str << std::hex;
		for (auto i = 0u; i < hash_length; ++i) {
			//std::cout << std::hex << std::setw(2) << std::setfill('0') <<
			//    (unsigned int) hash[i];
			hash_str << std::setw(2) << std::setfill('0')
					<< (unsigned int) hash[i];
		}
		m_md5 = hash_str.str();
	}

	return m_md5;
	//return std::string(hash, hash_length);
}
