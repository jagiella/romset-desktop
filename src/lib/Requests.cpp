/*
 * Requests.cpp
 *
 *  Created on: 02.03.2023
 *      Author: nick
 */

#include "Requests.hpp"

#include <ostream>
#include <iostream>
#include <fstream>
#include <curl/curl.h>


static int writer(char *data, size_t size, size_t nmemb,
		std::string *writerData) {
	if (writerData == NULL)
		return 0;

	writerData->append(data, size * nmemb);

	return size * nmemb;
}

static size_t header_callback(char *buffer, size_t size, size_t nitems,
		std::string *filename) {
	/* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
	/* 'userdata' is set with CURLOPT_HEADERDATA */

	std::string s(buffer, nitems);
	if (s.rfind("Content-Disposition", 0) == 0) { // pos=0 limits the search to the prefix
		// s starts with prefix
		//std::cout << "GOT HEADER LINE: " << s << std::endl;
		int pos = 43;
		int n = s.size() - pos - 3;
		filename[0] = s.substr(pos, n);
		//std::cout << filename[0] << std::endl;
	}

	return nitems * size;
}

Response::Response(std::string &text) :
		m_text(text) {
}

Response Requests::get(std::string url) {
	//CURL *curl;
	char errorBuffer[CURL_ERROR_SIZE];
	std::string buffer;
	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	CURLcode code;

	CURL *conn = curl_easy_init();

	if (conn == NULL) {
		fprintf(stderr, "Failed to create CURL connection\n");
		exit(EXIT_FAILURE);
	}

	code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
	if (code != CURLE_OK) {
		fprintf(stderr, "Failed to set error buffer [%d]\n", code);
		//return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
	if (code != CURLE_OK) {
		fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
		//return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 0L);
	if (code != CURLE_OK) {
		fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
		//return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
	if (code != CURLE_OK) {
		fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
		//return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
	if (code != CURLE_OK) {
		fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
		//return false;
	}

	// Retrieve content for the URL

	code = curl_easy_perform(conn);
	curl_easy_cleanup(conn);

	if (code != CURLE_OK) {
		fprintf(stderr, "Failed to get '%s' [%s]\n", url.c_str(), errorBuffer);
		exit(EXIT_FAILURE);
	}

	curl_global_cleanup();

	return Response(buffer);
}
std::string Requests::header(std::string url) {
	CURL *curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
		std::string filename = "";
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &filename);

		curl_easy_perform(curl);
		std::cout << "FILENAME: " << filename << std::endl;
		return filename;
	}
}

size_t write_data(const char *ptr, size_t size, size_t nmemb,
		std::ofstream *stream) {
	std::cout << "write chunk: " << size * nmemb << std::endl;
	stream->write(ptr, size * nmemb);
	return size * nmemb;
}

void Requests::download(const std::string url, std::ofstream &out) {
	CURL *curl = curl_easy_init();
	if (curl) {
		//std::ofstream out(filename, std::ios::out | std::ios::binary);
		//fp = fopen(filename, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
		curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		//fclose(fp);
	}

	//int main()
	/*{
		//Open the ZIP archive
		int err = 0;
		zip *z = zip_open(filename.c_str(), 0, &err);
		int num_files = zip_get_num_entries(z, ZIP_FL_UNCHANGED);
		std::cout << "contains files: " << num_files << std::endl;

		for (int i = 0; i < num_files; i++) {

			//Search for the file of given name
			//const char *name =
			//		"Sony - PlayStation 2 - Datfile (11091) (2023-03-03 00-34-31).dat";
			struct zip_stat st;
			zip_stat_init(&st);
			zip_stat_index(z, i, 0, &st);
			const char *name = zip_get_name(z, i, ZIP_FL_ENC_RAW);
			std::cout << "contains file: " << name << std::endl;
			std::ofstream out(std::string(name), std::ios::out | std::ios::binary);
			//zip_stat(z, name, 0, &st);

			//Alloc memory for its uncompressed contents
			char *contents = new char[st.size];

			//Read the compressed file
			//zip_file *f = zip_fopen(z, name, 0);
			zip_file *f = zip_fopen_index(z, i, 0);
			zip_fread(f, contents, st.size);
			zip_fclose(f);

			//And close the archive
			zip_close(z);

			//Do something with the contents

			out.write(contents, st.size);
			//delete allocated memory
			delete[] contents;
		}
	}*/
}
