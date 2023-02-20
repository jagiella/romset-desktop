/*
 * helloworld.hpp
 *
 *  Created on: 17.02.2023
 *      Author: nickj
 */

#ifndef SRC_HELLOWORLD_HPP_
#define SRC_HELLOWORLD_HPP_

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/dialog.h>
#include <gtkmm/paned.h>
#include <gtkmm/filechooser.h>
#include <gtkmm.h>

#include "Romset.hpp"
#include "Sidebar.hpp"


class ModelColumns: public Gtk::TreeModelColumnRecord {
public:

	ModelColumns() {
		add(m_col_text);
	}

	Gtk::TreeModelColumn<Glib::ustring> m_col_text;
};

class HelloWorld: public Gtk::Window {

private:
	RomsetCollection m_romsets;

public:
	HelloWorld();
	virtual ~HelloWorld();

protected:
	//Signal handlers:
	void on_button_clicked();
	void on_folder_button_clicked();
	void on_file_dialog_response(int response_id,
			Gtk::FileChooserDialog *dialog);
	void on_folder_dialog_response(int response_id,
			Gtk::FileChooserDialog *dialog);

	//Member widgets:
	Gtk::Button m_button;
	Gtk::Button m_folder_button;
	Gtk::Box m_ButtonBox;

	Sidebar m_sidebar;

	Gtk::Paned m_Paned;
	Gtk::TreeView m_tree;
	ModelColumns m_Columns;
	Gtk::TreeRow m_Row_matched;
	Gtk::TreeRow m_Row_unmatched;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeStore;
};

#endif /* SRC_HELLOWORLD_HPP_ */
