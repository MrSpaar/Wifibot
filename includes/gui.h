#pragma once

#include <gtkmm-3.0/gtkmm.h>
#include "wifibot.h"


class Gui : public Gtk::Window {
public:
	Gui();
	~Gui();
private:
	Wifibot m_robot;
	Gtk::Button m_bouton_1;
	Gtk::Button m_bouton_up;
	Gtk::Button m_bouton_down;
	Gtk::Button m_bouton_stop;
	Gtk::Button m_bouton_left;
	Gtk::Button m_bouton_right;
	Gtk::Button m_bouton_rot_l;
	Gtk::Button m_bouton_rot_r;
	Gtk::Entry m_entree_1;
	Gtk::Label titre;
	Gtk::Label fin;
};
