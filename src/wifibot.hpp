#pragma once

#include "order.hpp"
#include "socket.hpp"

#include <cmath>
#include <atomic>
#include <thread>
#include <iostream>

#define PORT 15020
#define LOOP_TIME 200
#define IR_MODEL(x) 62.9349569441*std::pow(x, -1.065799095080)


struct RData {
    struct Wheel {
        int speed;
        short ir;
        long odometry;
    } left, right;

    short current;
    short version;
    short battery_level;

    RData(uint8_t data[21]) {
        left.speed = (data[1] << 8) + data[0];
        if (left.speed > 32767) left.speed -= 65536;

        left.ir = IR_MODEL(data[3]);
        left.odometry = ((((long) data[8] << 24)) + (((long) data[7] << 16))
                                              + (((long) data[6] << 8))
                                              + ((long) data[5]));

        right.speed = (data[10] << 8) + data[9];
        if (right.speed > 32767) right.speed -= 65536;

        right.ir = IR_MODEL(data[11]);
        right.odometry = ((((long) data[16] << 24)) + (((long) data[15] << 16))
                                               + (((long) data[14] << 8))
                                               + ((long) data[13]));

        current = data[17];
        version = data[18];
        battery_level = 12.8/(((unsigned char) data[2]) / 10.0)*100;
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
        return { inBuf };
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

            short crc = computeCRC16(outBuf, 6, 1);
            outBuf[7] = crc & 0xFF;
            outBuf[8] = (crc >> 8) & 0xFF;

            socket.send(outBuf, 9);
            std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
        }

        std::cout << "Thread [send]: stop!" << std::endl << std::endl;
    }

    void startGetThread() {
        std::cout << "Thread [receive]: start!" << std::endl;

        while (running) {
            socket.receive(inBuf, 21);

            if (((inBuf[20] << 8) | (inBuf[19] & 0xFF)) != computeCRC16(inBuf, 19))
                continue;

            std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
        }

        std::cout << "Thread [receive]: stop!" << std::endl;
    }

    short computeCRC16(uint8_t* frame, size_t length, uint8_t index = 0) {
        unsigned int parity = 0;
        unsigned int crc = 0xFFFF;
        unsigned int polynome = 0xA001;

        for (; index < length; index++) {
            crc ^= *(frame + index);

            for (uint8_t idxBit = 0; idxBit <= 7; idxBit++) {
                parity = crc;
                crc >>= 1;

                if (parity%2 == true)
                    crc ^= polynome;
            }
        }

        return(crc);
    }
private:
    Order order;
    SocketTCP socket;

    std::thread threadSet;
    std::thread threadGet;
    std::atomic_bool running{true};

    uint8_t outBuf[9] = {};
    uint8_t inBuf[21] = {};
};
