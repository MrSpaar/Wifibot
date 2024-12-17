#include <iostream>
#include "giomm/init.h"
#include "includes/wifibot.h"


int main(int argc, char *argv[]) {
	// Gtk::Main app(argc,argv);
	// Gui win;
	// app.run(win);
	Gio::init();

	Wifibot bot;
	bot.connect("192.168.1.106");

	std::this_thread::sleep_for(std::chrono::seconds(2));
	WifibotData data = bot.get_data();

	std::cout << "Battery: " << data.battery_level << std::endl;
	std::cout << "Speed L: " << data.speed_l << std::endl;
	std::cout << "Speed R: " << data.speed_r << std::endl;
	std::cout << "IR1 L : " << data.ir1_l << std::endl;
	std::cout << "IR2 L: " << data.ir2_l << std::endl;
	std::cout << "IR1 R: " << data.ir1_r << std::endl;
	std::cout << "IR2 R: " << data.ir2_r << std::endl;
	std::cout << "Current: " << data.current << std::endl;
	std::cout << "Version: " << data.version << std::endl;
	std::cout << "Odometry L: " << data.odometry_l << std::endl;
	std::cout << "Odometry R: " << data.odometry_r << std::endl;

	return 0;
}
