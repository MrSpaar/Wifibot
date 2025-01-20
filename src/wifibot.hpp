#pragma once

#include "order.hpp"
#include "socket.hpp"

#include <cmath>
#include <atomic>
#include <thread>
#include <iostream>

#define R 0.014
#define L 0.032
#define PORT 15020
#define LOOP_TIME 200
#define IR_MODEL(x) 62.9349569441*std::pow(x, -1.065799095080)

static long initialL = 0;
static long initialR = 0;


struct RData {
    struct Wheel {
        int speed;
        short ir;
        long odometry;
    } left, right;

    short current;
    short version;
    short battery_level;

    double x;
    double y;
    double theta;

    void update(uint8_t data[21], bool &first) {
        left.speed = (data[1] << 8) + data[0];
        if (left.speed > 32767) left.speed -= 65536;

        left.ir = IR_MODEL(data[3]*3.3 / 255);
        left.odometry = (((long) data[8] << 24)) + (((long) data[7] << 16))
                                              + (((long) data[6] << 8))
                                              + ((long) data[5]) - initialL;

        right.speed = (data[10] << 8) + data[9];
        if (right.speed > 32767) right.speed -= 65536;

        right.ir = IR_MODEL(data[11]*3.3 / 255);
        right.odometry = (((long) data[16] << 24)) + (((long) data[15] << 16))
                                               + (((long) data[14] << 8))
                                               + ((long) data[13]) - initialR;

        current = data[17];
        version = data[18];
        battery_level = 12.8/(((unsigned char) data[2]) / 10.0)*100;

        if (first) {
            first = false;
            initialL = left.odometry;
            initialR = right.odometry;
        }
    }
};


class Wifibot {
public:
    void speedUp() {
        if (order.getOrderL() == order.getOrderR()) {
            order.setOrder(order.getOrderL()+10, order.getOrderR()+10);
        } else {
            int moy = order.getOrderR() + order.getOrderL()/2;
            order.setOrder(moy+10, moy+10);
        }

        std::cout << "UP" << std::endl;
        std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
    }

    void speedDown() {
        if (order.getOrderL() == order.getOrderR()) {
            order.setOrder(order.getOrderL()-10, order.getOrderR()-10);
        } else {
            int moy = order.getOrderR() + order.getOrderL()/2;
            order.setOrder(moy-10, moy-10);
        }

        std::cout << "DOWN" << std::endl;
        std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
    }

    void turn(int direction) {
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

    void rotate(int direction) {
        std::cout << "ROTATE" << std::endl;
        stop();
        usleep(1000000);

        if (direction == +1) {
            order.setOrder(10, -10);
            std::cout << "Rotation à Droite" << std::endl;
            std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
        } else {
            order.setOrder(-10, 10);
            std::cout << "Rotation à Gauche" << std::endl;
            std::cout << "Gauche : " << order.getOrderL() << "    Droite : " << order.getOrderR() << std::endl;
        }
    }

    void stop() {
        order.setOrder(0,0);
        std::cout << "Stop()" << std::endl;
    }

    bool connect(const std::string &ip) {
        std::cout << "Wifibot connect()" << std::endl;
        if (!socket.open(ip, PORT)) return false;

        threadSet = std::thread(&Wifibot::startSetThread, this);
        threadGet = std::thread(&Wifibot::startGetThread, this);
        return true;
    }

    RData getData() {
        return rData;
    }

    void disconnect() {
        running = false;
        socket.close();

        if (threadGet.joinable())
           threadGet.join();
        if (threadSet.joinable())
           threadSet.join();
    }

    ~Wifibot() {
        disconnect();
    }
private:
    void startSetThread() {
        unsigned char crcFrame[6];
        std::cout << "Thread [send]: start!" << std::endl;

        while (running) {
            short leftSpeed = abs(order.getOrderL());
            short rightSpeed = abs(order.getOrderR());
            bool speedCtr = order.getSpeedCtr();
            bool leftDirection = order.getOrderL() >= 0;
            bool rightDirection = order.getOrderR() >= 0;

            outBuf[0] = 0xFF;
            outBuf[1] = 0x07;
            outBuf[2] = leftSpeed & 0xFF;
            outBuf[3] = (leftSpeed >> 8) & 0xFF;
            outBuf[4] = rightSpeed & 0xFF;
            outBuf[5] = (rightSpeed >> 8) & 0xFF;
            outBuf[6] = (char) (128*speedCtr + 64*leftDirection + 32*speedCtr + 16*rightDirection + 8);

            crcFrame[0] = outBuf[1];
            crcFrame[1] = outBuf[2];
            crcFrame[2] = outBuf[3];
            crcFrame[3] = outBuf[4];
            crcFrame[4] = outBuf[5];
            crcFrame[5] = outBuf[6];

            short crc = computeCRC16(crcFrame, 6);
            outBuf[7] = crc & 0xFF;
            outBuf[8] = (crc >> 8) & 0xFF;

            socket.send(outBuf, 9);
            std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
        }

        std::cout << "Thread [send]: stop!" << std::endl << std::endl;
    }

    void startGetThread() {
        double delta = LOOP_TIME/1000.0;
        std::cout << "Thread [receive]: start!" << std::endl;

        while (running) {
            socket.receive(inBuf, 21);

            if (((inBuf[20] << 8) | (inBuf[19] & 0xFF)) != computeCRC16(inBuf, 19))
                continue;

            long beforeL = rData.left.odometry;
            long beforeR = rData.right.odometry;
            rData.update(inBuf, first);

            double vL = ((rData.left.odometry - beforeL)/delta)*R;
            double vR = ((rData.right.odometry - beforeR)/delta)*R;

            double v = (vL + vR) / 2;
            double omega = (vR - vL) / L;

            rData.theta += omega * delta;
            rData.x += v * cos(rData.theta) * delta;
            rData.y += v * sin(rData.theta) * delta;

            if (rData.theta > M_PI) rData.theta -= 2 * M_PI;
            if (rData.theta < -M_PI) rData.theta += 2 * M_PI;

            std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
        }

        std::cout << "Thread [receive]: stop!" << std::endl;
    }

    short computeCRC16(unsigned char *frame, size_t length) {
    	unsigned int crc = 0xFFFF;
    	unsigned int polynome = 0xA001;
    	unsigned int parity = 0;

    	for (size_t CptOctet = 0; CptOctet < length; CptOctet++) {
    		crc ^= *(frame + CptOctet);

    		for (int CptBit = 0; CptBit <= 7 ; CptBit++) {
    			parity = crc;
    			crc >>= 1;
    			if (parity%2 == true) crc ^= polynome;
    		}
    	}

    	return(crc);
    }
private:
    RData rData;
    Order order;
    SocketTCP socket;

    std::thread threadSet;
    std::thread threadGet;
    std::atomic_bool running{true};

    bool first = true;
    uint8_t outBuf[9] = {};
    uint8_t inBuf[21] = {};
};
