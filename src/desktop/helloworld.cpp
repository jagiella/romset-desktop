#include "helloworld.hpp"
#include "RomFile.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <gtkmm.h>
#include <jsoncpp/json/json.h>

HelloWorld::HelloWorld() :
		m_button("Load Dat file"), // creates a new button with label "Hello World".
		m_folder_button("Scan directory"),
		m_sidebar(&m_romsets),
		m_headerbar() {

	std::cout << "HelloWorld()" << std::endl;
	set_size_request(500, 500);
	m_button.signal_clicked().connect(
			sigc::mem_fun(*this, &HelloWorld::on_button_clicked));
	m_folder_button.signal_clicked().connect(
			sigc::mem_fun(*this, &HelloWorld::on_folder_button_clicked));

	// create model
	m_refTreeStore = Gtk::TreeStore::create(m_Columns);
	// Assign the model to the TreeView
	m_tree.set_model(m_refTreeStore);
	// Adding View Columns
	m_tree.append_column("Messages", m_Columns.m_col_text);

	m_Row_matched = *m_refTreeStore->append();
	m_Row_matched[m_Columns.m_col_text] = "match";
	m_Row_unmatched = *m_refTreeStore->append();
	m_Row_unmatched[m_Columns.m_col_text] = "unmatched";

	auto row_child = *m_refTreeStore->append(m_Row_matched.children());
	row_child[m_Columns.m_col_text] = "rom1";
	row_child = *m_refTreeStore->append(m_Row_matched.children());
	row_child[m_Columns.m_col_text] = "rom2";

	auto scroll = Gtk::ScrolledWindow();
	scroll.set_child(m_tree);

	// This packs the button into the Window (a container).
	m_ButtonBox.append(m_button);
	m_ButtonBox.append(m_folder_button);
	//m_ButtonBox.append(scroll);
	m_Paned.set_start_child(m_sidebar);
	m_Paned.set_end_child(scroll);
	this->set_child(m_Paned);

	// The final step is to display this newly created widget...
	m_button.show();

	/*Json::Value root;
	std::ifstream myfile;
	std::cout << "read config" << std::endl;
	myfile.open("/home/nick/Downloads/example.json");
	if (myfile.is_open()) {
		myfile >> root;
		myfile.close();
		std::cout << root << std::endl;

		for (int index = 0; index < root.size(); ++index) {
			this->m_romsets.add(root[index][0].asString(),
					root[index][1].asString());
		}
	}*/
	std::filesystem::path home = getenv("HOME");
	std::filesystem::path path = home / ".config" / "romset";
	std::filesystem::create_directories(path);

	for (auto datfile : std::filesystem::directory_iterator(path)) {
		if (datfile.path().extension() == ".dat") {
			std::cout << "Found:" << datfile << std::endl;
			this->m_romsets.add(path / datfile, "");
		}
	}

	//creatTitlebar();
	this->set_titlebar(m_headerbar);
}

void HelloWorld::creatTitlebar() {
	Gtk::Button btn;
	btn.set_image_from_icon_name("list-add");
	Gtk::Button btn2;
	btn2.set_image_from_icon_name("folder-saved-search");
	m_headerbar.pack_start(btn);
	m_headerbar.pack_start(btn2);
	set_titlebar(m_headerbar);
}

HelloWorld::~HelloWorld() {
	Json::Value root;

	for (auto romset : m_romsets) {
		Json::Value v_romset;
		v_romset.append(romset->filename());
		v_romset.append(romset->directory());
		root.append(v_romset);
	}
	std::cout << root << std::endl;

	std::ofstream myfile;
	myfile.open("example.txt");
	myfile << root;
	myfile.close();
}

void HelloWorld::on_button_clicked() {
	std::cout << "Hello World" << std::endl;

	auto dialog = new Gtk::FileChooserDialog("Please choose a file",
			Gtk::FileChooser::Action::OPEN);
	dialog->set_transient_for(*this);
	dialog->set_modal(true);
	dialog->signal_response().connect(
			sigc::bind(
					sigc::mem_fun(*this, &HelloWorld::on_file_dialog_response),
					dialog));

	//Add response buttons to the dialog:
	dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
	dialog->add_button("_Open", Gtk::ResponseType::OK);

	auto filter_text = Gtk::FileFilter::create();
	filter_text->set_name("Dat files");
	//filter_text->add_mime_type("text/plain");
	filter_text->add_pattern("*.dat");
	dialog->add_filter(filter_text);

	dialog->show();
	std::cout << "show finished." << std::endl;
}

void HelloWorld::on_folder_button_clicked() {
	auto dialog = new Gtk::FileChooserDialog("Please choose a folder",
			Gtk::FileChooser::Action::SELECT_FOLDER);
	dialog->set_transient_for(*this);
	dialog->set_modal(true);
	dialog->signal_response().connect(
			sigc::bind(
					sigc::mem_fun(*this,
							&HelloWorld::on_folder_dialog_response), dialog));

	//Add response buttons to the dialog:
	dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
	dialog->add_button("Select", Gtk::ResponseType::OK);

	//Show the dialog and wait for a user response:
	dialog->show();
}

void HelloWorld::on_file_dialog_response(int response_id,
		Gtk::FileChooserDialog *dialog) {
	//Handle the response:
	switch (response_id) {
	case Gtk::ResponseType::OK: {
		std::cout << "Open clicked." << std::endl;

		//Notice that this is a std::string, not a Glib::ustring.
		auto filename = dialog->get_file()->get_path();
		std::cout << "File selected: " << filename << std::endl;
		this->m_romsets.add(filename, "/home/nickj/Dokumente/Romsets/");
		break;
	}
	case Gtk::ResponseType::CANCEL: {
		std::cout << "Cancel clicked." << std::endl;
		break;
	}
	default: {
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	}
	}
	std::cout << "dialog." << std::endl;
	delete dialog;
	std::cout << "dialog finished." << std::endl;
}

void HelloWorld::on_folder_dialog_response(int response_id,
		Gtk::FileChooserDialog *dialog) {
	//Handle the response:
	switch (response_id) {
	case Gtk::ResponseType::OK: {
		std::cout << "Select clicked." << std::endl;
		std::cout << "Folder selected: " << dialog->get_file()->get_path()
				<< std::endl;

		int count = 0;
		for (auto &entry : std::filesystem::directory_iterator(
				dialog->get_file()->get_path())) {
			RomFile romfile(entry.path().c_str());
			std::cout << romfile.md5() << " ==> " << entry.path() << std::endl;
			/*for( auto rom : m_romsets.back().roms()){
			 if(romfile.size() == rom.size() and romfile.md5() == rom.md5())
			 std::cout << "found match:" << rom << std::endl;
			 }*/
			for (auto romset : m_romsets) {
				if (romset->contains(&romfile)) {
					std::cout << "found match:" << romfile << std::endl;
					romset->import(romfile.path(), &romfile);
					auto row_child = *m_refTreeStore->append(
							m_Row_matched.children());
					row_child[m_Columns.m_col_text] = romfile.name();
					count++;
				} else {
					auto row_child = *m_refTreeStore->append(
							m_Row_unmatched.children());
					row_child[m_Columns.m_col_text] = romfile.name();
					//m_Row_unmatched.
				}
			}
		}
		break;
	}
	case Gtk::ResponseType::CANCEL: {
		std::cout << "Cancel clicked." << std::endl;
		break;
	}
	default: {
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	}
	}
	delete dialog;
}
