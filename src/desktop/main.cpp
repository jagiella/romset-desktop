#include "helloworld.hpp"
#include "Catalogue.hpp"
#include <gtkmm/application.h>
#include <gtkmm/headerbar.h>

int main(int argc, char *argv[]) {
	RomsetCatalogueNointro cat("http://redump.org/downloads/");
	//return 1;
	auto app = Gtk::Application::create("org.gtkmm.example");

	//Shows the window and returns when it is closed.
	return app->make_window_and_run<HelloWorld>(argc, argv);
}
