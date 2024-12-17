#include <iostream>
#include "includes/socket.h"


/***** SOCKET UDP *****/
Socket_UDP::Socket_UDP() :
	m_p_socket(nullptr),
	m_p_address_port(nullptr) {}

Socket_UDP::~Socket_UDP() {
	if(m_p_socket && m_p_address_port)
		m_p_socket->close();
}

void Socket_UDP::open(std::string ip, int port) {
	m_p_socket = Gio::Socket::create(Gio::SOCKET_FAMILY_IPV4,
	                                 Gio::SOCKET_TYPE_DATAGRAM,
	                                 Gio::SOCKET_PROTOCOL_UDP);
	m_p_address_port = Gio::InetSocketAddress::create(Gio::InetAddress::create(ip), port);
	if(m_p_socket && m_p_address_port) {
		try {
			m_p_socket->connect(m_p_address_port);
		}
		catch (const Gio::Error &e)  {
			std::cerr << "[ exeption connect() ] Code : " << e.code()
				      << ", Error : " << e.what() << std::endl;
		}
	}
}

void Socket_UDP::close() {
	if(m_p_socket && m_p_address_port)
		m_p_socket->close();
}

bool Socket_UDP::is_open() const {
	if(m_p_socket && m_p_address_port)
		return m_p_socket->is_connected();
	else
		return false;
}

void Socket_UDP::send(const char* p_data, int size) const {
	if(m_p_socket && m_p_address_port) {
		if(m_p_socket->is_connected() ) {
			try {
				m_p_socket->send_to(m_p_address_port, p_data, size);
			}
			catch (const Gio::Error &e) {
				std::cerr << "[ exeption send_to() ] Code : " << e.code()
					      << ", Error : " << e.what() << std::endl;
			}
		}
	}
}

void Socket_UDP::send(std::string str_data) const {
	if(m_p_socket && m_p_address_port) {
		int size = str_data.size();
		const char* p_data = str_data.c_str();
		if(m_p_socket->is_connected() ) {
			try {
				m_p_socket->send_to(m_p_address_port, p_data, size);
			}
			catch (const Gio::Error &e) {
				std::cerr << "[ exeption send_to() ] Code : " << e.code()
					      << ", Error : " << e.what() << std::endl;
			}
		}
	}
}

/***** SOCKET TCP *****/
Socket_TCP::Socket_TCP() :
	m_p_socket (nullptr),
	m_p_address_port(nullptr),
	m_p_cancellable (nullptr)
{
}

Socket_TCP::~Socket_TCP() {
	if(m_p_socket && m_p_address_port && m_p_cancellable)
		m_p_socket->close();
}

void Socket_TCP::open(std::string ip, int port) {
	m_p_cancellable = Gio::Cancellable::create();
	m_p_socket = Gio::Socket::create(Gio::SOCKET_FAMILY_IPV4,
	                                 Gio::SOCKET_TYPE_STREAM,
	                                 Gio::SOCKET_PROTOCOL_TCP);
	m_p_address_port = Gio::InetSocketAddress::create(Gio::InetAddress::create(ip), port);
	if(m_p_socket && m_p_address_port && m_p_cancellable) {
		try {
			m_p_socket->connect(m_p_address_port, m_p_cancellable);
		}
		catch (const Gio::Error &e)  {
			std::cerr << "[ exeption connect() ] Code : " << e.code()
				      << ", Error : " << e.what() << std::endl;
		}
	}
}

void Socket_TCP::close() {
	if(m_p_socket && m_p_address_port && m_p_cancellable) {
		m_p_cancellable->cancel();
		//m_p_socket->shutdown(true, true);
		m_p_socket->close();
	}
}

bool Socket_TCP::is_open() const {
	if(m_p_socket && m_p_address_port)
		return m_p_socket->is_connected();
	else
		return false;
}

void Socket_TCP::send(const  char* p_data, int size) const {
	if(m_p_socket && m_p_address_port && m_p_cancellable) {
		if(m_p_socket->is_connected() ) {
			try {
				m_p_socket->send_to(m_p_address_port, p_data, size, m_p_cancellable);
			}
			catch (const Gio::Error &e) {
				std::cerr << "[ exeption send_to() ] Code : " << e.code()
					      << ", Error : " << e.what() << std::endl;
			}
		}
	}
}

void Socket_TCP::send(std::string str_data) const {
	if(m_p_socket && m_p_address_port && m_p_cancellable) {
		int size = str_data.size();
		const char* p_data = str_data.c_str();
		if(m_p_socket->is_connected() ) {
			try {
				m_p_socket->send_to(m_p_address_port, p_data, size, m_p_cancellable);
			}
			catch (const Gio::Error &e) {
				std::cerr << "[ exeption send_to() ] Code : " << e.code()
					      << ", Error : " << e.what() << std::endl;
			}
		}
	}
}

void Socket_TCP::receive( char* p_data, int size) {
	if(m_p_socket && m_p_address_port) {
		if(m_p_socket->is_connected() ) {
			try {
				Glib::RefPtr<Gio::SocketAddress> p_socket_address = m_p_socket->get_remote_address();
				// get_local_address()  // adresse locale
				// get_remote_address() // adresse distante
				m_p_socket->receive_from(p_socket_address, p_data, size, m_p_cancellable);
			}
			catch (const Gio::Error &e) {
				std::cerr << "[ exeption receive() ] Code : " << e.code()
					      << ", Error : " << e.what() << std::endl;
			}
		}
	}
}
