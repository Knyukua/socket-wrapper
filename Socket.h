#pragma once
#include <utility>
#include "SocketAddress.h"
#include "SocketException.h"

class Socket
{
private:
	Socket(SOCKET sock);

public:
	Socket();
	Socket(AddressFamily family, SocketType type);
	~Socket();

	void bind(SocketAddress address);
	void listen(int backlog);
	std::pair<Socket, SocketAddress> accept();

	void send(std::string message);
	void sendto(std::string message, SocketAddress address);

	std::string recv(int bufsize);
	std::pair<std::string, SocketAddress> recvfrom(int bufsize);

	void create(AddressFamily family, SocketType type);
	void close();

private:
	SOCKET _socket;
};