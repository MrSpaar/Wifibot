#pragma once

#include <atomic>
#include <thread>
#include "order.h"
#include "socket.h"

#define LOOP_TIME 200
#define PORT 15020


struct WifibotData {
    short speed_l;
	short speed_r;
	short battery_level;
	short ir1_l;
	short ir2_l;
	short ir1_r;
	short ir2_r;
	short current;
	short version;
	long odometry_l;
	long odometry_r;

	WifibotData(char data[21]);
};


class Wifibot {
public:
	Wifibot();
	~Wifibot();
	void stop();
	void speed_up();
	void speed_down();
	void turn(int direction);
	void rotate(int direction);
	void connect(std::string ip);
	void disconnect();

	WifibotData get_data();
private:
	Order m_order;
	std::thread* m_p_thread_set;
	std::thread* m_p_thread_get;
	std::atomic_bool m_stop;
	Socket_TCP m_socket;
	char m_output_buf[9] = {};
	char m_input_buf[21] = {};
	unsigned char trame_crc[6];
private:
	void start_set_thread();
	void start_get_thread();
	short crc16(unsigned char*, unsigned int);

};
