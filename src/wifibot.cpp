#include <cmath>
#include <thread>
#include <unistd.h>
#include <iostream>

#include "includes/wifibot.h"


void Wifibot::stop() {
	order.setOrder(0,0);
	std::cout << "Stop()" << std::endl;
}

void Wifibot::speedUp() {
	if (order.getOrderL() == order.getOrderR()) {
		order.setOrder(order.getOrderL()+10, order.getOrderR()+10);
	} else {
		int moy = order.getOrderR() + order.getOrderL()/2;
		order.setOrder(moy+10, moy+10);
	}

	std::cout << "UP" << std::endl;
	std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
}

void Wifibot::speedDown() {
	if (order.getOrderL() == order.getOrderR()) {
		order.setOrder(order.getOrderL()-10, order.getOrderR()-10);
	} else {
		int moy = order.getOrderR() + order.getOrderL()/2;
		order.setOrder(moy-10, moy-10);
	}

	std::cout << "DOWN" << std::endl;
	std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;

}

void Wifibot::turn(int direction) {
	std::cout << "TURN" << std::endl;
	if (direction == +1) {
		order.setOrder(order.getOrderL()*1.2, order.getOrderR()*0.8);
		std::cout << "Tourne à Droite" << std::endl;
		std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
	} else {
		order.setOrder(order.getOrderL()*0.8, order.getOrderR()*1.2);
		std::cout << "Tourne à Gauche" << std::endl;
		std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
	}
}

void Wifibot::rotate(int direction) {
	std::cout << "ROTATE" << std::endl;
	this->stop();
	usleep(1000000);

	if (direction==+1) {
		order.setOrder(10, -10);
		std::cout << "Rotation à Droite" << std::endl;
		std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
	} else {
		order.setOrder(-10, 10);
		std::cout << "Rotation à Gauche" << std::endl;
		std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
	}
}

short Wifibot::computeCRC16(unsigned char* frame, unsigned int length) {
    unsigned int parity = 0;
	unsigned int crc = 0xFFFF;
	unsigned int polynome = 0xA001;

	for (unsigned int idxByte = 0 ; idxByte < length; idxByte++) {
		crc ^= *(frame + idxByte);

		for (unsigned int idxBit = 0; idxBit <= 7 ; idxBit++) {
			parity = crc;
			crc >>= 1;
			if (parity%2 == true) crc ^= polynome;
		}
	}

	return(crc);
}

void Wifibot::startSetThread() {
    short crc;
	static int cpt;

	while (running) {
		std::cout << "Thread [send]: " << ++cpt << std::endl;

		bool speedCtr = order.getSpeedCtr();
		bool rightDirection = order.getOrderR() >=0;
		short rightSpeed = abs(order.getOrderR());
		bool leftDirection = order.getOrderL() >=0;
		short leftSpeed = abs(order.getOrderL());

		outBuf[0] = 0xFF;
		outBuf[1] = 0x07;
		outBuf[2] = leftSpeed & 0xFF;
		outBuf[3] = (leftSpeed >> 8) & 0xFF;
		outBuf[4] = rightSpeed & 0xFF;
		outBuf[5] = (rightSpeed >> 8) & 0xFF;
		outBuf[6] = (char) (128*speedCtr + 64*leftDirection + 32*speedCtr + 16*rightDirection + 8);

		crcFrame[0] = (unsigned char) outBuf[1];
		crcFrame[1] = (unsigned char) outBuf[2];
		crcFrame[2] = (unsigned char) outBuf[3];
		crcFrame[3] = (unsigned char) outBuf[4];
		crcFrame[4] = (unsigned char) outBuf[5];
		crcFrame[5] = (unsigned char) outBuf[6];

		crc = computeCRC16(crcFrame,6);
		outBuf[7] = crc & 0xFF;
		outBuf[8] = (crc >> 8) & 0xFF;

		socket.send(outBuf, 9);
		std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
	}

	std::cout << "Thread [send]: stop !" << std::endl << std::endl;
}

void Wifibot::startGetThread() {
    short crc;

    while (running) {
        socket.receive(inBuf, 21);
        crc = (inBuf[20] << 8) | (inBuf[19] & 0xFF);

        if (crc != computeCRC16(reinterpret_cast<unsigned char*>(inBuf), 19))
            continue;

        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
    }
}

RData::RData(char data[21]) {
    left.speed = (data[1] << 8) + data[0];
    left.ir1 = data[3];
    left.ir2 = data[4];
    left.odometry = ((((long)data[8] << 24)) + (((long)data[7] << 16))
                                          + (((long)data[6] << 8))
                                          + ((long)data[5]));

    right.speed = (data[10] << 8) + data[9];
    right.ir1 = data[11];
    right.ir2 = data[12];
    right.odometry = ((((long)data[16] << 24)) + (((long)data[15] << 16))
                                           + (((long)data[14] << 8))
                                           + ((long)data[13]));

    current = data[17];
    version = data[18];
    battery_level = data[2];
}

void Wifibot::connect(const std::string &ip) {
	socket.open(ip, PORT);
	std::cout << "Wifibot connect()" << std::endl;

	if (socket.isOpened()) {
	    // threadSet = std::thread(&Wifibot::startSetThread, this);
		threadGet = std::thread(&Wifibot::startGetThread, this);
	}
}

RData Wifibot::getData() {
    return { inBuf };
}

void Wifibot::disconnect() {
    running = false;

	if (threadGet.joinable())
	   threadGet.join();
	if (threadSet.joinable())
	   threadSet.join();

	socket.close();
}

Wifibot::~Wifibot() {
	disconnect();
}
