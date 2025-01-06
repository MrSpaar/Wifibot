#pragma once

#include "wifibot.hpp"
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


class ControlGrid: public Gtk::Grid {
public:
    void addButton(
        const char *iconName, int row, int col, std::function<void()> callback
    ) {
        auto button = Gtk::make_managed<Gtk::Button>();

        button->set_sensitive(false);
        button->signal_clicked().connect(callback);
        button->set_image_from_icon_name(iconName);

        attach(*button, col, row);
    }
};


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

        controlGrid.addButton("up-button-symbolic", 0, 1, [this] {
           robot.speedUp();
        });

        controlGrid.addButton("down-button-symbolic", 2, 1, [this] {
           robot.speedDown();
        });

        controlGrid.addButton("left-button-symbolic", 1, 0, [this] {
           robot.turn(-1);
        });

        controlGrid.addButton("right-button-symbolic", 1, 2, [this] {
           robot.turn(+1);
        });

        controlGrid.addButton("rotate-left-button-symbolic", 2, 0, [this] {
            robot.rotate(-1);
        });

        controlGrid.addButton("rotate-right-button-symbolic", 2, 2, [this] {
            robot.rotate(+1);
        });

        controlGrid.addButton("stop-button-symbolic", 1, 1, [this] {
            robot.stop();
        });

        controlGrid.set_margin_end(5);
        controlGrid.set_halign(Gtk::Align::CENTER);
        controlGrid.set_valign(Gtk::Align::CENTER);

        dataGrid.addLabel("Battery", "battery-label-symbolic", "?%", 0, 0);
        dataGrid.addLabel("Current", "current-label-symbolic", "?A", 0, 1);
        dataGrid.addLabel("Version", "version-label-symbolic", "?", 0, 2, 2, 1);

        dataGrid.addLabel("Left Speed", "speedometer-label-symbolic", "? tics", 1, 0);
        dataGrid.addLabel("Left IR", "ir-label-symbolic", "?cm", 1, 1);
        dataGrid.addLabel("Left Odometry", "odometry-label-symbolic", "?", 2, 0, 2, 1);

        dataGrid.addLabel("Right Speed", "speedometer-label-symbolic", "? tics", 1, 2);
        dataGrid.addLabel("Right IR", "ir-label-symbolic", "?cm", 1, 3);
        dataGrid.addLabel("Right Odometry", "odometry-label-symbolic", "?", 2, 2, 2, 1);

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
        dataGrid.updateLabel("Left Odometry", std::to_string(data.left.odometry));
        dataGrid.updateLabel("Left IR", std::to_string(data.left.ir) + "cm");

        dataGrid.updateLabel("Right Speed", std::to_string(data.right.speed) + " tics");
        dataGrid.updateLabel("Right Odometry", std::to_string(data.right.odometry));
        dataGrid.updateLabel("Right I1", std::to_string(data.right.ir) + "cm");

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
