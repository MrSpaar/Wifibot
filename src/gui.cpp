#include <iostream>
#include "includes/gui.h"


Gui::Gui():
	m_robot(),
	m_bouton_1("Connexion"),
	m_bouton_up(),
	m_bouton_down(),
	m_bouton_stop(),
	m_bouton_left(),
	m_bouton_right(),
	m_bouton_rot_l(),
	m_bouton_rot_r(),
	m_entree_1(),
	titre("WIFIBOT CONTROL"),
	fin("COPYRIGHT 2023")
{
	Gtk::HBox* m_hbox_1=Gtk::manage(new Gtk::HBox(false,20));
	Gtk::HBox* m_hbox_2=Gtk::manage(new Gtk::HBox(false,20));
	Gtk::HBox* m_hbox_debut=Gtk::manage(new Gtk::HBox(false,20));
	Gtk::HBox* m_hbox_fin=Gtk::manage(new Gtk::HBox(false,20));
	Gtk::VBox* m_vbox=Gtk::manage(new Gtk::VBox(false,30));
	Gtk::Table* tableau=Gtk::manage(new Gtk::Table(3,3));
	Gtk::Image* image_1=Gtk::manage(new Gtk::Image("images/robot.png"));
	Gtk::Image* avant=Gtk::manage(new Gtk::Image("images/icon_up.png"));
	Gtk::Image* arriere=Gtk::manage(new Gtk::Image("images/icon_down.png"));
	Gtk::Image* gauche=Gtk::manage(new Gtk::Image("images/icon_left.png"));
	Gtk::Image* droite=Gtk::manage(new Gtk::Image("images/icon_right.png"));
	Gtk::Image* rot_g=Gtk::manage(new Gtk::Image("images/icon_rot_left.png"));
	Gtk::Image* rot_d=Gtk::manage(new Gtk::Image("images/icon_rot_right.png"));
	Gtk::Image* stop=Gtk::manage(new Gtk::Image("images/stop.png"));

	m_bouton_up.set_image(*avant);
	m_bouton_down.set_image(*arriere);
	m_bouton_left.set_image(*gauche);
	m_bouton_right.set_image(*droite);
	m_bouton_rot_l.set_image(*rot_g);
	m_bouton_rot_r.set_image(*rot_d);
	m_bouton_stop.set_image(*stop);

	tableau->attach(m_bouton_up,1,2,0,1);
	tableau->attach(m_bouton_down,1,2,2,3);
	tableau->attach(m_bouton_left,0,1,1,2);
	tableau->attach(m_bouton_right,2,3,1,2);
	tableau->attach(m_bouton_rot_l,0,1,2,3);
	tableau->attach(m_bouton_rot_r,2,3,2,3);
	tableau->attach(m_bouton_stop,1,2,1,2);

	this->set_title("Projet Wifibot");
	this->set_position(Gtk::WIN_POS_CENTER);
	this->resize(650,300);
	this->set_resizable(false);

	m_entree_1.set_text("127.0.0.1");
	//m_entree_1.set_alignment(0.5);

	//m_box_1.pack_start(m_box_3);
	//m_box_1.pack_start(m_box_4);
	m_hbox_1->pack_start(m_entree_1);
	m_hbox_1->pack_start(m_bouton_1);
	m_hbox_debut->pack_start(titre);
	m_hbox_fin->pack_start(fin);
	m_vbox->set_spacing(30);
	m_vbox->add(*m_hbox_debut);
	m_vbox->add(*m_hbox_1);
	m_hbox_2->pack_start(*image_1);
	m_hbox_2->pack_start(*tableau);

	m_vbox->add(*m_hbox_2);
	m_vbox->add(*m_hbox_fin);

	this->add(*m_vbox);
	this->show_all();

	m_bouton_1.signal_clicked().connect([this]() {
		std::string adresse=m_entree_1.get_text();
		m_robot.connect(adresse);
		std::cout << "Connexion Wifibot" << std::endl;
	});

	m_bouton_up.signal_clicked().connect([this]() {
		m_robot.speed_up();
	});

	m_bouton_down.signal_clicked().connect([this]() {
		m_robot.speed_down();
	});

	m_bouton_left.signal_clicked().connect([this]() {
		m_robot.turn(-1);
	});

	m_bouton_right.signal_clicked().connect([this]() {
		m_robot.turn(+1);
	});

	m_bouton_rot_l.signal_clicked().connect([this]() {
		m_robot.rotate(-1);
	});

	m_bouton_rot_r.signal_clicked().connect([this]() {
		m_robot.rotate(+1);
	});

	m_bouton_stop.signal_clicked().connect([this]() {
		m_robot.stop();
	});

}

Gui::~Gui() {
	m_robot.disconnect();
}
