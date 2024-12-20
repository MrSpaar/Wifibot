#include "includes/gui.h"
#include <gtkmm/application.h>


int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("wifibot.polytech.nancy");
    return app->make_window_and_run<Gui>(argc, argv);

	// Gio::init();
	// Wifibot bot;
	// bot.connect("192.168.1.106");

	// std::this_thread::sleep_for(std::chrono::seconds(2));
	// WifibotData data = bot.get_data();

	return 0;
}
