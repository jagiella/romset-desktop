/*
 * Requests.hpp
 *
 *  Created on: 02.03.2023
 *      Author: nick
 */

#ifndef INCLUDE_REQUESTS_HPP_
#define INCLUDE_REQUESTS_HPP_

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
	static std::string header(const std::string url);
	static void download(const std::string url, std::ofstream &out);
};




#endif /* INCLUDE_REQUESTS_HPP_ */
