#pragma once

#include "wifibot.hpp"
#include <cstdlib>
#include <string>
#include <unordered_map>

#include <glibmm.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/application.h>


class LabeledIcon: public Gtk::Button {
public:
    LabeledIcon(const char *iconName, const char *name, const char *text) {
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

    void setText(const std::string &text) {
        label.set_text(text);
    }
private:
    Gtk::Box container;
    Gtk::Image image;
    Gtk::Label label;
};


class ControlGrid: public Gtk::Grid {
public:
    ControlGrid() {
        confirmButton.set_label("Go");
        confirmButton.set_margin_top(5);
        confirmButton.set_sensitive(false);

        distanceEntry.set_margin_top(5);
        distanceEntry.set_sensitive(false);

        attach(confirmButton, 2, 3);
        attach(distanceEntry, 0, 3, 2, 1);
    }

    void addButton(
        const char *tooltip, const char *iconName, int row, int col, std::function<void()> callback
    ) {
        auto button = Gtk::make_managed<Gtk::Button>();
        button->set_tooltip_text(tooltip);

        button->set_sensitive(false);
        button->signal_clicked().connect(callback);
        button->set_image_from_icon_name(iconName);

        attach(*button, col, row);
    }

    float getDistance() {
        return strtof(distanceEntry.get_text().c_str(), NULL);
    }

    auto go() {
        return confirmButton.signal_clicked();
    }
private:
    Gtk::Entry distanceEntry;
    Gtk::Button confirmButton;
};


class DataGrid: public Gtk::Grid {
public:
    void addLabel(
        const char *name, const char *icon, const char *text,
        int row, int col, int width = 1, int height = 1
    ) {
        labels[name] = Gtk::make_managed<LabeledIcon>(icon, name, text);
        attach(*labels[name], col, row, width, height);
    }

    void updateLabel(const std::string &name, const std::string &value) {
        labels[name]->setText(value);
    }
private:
    std::unordered_map<std::string, LabeledIcon*> labels;
};


class Gui: public Gtk::Window {
public:
    Gui() {
        set_child(container);
        set_title("Wifibot");
        set_resizable(false);

        ipEntry.set_hexpand(true);
        ipEntry.set_margin_end(5);
        ipEntry.set_text("192.168.1.106");

        connectButton.set_label("Connect");
        connectButton.signal_clicked().connect(sigc::mem_fun(*this, &Gui::connect));

        connectBox.append(ipEntry);
        connectBox.append(connectButton);

        controlGrid.addButton("Forward", "up-button-symbolic", 0, 1, [this] {
           robot.speedUp();
        });

        controlGrid.addButton("Back", "down-button-symbolic", 2, 1, [this] {
           robot.speedDown();
        });

        controlGrid.addButton("Left", "left-button-symbolic", 1, 0, [this] {
           robot.turn(-1);
        });

        controlGrid.addButton("Right", "right-button-symbolic", 1, 2, [this] {
           robot.turn(+1);
        });

        controlGrid.addButton("Anti-Clockwise rotation", "rotate-left-button-symbolic", 2, 0, [this] {
            robot.rotate(-1);
        });

        controlGrid.addButton("Clockwise rotation", "rotate-right-button-symbolic", 2, 2, [this] {
            robot.rotate(+1);
        });

        controlGrid.addButton("Stop", "stop-button-symbolic", 1, 1, [this] {
            robot.stop();
        });

        controlGrid.go().connect([this] {
            robot.move(controlGrid.getDistance());
        });

        controlGrid.set_margin_end(5);
        controlGrid.set_halign(Gtk::Align::CENTER);
        controlGrid.set_valign(Gtk::Align::CENTER);

        dataGrid.addLabel("Battery", "battery-label-symbolic", "?%", 0, 0);
        dataGrid.addLabel("Current", "current-label-symbolic", "?A", 0, 1);
        dataGrid.addLabel("Version", "version-label-symbolic", "?", 0, 2, 2, 1);

        dataGrid.addLabel("Left Speed", "speedometer-label-symbolic", "? tics", 1, 0);
        dataGrid.addLabel("Left IR", "ir-label-symbolic", "?cm", 1, 1);

        dataGrid.addLabel("Right Speed", "speedometer-label-symbolic", "? tics", 1, 2);
        dataGrid.addLabel("Right IR", "ir-label-symbolic", "?cm", 1, 3);

        dataGrid.addLabel("X", "odometry-label-symbolic", "X", 2, 0);
        dataGrid.addLabel("Y", "odometry-label-symbolic", "Y", 2, 1);
        dataGrid.addLabel("θ", "odometry-label-symbolic", "θ", 2, 2, 2);

        dataGrid.set_row_spacing(5);
        dataGrid.set_column_spacing(5);
        dataGrid.set_halign(Gtk::Align::CENTER);

        container.set_margin(10);
        container.set_row_spacing(10);
        container.set_column_spacing(10);
        container.attach(connectBox, 0, 0, 2, 1);
        container.attach(controlGrid, 0, 1);
        container.attach(dataGrid, 1, 1);
    }

    ~Gui() {
        robot.disconnect();
    }
private:
    void connect() {
        if (!robot.connect(ipEntry.get_text()))
            return ipEntry.set_text("");

        ipEntry.set_sensitive(false);
        connectButton.set_sensitive(false);

        for (auto *child: controlGrid.get_children()) {
            child->set_sensitive(true);
        }

        for (auto *child: dataGrid.get_children()) {
            child->set_sensitive(true);
        }

        Glib::signal_timeout().connect(sigc::mem_fun(*this, &Gui::timeOut), 400);
    }

    bool timeOut() {
        RData data = robot.getData();

        dataGrid.updateLabel("Battery", std::to_string(data.battery_level) + "%");
        dataGrid.updateLabel("Current", std::to_string(data.current) + "A");
        dataGrid.updateLabel("Version", std::to_string(data.version));

        dataGrid.updateLabel("Left Speed", std::to_string(data.left.speed) + " tics");
        dataGrid.updateLabel("Left IR", std::to_string(data.left.ir) + "cm");

        dataGrid.updateLabel("Right Speed", std::to_string(data.right.speed) + " tics");
        dataGrid.updateLabel("Right IR", std::to_string(data.right.ir) + "cm");

        dataGrid.updateLabel("X", std::to_string(data.x));
        dataGrid.updateLabel("Y", std::to_string(data.y));
        dataGrid.updateLabel("θ", std::to_string(data.theta*180/M_PI));

        return true;
    }
private:
    Wifibot robot;

    Gtk::Grid container;
    ControlGrid controlGrid;
    DataGrid dataGrid;

    Gtk::Box connectBox;
    Gtk::Entry ipEntry;
    Gtk::Button connectButton;
};
