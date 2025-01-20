#include "gui.hpp"


int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("com.github.mrspaar.Wifibot");
    return app->make_window_and_run<Gui>(argc, argv);
}
