/*
 * Catalogue.cpp
 *
 *  Created on: 22.02.2023
 *      Author: nickj
 */

#include "Catalogue.hpp"
#include "XML.hpp"

#include <curl/curl.h>
#include <ostream>
#include <iostream>
#include <fstream>

#include <tinyxml2.h>
#include <libxml++/libxml++.h>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <gumbo.h>

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
;

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

	code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
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

RomsetCatalogueNointro::RomsetCatalogueNointro(std::string url =
		"/home/nickj/Downloads/redump.org.html") :
		m_url(url) {
	update();
}

GumboNode* findFirstChild(GumboNode *node, GumboTag tag, const char *id = 0) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return 0;
	}

	auto children = node->v.element.children;
	//std::cout << "has children: " << children.length << std::endl;
	for (int i = 0; i < children.length; i++) {
		auto child = static_cast<GumboNode*>(children.data[i]);
		//std::cout << "-> child: "
		//		<< gumbo_normalized_tagname(child->v.element.tag) << std::endl;
		if (child->v.element.tag == tag) {
			if (id == 0)
				return child;
			else {
				//std::cout << "look for attribute " << std::endl;
				GumboAttribute *attr = gumbo_get_attribute(
						&child->v.element.attributes, "id");
				if (attr != 0 and strstr(attr->value, id) != NULL) {
					return child;
				}
			}
		}
	}
	return 0;
}
void printChildren(GumboNode *node, int indent = 0) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}
	for (int i = 0; i < indent; i++)
		std::cout << " ";
	std::cout << gumbo_normalized_tagname(node->v.element.tag) << std::endl;

	auto children = &node->v.element.children;
	for (int i = 0; i < children->length; i++) {
		GumboNode *child = static_cast<GumboNode*>(children->data[i]);
		/*std::cout << gumbo_normalized_tagname(node->v.element.tag)
		 << "-> child: "
		 << gumbo_normalized_tagname(child->v.element.tag) << std::endl;*/
		printChildren(child, indent + 1);
	}
}

void RomsetCatalogueNointro::update() {
	std::ifstream in("/home/nickj/Downloads/redump.org.html",
			std::ios::in | std::ios::binary);
	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();

	auto options = kGumboDefaultOptions;
	GumboOutput *output = gumbo_parse_with_options(&options, contents.data(),
			contents.length());

	auto root = output->root;
	std::cout << gumbo_normalized_tagname(root->v.element.tag) << std::endl;
	auto body = findFirstChild(root, GUMBO_TAG_BODY);
	std::cout << gumbo_normalized_tagname(body->v.element.tag) << std::endl;
	auto div = findFirstChild(body, GUMBO_TAG_DIV, "main");
	std::cout << gumbo_normalized_tagname(div->v.element.tag) << std::endl;
	auto table = findFirstChild(div, GUMBO_TAG_TABLE);
	std::cout << gumbo_normalized_tagname(table->v.element.tag) << std::endl;
	auto tbody = findFirstChild(table, GUMBO_TAG_TBODY);
	std::cout << gumbo_normalized_tagname(tbody->v.element.tag) << std::endl;

	auto trs = &tbody->v.element.children;
	for (int i = 1; i < trs->length; i++) {
		GumboNode *tr = static_cast<GumboNode*>(trs->data[i]);

		// name
		GumboNode *name =
				static_cast<GumboNode*>(tr->v.element.children.data[0]);
		GumboNode *text =
				static_cast<GumboNode*>(name->v.element.children.data[0]);
		std::cout << text->v.text.text << std::endl;

		// url
		GumboNode *url = static_cast<GumboNode*>(tr->v.element.children.data[2]);
		if (url->v.element.children.length > 0) {
			GumboNode *a =
					static_cast<GumboNode*>(url->v.element.children.data[0]);
			GumboAttribute *attr = gumbo_get_attribute(&a->v.element.attributes,
					"href");
			std::cout << attr->value << std::endl;

			auto filename = Requests::header(attr->value);

			std::string name="", version="";

			std::string::size_type pos = filename.find(" - Datfile");
			if (pos != std::string::npos)
				name = filename.substr(0, pos);

			auto pos1 = filename.find_last_of('(')+1;
			auto pos2 = filename.find_last_of(')');
			version = filename.substr(pos1, pos2-pos1);

			m_romsets.add(name);
			m_romsets.back().set_version(version);
		}
	}
}
