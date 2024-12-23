#pragma once

#include <giomm/socket.h>
#include <giomm/inetsocketaddress.h>


class SocketTCP {
public:
    ~SocketTCP();

    bool open(const std::string &ip, int port);
    void close();

    void send(const char* p_data, int size) const;
    void receive(char* p_data, int size);
private:
    Glib::RefPtr<Gio::Socket> socket;
    Glib::RefPtr<Gio::InetSocketAddress> address;
    Glib::RefPtr<Gio::Cancellable> cancellable;
};
