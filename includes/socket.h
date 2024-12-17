/*
* Classe Socket_UDP and Socket_TCP
* Package : Gtkmm
* -std=c++11
*/


#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <glibmm/refptr.h>
#include <giomm/socket.h>
#include <giomm/inetsocketaddress.h>
#include <giomm/inetaddress.h>
#include <giomm/socketaddress.h>
#include <giomm/cancellable.h>
#include <giomm/error.h>


class Socket_UDP {
private:
	Glib::RefPtr<Gio::Socket> m_p_socket;
	Glib::RefPtr<Gio::InetSocketAddress> m_p_address_port;

public:
	Socket_UDP();
	~Socket_UDP();
	void open(std::string ip, int port);
	void close();
	bool is_open() const;
	void send(const   char* data, int size) const;
	void send(std::string str) const;
};


class Socket_TCP {
private:
	Glib::RefPtr<Gio::Socket> m_p_socket;
	Glib::RefPtr<Gio::InetSocketAddress> m_p_address_port;
	Glib::RefPtr<Gio::Cancellable> m_p_cancellable;

public:
	Socket_TCP();
	~Socket_TCP();
	void open(std::string ip, int port);
	void close();
	bool is_open() const;
	void send(const char* p_data, int size) const;
	void send(std::string str_data) const;

	void receive(char* p_data, int size);
};

#endif // _SOCKET_H_
