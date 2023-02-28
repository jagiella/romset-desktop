#include "XML.hpp"
#include <libxml/parser.h>
#include <libxml/xpath.h>

XMLDocument::XMLDocument(std::string filename) {
	xmlDocPtr doc;
	doc = xmlParseFile(docname);

	if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully. \n");
		//return NULL;
	}
}
