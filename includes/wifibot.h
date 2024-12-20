#pragma once

#include "order.h"
#include "socket.h"

#include <atomic>
#include <thread>

#define LOOP_TIME 200
#define PORT 15020


struct RData {
    struct Wheel {
        short speed;
        short ir1;
        short ir2;
        long odometry;
    } left, right;

	short current;
	short version;
	short battery_level;

	RData(char data[21]);
};


class Wifibot {
public:
	~Wifibot();

	void stop();
	void speedUp();
	void speedDown();
	void turn(int direction);
	void rotate(int direction);
	void connect(const std::string &ip);

	RData getData();
	void disconnect();
private:
	Order order;
	SocketTCP socket;

	std::thread threadSet;
	std::thread threadGet;
	std::atomic_bool running{true};

	char outBuf[9] = {};
	char inBuf[21] = {};
	unsigned char crcFrame[6];
private:
	void startSetThread();
	void startGetThread();

	short computeCRC16(unsigned char*, unsigned int);
};
