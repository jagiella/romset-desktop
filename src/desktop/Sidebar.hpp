/*
 * Sidebar.hpp
 *
 *  Created on: 19.02.2023
 *      Author: nick
 */

#ifndef SRC_DESKTOP_SIDEBAR_HPP_
#define SRC_DESKTOP_SIDEBAR_HPP_

#include "Romset.hpp"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/listbox.h>
#include <gtkmm/button.h>

class Sidebar: public Gtk::ScrolledWindow{
private:
	RomsetCollection *m_romsets;
	std::list<Gtk::Button> m_buttons;
	Gtk::ListBox m_listBox;
public:
	Sidebar(RomsetCollection *romsets);
	void onRomsetAdded(std::shared_ptr<Romset> romset);
};



#endif /* SRC_DESKTOP_SIDEBAR_HPP_ */
