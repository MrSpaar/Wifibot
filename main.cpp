#include "includes/gui.h"
#include <gtkmm/application.h>

// Capteurs extéroceptifs :
// 2 infrarouge à l'avant : GP2Y0A02YK
// 20cm à 150cm
// Signal analogique (Voltage)
// La trame renvoit le voltage

// Modèle : 48.2275*x^-0.935666

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("org.wifibot");
    return app->make_window_and_run<Gui>(argc, argv);
}
