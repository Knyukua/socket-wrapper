#include "SocketAddress.h"

#if _WIN32
#include <WS2tcpip.h>
#elif __linux__
#include <arpa/inet.h>
#endif

SocketAddress::SocketAddress(std::string ip, uint16_t port, AddressFamily family)
{
	_ip = ip;
	_port = port;
	_family = (int)family;

	inet_pton(_family, ip.c_str(), &_sockaddr.sin_addr);
	_sockaddr.sin_family = _family;
	_sockaddr.sin_port = htons(port);
}

SocketAddress::SocketAddress(sockaddr* socketAddress)
{
	_sockaddr = *(sockaddr_in*)socketAddress;
	_port = ntohs(_sockaddr.sin_port);
	_family = _sockaddr.sin_family;

	int bufsize = _family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
	char *buf = new char[bufsize];
	inet_ntop(_family, &_sockaddr.sin_addr, buf, bufsize);
	_ip = buf;
	delete[] buf;
}

const std::string& SocketAddress::ip() const
{
	return _ip;
}

uint16_t SocketAddress::port() const
{
	return _port;
}

int SocketAddress::family() const
{
	return _family;
}

const sockaddr* SocketAddress::getSockAddr() const
{
	return (sockaddr*)&_sockaddr;
}
