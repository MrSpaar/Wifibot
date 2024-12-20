#include <stdexcept>
#include "includes/socket.h"


void SocketTCP::open(const std::string &ip, int port) {
	socket = Gio::Socket::create(
	    Gio::SocketFamily::IPV4,
	    Gio::Socket::Type::STREAM,
		Gio::Socket::Protocol::TCP
	);

	cancellable = Gio::Cancellable::create();
	address = Gio::InetSocketAddress::create(Gio::InetAddress::create(ip), port);

	if (!socket || !address || !cancellable)
	   throw std::runtime_error("Could not allocate socket");

	socket->connect(address, cancellable);
}

bool SocketTCP::isOpened() {
    return socket->is_connected();
}

void SocketTCP::close() {
	cancellable->cancel();
	socket->close();
}

void SocketTCP::send(const  char* p_data, int size) const {
	if (!socket->is_connected())
	   return;

	socket->send_to(address, p_data, size, cancellable);
}

void SocketTCP::send(std::string str_data) const {
    if (!socket->is_connected())
	   return;

	int size = str_data.size();
	const char* p_data = str_data.c_str();
	socket->send_to(address, p_data, size, cancellable);
}

void SocketTCP::receive( char* p_data, int size) {
	if (!socket->is_connected())
	   return;

	Glib::RefPtr<Gio::SocketAddress> p_socket_address = socket->get_remote_address();
	socket->receive_from(p_socket_address, p_data, size, cancellable);
}

SocketTCP::~SocketTCP() {
	if(socket->is_connected())
		socket->close();
}
