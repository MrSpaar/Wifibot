#pragma once
#include <giomm.h>


class SocketTCP {
public:
	~SocketTCP();

	void open(const std::string &ip, int port);
	bool isOpened();
	void close();
	void send(const char* p_data, int size) const;
	void send(std::string str_data) const;
	void receive(char* p_data, int size);
private:
	Glib::RefPtr<Gio::Socket> socket;
	Glib::RefPtr<Gio::InetSocketAddress> address;
	Glib::RefPtr<Gio::Cancellable> cancellable;
};
