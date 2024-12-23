#include "includes/socket.h"


bool SocketTCP::open(const std::string &ip, int port) {
    socket = Gio::Socket::create(
        Gio::SocketFamily::IPV4,
        Gio::Socket::Type::STREAM,
        Gio::Socket::Protocol::TCP
    );

    cancellable = Gio::Cancellable::create();
    address = Gio::InetSocketAddress::create(Gio::InetAddress::create(ip), port);

    if (!socket || !cancellable || !address) {
        return false;
    }

    try {
        socket->connect(address, cancellable);
        return socket->is_connected();
    } catch (const Glib::Error &e) {
       return false;
    }
}

void SocketTCP::send(const char* p_data, int size) const {
    if (!socket->is_connected())
       return;

    socket->send_to(address, p_data, size, cancellable);
}

void SocketTCP::receive(char* p_data, int size) {
    if (!socket->is_connected())
       return;

    Glib::RefPtr<Gio::SocketAddress> p_socket_address = socket->get_remote_address();
    socket->receive_from(p_socket_address, p_data, size, cancellable);
}

void SocketTCP::close() {
    if (socket->is_connected()) {
        cancellable->cancel();
        socket->close();
    }
}

SocketTCP::~SocketTCP() {
    close();
}
