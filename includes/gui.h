#pragma once

#include "wifibot.h"

#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>


class Gui : public Gtk::Window {
public:
	Gui();
	~Gui();
private:
	Wifibot robot;
	Gtk::Box container;

	Gtk::Box connectBox;
	Gtk::Entry ipEntry;
	Gtk::Button connectButton;

	Gtk::Grid buttonGrid;
	Gtk::Button upButton;
	Gtk::Button downButton;
	Gtk::Button stopButton;
	Gtk::Button leftButton;
	Gtk::Button rightButton;
	Gtk::Button rotLButton;
	Gtk::Button rotRButton;
};
