/*
 * Catalogue.cpp
 *
 *  Created on: 22.02.2023
 *      Author: nickj
 */

#include "Catalogue.hpp"
#include "Requests.hpp"

#include <ostream>
#include <iostream>
#include <fstream>

#include <tinyxml2.h>
#include <libxml++/libxml++.h>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <gumbo.h>
#include <zip.h>

RomsetCatalogueNointro::RomsetCatalogueNointro(std::string url) :
		m_url(url) {
	std::cout << "RomsetCatalogueNointro()" << std::endl;
	//std::cout << "RomsetCatalogueNointro::update()" << std::endl;
	//update();
	//std::cout << "RomsetCatalogueNointro::update() finished" << std::endl;
}

std::shared_ptr<Romset> RomsetCatalogueNointro::find(std::string romset_name) {
	for (auto &romset : m_romsets) {
		if (romset->name() == romset_name)
			return romset;
	}
	return 0;
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
	/*std::ifstream in("/home/nick/Downloads/redump.org.html",
	 std::ios::in | std::ios::binary);
	 std::string contents;
	 in.seekg(0, std::ios::end);
	 contents.resize(in.tellg());
	 in.seekg(0, std::ios::beg);
	 in.read(&contents[0], contents.size());
	 in.close();*/
	std::string base_url = "http://redump.org";
	Response res = Requests::get(base_url + "/downloads");
	std::string contents = res.text();
	std::cout << contents << std::endl;

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

			std::string url = base_url + attr->value;
			auto filename = Requests::header(url);

			std::string name = "", version = "";

			std::string::size_type pos = filename.find(" - Datfile");
			if (pos != std::string::npos)
				name = filename.substr(0, pos);

			auto pos1 = filename.find_last_of('(') + 1;
			auto pos2 = filename.find_last_of(')');
			version = filename.substr(pos1, pos2 - pos1);

			m_romsets.add(name);
			m_romsets.back()->set_version(version);
			m_romsets.back()->set_url(url);
		}
	}

}

std::filesystem::path RomsetCatalogueNointro::download(std::shared_ptr<Romset> romset, std::filesystem::path directory) {
	std::filesystem::path filename_out;

	std::cout << "download [" << romset->url() << "] to " << directory << std::endl;

	std::string tmpfilename = std::tmpnam(nullptr);
	{
		std::ofstream tmpfile(tmpfilename, std::ios::out | std::ios::binary);
		Requests::download(romset->url(), tmpfile);
	}
	{
		//Open the ZIP archive
		int err = 0;
		zip *z = zip_open(tmpfilename.c_str(), 0, &err);
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
			filename_out = directory / std::string(name);
			std::ofstream out(directory / std::string(name),
					std::ios::out | std::ios::binary);
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
	}
	std::remove(tmpfilename.c_str());

	return filename_out;
}
