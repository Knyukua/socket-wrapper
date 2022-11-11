#pragma once
#include <cstdint>
#include <string>
#include "SocketEnums.h"

#if _WIN32
#include <WinSock2.h>
#elif __linux__
#include <sys/socket.h>
#include <netinet/ip.h>
#endif


class SocketAddress
{
private:
	friend class Socket;
	SocketAddress(sockaddr* socketAddress);

public:
	SocketAddress(std::string ip, uint16_t port, AddressFamily family);

	const std::string& ip() const;
	uint16_t port() const;
	int family() const;

private:	
	const sockaddr* getSockAddr() const;

	std::string _ip;
	uint16_t _port;
	int _family;

	sockaddr_in _sockaddr;
};