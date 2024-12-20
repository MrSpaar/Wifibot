#include <iostream>
#include "includes/gui.h"


Gui::Gui() {
    set_child(container);
    set_title("Wifibot");
	set_resizable(false);

	connectButton.set_label("Connect");
	ipEntry.set_margin_end(5);
	ipEntry.set_text("127.0.0.1");

	connectBox.append(ipEntry);
	connectBox.append(connectButton);
	connectBox.set_margin(5);
	connectBox.set_orientation(Gtk::Orientation::HORIZONTAL);

	upButton.set_image_from_icon_name("go-up-symbolic");
	buttonGrid.attach(upButton, 1, 0);

	downButton.set_image_from_icon_name("go-down-symbolic");
	buttonGrid.attach(downButton, 1, 2);

	leftButton.set_image_from_icon_name("go-previous-symbolic");
	buttonGrid.attach(leftButton, 0, 1);

	rightButton.set_image_from_icon_name("go-next-symbolic");
	buttonGrid.attach(rightButton, 2, 1);

	rotLButton.set_image_from_icon_name("object-rotate-left-symbolic");
	buttonGrid.attach(rotLButton, 0, 2);

	rotRButton.set_image_from_icon_name("object-rotate-right-symbolic");
	buttonGrid.attach(rotRButton, 2, 2);

	stopButton.set_image_from_icon_name("media-playback-stop-symbolic");
	buttonGrid.attach(stopButton, 1, 1);

	buttonGrid.set_margin_bottom(5);
	buttonGrid.set_halign(Gtk::Align::CENTER);

	container.append(connectBox);
	container.append(buttonGrid);
	container.set_halign(Gtk::Align::CENTER);
	container.set_orientation(Gtk::Orientation::VERTICAL);

	connectButton.signal_clicked().connect([this]() {
		robot.connect(ipEntry.get_text());
		std::cout << "Connexion Wifibot" << std::endl;
	});

	upButton.signal_clicked().connect([this]() {
		robot.speedUp();
	});

	downButton.signal_clicked().connect([this]() {
		robot.speedDown();
	});

	leftButton.signal_clicked().connect([this]() {
		robot.turn(-1);
	});

	rightButton.signal_clicked().connect([this]() {
		robot.turn(+1);
	});

	rotLButton.signal_clicked().connect([this]() {
		robot.rotate(-1);
	});

	rotRButton.signal_clicked().connect([this]() {
		robot.rotate(+1);
	});

	stopButton.signal_clicked().connect([this]() {
		robot.stop();
	});
}

Gui::~Gui() {
	robot.disconnect();
}
