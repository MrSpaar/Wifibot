#include <cmath>
#include <thread>
#include <unistd.h>
#include <iostream>

#include "includes/wifibot.h"


WifibotData::WifibotData(char data[21]) {
    speed_l = (data[1] << 8) + data[0];
    speed_r = (data[10] << 8) + data[9];
    battery_level = data[2];
    ir1_l = data[3];
    ir2_l = data[4];
    ir1_r = data[11];
    ir2_r = data[12];
    current = data[17];
    version = data[18];

    odometry_l = ((((long)data[8] << 24)) + (((long)data[7] << 16))
                                          + (((long)data[6] << 8))
                                          + ((long)data[5]));

    odometry_r = ((((long)data[16] << 24)) + (((long)data[15] << 16))
                                           + (((long)data[14] << 8))
                                           + ((long)data[13]));
}


Wifibot::Wifibot():
	m_order(),
	m_p_thread_set(NULL),
	m_p_thread_get(NULL),
	m_stop(false),
	m_socket() {}

Wifibot::~Wifibot() {
	this->disconnect();
}

void Wifibot::stop() {
	m_order.set_order(0,0);
	std::cout << "Stop()" << std::endl;
}

void Wifibot::speed_up() {
	if (m_order.get_order_L() == m_order.get_order_R()) {
		m_order.set_order(m_order.get_order_L()+10, m_order.get_order_R()+10);
	} else {
		int moy = m_order.get_order_R() + m_order.get_order_L()/2;
		m_order.set_order(moy+10, moy+10);
	}

	std::cout << "UP" << std::endl;
	std::cout << "Gauche : " << m_order.get_order_L() << "    Droite : " << m_order.get_order_R() << std::endl;
}

void Wifibot::speed_down() {
	if (m_order.get_order_L() == m_order.get_order_R()) {
		m_order.set_order(m_order.get_order_L()-10, m_order.get_order_R()-10);
	} else {
		int moy = m_order.get_order_R() + m_order.get_order_L()/2;
		m_order.set_order(moy-10, moy-10);
	}

	std::cout << "DOWN" << std::endl;
	std::cout << "Gauche : " << m_order.get_order_L() << "    Droite : " << m_order.get_order_R() << std::endl;

}

void Wifibot::turn(int direction) {
	std::cout << "TURN" << std::endl;
	if (direction == +1) {
		m_order.set_order(m_order.get_order_L()*1.2, m_order.get_order_R()*0.8);
		std::cout << "Tourne à Droite" << std::endl;
		std::cout << "Gauche : " << m_order.get_order_L() << "    Droite : " << m_order.get_order_R() << std::endl;
	} else {
		m_order.set_order(m_order.get_order_L()*0.8, m_order.get_order_R()*1.2);
		std::cout << "Tourne à Gauche" << std::endl;
		std::cout << "Gauche : " << m_order.get_order_L() << "    Droite : " << m_order.get_order_R() << std::endl;
	}
}

void Wifibot::rotate(int direction) {
	std::cout << "ROTATE" << std::endl;
	this->stop();
	usleep(1000000);

	if (direction==+1) {
		m_order.set_order(10, -10);
		std::cout << "Rotation à Droite" << std::endl;
		std::cout << "Gauche : " << m_order.get_order_L() << "    Droite : " << m_order.get_order_R() << std::endl;
	} else {
		m_order.set_order(-10, 10);
		std::cout << "Rotation à Gauche" << std::endl;
		std::cout << "Gauche : " << m_order.get_order_L() << "    Droite : " << m_order.get_order_R() << std::endl;
	}
}

void Wifibot::start_set_thread() {
	static int cpt;
	short crc;

	while (m_stop == false) {
		std::cout << "Thread [send] : " << ++cpt << std::endl;
		bool speed_ctr = m_order.get_speed_ctr();
		bool sens_right = ((m_order.get_order_R() >=0) ? true :false);
		short speed_right = abs(m_order.get_order_R());
		bool sens_left = ((m_order.get_order_L() >=0) ? true :false);
		short speed_left = abs(m_order.get_order_L());

		m_output_buf[0] = 0xFF;
		m_output_buf[1] = 0x07;
		m_output_buf[2] = speed_left & 0xFF;
		m_output_buf[3] = (speed_left >> 8) & 0xFF;
		m_output_buf[4] = speed_right & 0xFF;
		m_output_buf[5] = (speed_right >> 8) & 0xFF;
		m_output_buf[6] = (char) (128*speed_ctr+64*sens_left+32*speed_ctr+16*sens_right+8);

		trame_crc[0] = (unsigned char) m_output_buf[1];
		trame_crc[1] = (unsigned char) m_output_buf[2];
		trame_crc[2] = (unsigned char) m_output_buf[3];
		trame_crc[3] = (unsigned char) m_output_buf[4];
		trame_crc[4] = (unsigned char) m_output_buf[5];
		trame_crc[5] = (unsigned char) m_output_buf[6];

		crc = crc16(trame_crc,6);
		m_output_buf[7] = crc & 0xFF;
		m_output_buf[8] = (crc >> 8) & 0xFF;

		m_socket.send(m_output_buf,9);
		std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
	}

	std::cout << "Thread [send] : stop !" << std::endl << std::endl;
}

void Wifibot::start_get_thread() {
    short crc;

    while (m_stop == false) {
        m_socket.receive(m_input_buf, 21);
        crc = (m_input_buf[20] << 8) | (m_input_buf[19] & 0xFF);

        if (crc != crc16(reinterpret_cast<unsigned char*>(m_input_buf), 19))
            continue;

        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_TIME));
    }
}

void Wifibot::connect(std::string ip) {
	m_socket.open(ip, PORT);
	std::cout << "Wifibot connect()" << std::endl;

	if (m_socket.is_open()) {
	    // m_p_thread_set = new std::thread([this]() { this->run(); });
		m_p_thread_get = new std::thread([this]() { this->start_get_thread(); });
	}
}

WifibotData Wifibot::get_data() {
    return { m_input_buf };
}

void Wifibot::disconnect() {
	m_stop = true;

	if (m_p_thread_get != NULL) {
	   m_p_thread_get->join();
	   delete m_p_thread_get;
	}

	m_socket.close();
}

short Wifibot::crc16(unsigned char* trame, unsigned int longueur) {
	unsigned int Crc = 0xFFFF;
	unsigned int Polynome = 0xA001;
	unsigned int CptOctet = 0;
	unsigned int CptBit = 0;
	unsigned int Parity = 0;

	for (CptOctet= 0 ; CptOctet < longueur ; CptOctet++) {
		Crc ^= *( trame + CptOctet);

		for (CptBit = 0; CptBit <= 7 ; CptBit++) {
			Parity= Crc;
			Crc >>= 1;
			if (Parity%2 == true) Crc ^= Polynome;
		}
	}

	return(Crc);
}
