#include "includes/gui.h"
#include "includes/wifibot.h"
#include <glibmm.h>
#include <string>


void ControlGrid::addButton(
    const char *iconName, int row, int col, std::function<void()> callback
) {
    auto button = Gtk::make_managed<Gtk::Button>();

    button->signal_clicked().connect(callback);
    button->set_image_from_icon_name(iconName);

    attach(*button, col, row);
}

LabeledIcon::LabeledIcon(const char *iconName, const char *name, const char *text) {
    label.set_text(text);
    image.set_from_icon_name(iconName);

    container.append(image);
    container.append(label);
    container.set_spacing(5);
    container.set_halign(Gtk::Align::CENTER);

    set_child(container);
    set_tooltip_text(name);
    set_sensitive(false);
}

void LabeledIcon::setText(const std::string &text) {
    label.set_text(text);
}

void DataGrid::addLabel(
    const char *name, const char *iconName, const char *text,
    int row, int col, int width, int height
) {
    labels[name] = Gtk::make_managed<LabeledIcon>(iconName, name, text);
    attach(*labels[name], col, row, width, height);
}

void DataGrid::updateLabel(const std::string &name, const std::string &value) {
    labels[name]->setText(value);
}

Gui::Gui() {
    set_child(container);
    set_title("Wifibot");
	set_resizable(false);

	ipEntry.set_hexpand(true);
	ipEntry.set_margin_end(5);
	ipEntry.set_text("192.168.1.106");

	connectButton.set_label("Connect");
	connectButton.signal_clicked().connect([&] {
		robot.connect(ipEntry.get_text());
	});

	connectBox.append(ipEntry);
	connectBox.append(connectButton);

	controlGrid.addButton("up-button-symbolic", 0, 1, [&] {
	   robot.speedUp();
	});

	controlGrid.addButton("down-button-symbolic", 2, 1, [&] {
	   robot.speedDown();
	});

	controlGrid.addButton("left-button-symbolic", 1, 0, [&] {
	   robot.turn(-1);
	});

	controlGrid.addButton("right-button-symbolic", 1, 2, [&] {
	   robot.turn(+1);
	});

    controlGrid.addButton("rotate-left-button-symbolic", 2, 0, [&] {
        robot.rotate(-1);
    });

    controlGrid.addButton("rotate-right-button-symbolic", 2, 2, [&] {
        robot.rotate(+1);
    });

    controlGrid.addButton("stop-button-symbolic", 1, 1, [&] {
        robot.stop();
    });

	controlGrid.set_margin_end(5);
	controlGrid.set_halign(Gtk::Align::CENTER);
	controlGrid.set_valign(Gtk::Align::CENTER);

	dataGrid.addLabel("Battery", "battery-label-symbolic", "?%", 0, 0);
	dataGrid.addLabel("Current", "current-label-symbolic", "?A", 0, 1);
	dataGrid.addLabel("Version", "version-label-symbolic", "?", 0, 2, 2, 1);

	dataGrid.addLabel("Left Speed", "speedometer-label-symbolic", "?", 1, 0);
	dataGrid.addLabel("Left Odometry", "odometry-label-symbolic", "?", 1, 1);
	dataGrid.addLabel("Left IR1", "ir-label-symbolic", "?", 2, 0);
	dataGrid.addLabel("Left IR2", "ir-label-symbolic", "?", 2, 1);

	dataGrid.addLabel("Right Speed", "speedometer-label-symbolic", "?", 1, 2);
	dataGrid.addLabel("Right Odometry", "odometry-label-symbolic", "?", 1, 3);
	dataGrid.addLabel("Right IR1", "ir-label-symbolic", "?", 2, 2);
	dataGrid.addLabel("Right IR2", "ir-label-symbolic", "?", 2, 3);

	dataGrid.set_row_spacing(5);
	dataGrid.set_column_spacing(5);
	dataGrid.set_halign(Gtk::Align::CENTER);

	container.attach(connectBox, 0, 0, 2, 1);
	container.attach(controlGrid, 0, 1);
	container.attach(dataGrid, 1, 1);
	container.set_margin(10);
	container.set_row_spacing(10);
	container.set_column_spacing(10);

	Glib ::signal_timeout().connect(sigc::mem_fun(*this, &Gui::timeOut), 400);
}

bool Gui::timeOut() {
    RData data = robot.getData();

    dataGrid.updateLabel("Battery", std::to_string(data.battery_level) + "%");
    dataGrid.updateLabel("Current", std::to_string(data.current) + "A");
    dataGrid.updateLabel("Version", std::to_string(data.version));

    dataGrid.updateLabel("Left Speed", std::to_string(data.left.speed));
    dataGrid.updateLabel("Left Odometry", std::to_string(data.left.odometry));
    dataGrid.updateLabel("Left IR1", std::to_string(data.left.ir1));
    dataGrid.updateLabel("Left IR2", std::to_string(data.left.ir2));

    dataGrid.updateLabel("Right Speed", std::to_string(data.right.speed));
    dataGrid.updateLabel("Right Odometry", std::to_string(data.right.odometry));
    dataGrid.updateLabel("Right IR1", std::to_string(data.right.ir1));
    dataGrid.updateLabel("Right IR2", std::to_string(data.right.ir2));

    return true;
}

Gui::~Gui() {
	robot.disconnect();
}
