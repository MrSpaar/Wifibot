#include "gui.hpp"

// Capteurs extéroceptifs :
// 2 infrarouge à l'avant : GP2Y0A02YK
// 20cm à 150cm
// Signal analogique (Voltage)
// La trame renvoit le voltage


int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("com.github.mrspaar.Wifibot");
    return app->make_window_and_run<Gui>(argc, argv);
}
