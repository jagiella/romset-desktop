/*
 * Headerbar.cpp
 *
 *  Created on: 28.02.2023
 *      Author: nickj
 */

#include "Headerbar.hpp"

#include <gtkmm/button.h>

Headerbar::Headerbar() : HeaderBar(){
	Gtk::Button btn;
	btn.set_image_from_icon_name("list-add");
	Gtk::Button btn2;
	btn2.set_image_from_icon_name("folder-saved-search");
	this->pack_start(btn);
	this->pack_start(btn2);
}
