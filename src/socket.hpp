#pragma once

#include <giomm/socket.h>
#include <giomm/inetsocketaddress.h>


class SocketTCP {
public:
    ~SocketTCP() {
        close();
    }

    bool open(const std::string &ip, int port) {
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

    void close() {
        cancellable->cancel();
        socket->close();
    }

    void send(uint8_t* data, size_t size) const {
        if (!socket->is_connected())
           return;

        socket->send_to(address, (const char*) data, size, cancellable);
    }

    void receive(uint8_t* data, size_t size) {
        if (!socket->is_connected())
           return;

        Glib::RefPtr<Gio::SocketAddress> p_socket_address = socket->get_remote_address();
        socket->receive_from(p_socket_address, (char*) data, size, cancellable);
    }
private:
    Glib::RefPtr<Gio::Socket> socket;
    Glib::RefPtr<Gio::InetSocketAddress> address;
    Glib::RefPtr<Gio::Cancellable> cancellable;
};
