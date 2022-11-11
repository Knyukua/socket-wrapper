#pragma once
#include <utility>
#include "SocketAddress.h"
#include "SocketException.h"

#if __linux__
typedef int SOCKET;
#else
#ifndef _WIN32
#error Unsupported platform
#endif
#endif

class Socket
{
private:
	Socket(SOCKET sock);

public:
	Socket();
	Socket(AddressFamily family, SocketType type);

	void bind(SocketAddress address);
	void listen(int backlog);
	std::pair<Socket, SocketAddress> accept();
	void connect(SocketAddress address);

	void send(std::string message);
	void sendto(std::string message, SocketAddress address);

	std::string recv(int bufsize);
	std::pair<std::string, SocketAddress> recvfrom(int bufsize);

	void create(AddressFamily family, SocketType type);
	void close();

private:
	SOCKET _socket;

	int getLastErrorCode();
};