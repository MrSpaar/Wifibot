#pragma once

#include "wifibot.h"

#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <unordered_map>


class ControlGrid: public Gtk::Grid {
public:
    void addButton(
        const char *iconName, int row, int col, std::function<void()> callback
    );
};


class LabeledIcon: public Gtk::Button {
public:
    LabeledIcon(const char *iconName, const char *name, const char *text);
    void setText(const std::string &text);
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
    );

    void updateLabel(const std::string &name, const std::string &value);
private:
    std::unordered_map<std::string, LabeledIcon*> labels;
};


class Gui: public Gtk::Window {
public:
	Gui();
	~Gui();
private:
    bool timeOut();
private:
	Wifibot robot;

	Gtk::Grid container;
	ControlGrid controlGrid;
	DataGrid dataGrid;

	Gtk::Box connectBox;
	Gtk::Entry ipEntry;
	Gtk::Button connectButton;
};
