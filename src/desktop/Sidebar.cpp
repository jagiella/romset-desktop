/*
 * Sidebar.cpp
 *
 *  Created on: 19.02.2023
 *      Author: nick
 */

#include "Sidebar.hpp"


#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <iostream>

Sidebar::Sidebar(RomsetCollection *romsets):
	m_romsets(romsets)
{
	auto glambda = [this](Romset* rs) { this->onRomsetAdded(rs); };
	romsets->added.connect(glambda);

	for (auto romset: *m_romsets){
		auto &btn = m_buttons.emplace_back(romset.name());
		//btn.set_label(romset.name());
		//btn.set_icon_name("applications-games");
		//btn.set_name(romset.name());
		m_listBox.append(btn);
	}

	this->set_child(m_listBox);
}
void Sidebar::onRomsetAdded(Romset* romset){
	std::cout << "Romset added" << std::endl;

	auto box = Gtk::Box();
	auto icon = Gtk::Image();icon.set_from_icon_name("applications-games");
	auto label = Gtk::Label(romset->name());
	box.append(icon);
	box.append(label);

	auto &btn = m_buttons.emplace_back();
	btn.set_child(box);
	//btn.set_icon_name("applications-games");
	//btn.set_name(romset->name());
		m_listBox.append(btn);
}
