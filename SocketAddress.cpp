#include "SocketAddress.h"
#include "SocketException.h"


SocketAddress::SocketAddress(std::string ip, uint16_t port)
{
	_ip = ip;
	_port = port;

	_sockaddr4 = {};
	_sockaddr6 = {};

	if (inet_pton(AF_INET, ip.c_str(), &_sockaddr4.sin_addr) == 1)
	{
		_sockaddr4.sin_family = AF_INET;
		_sockaddr4.sin_port = htons(port);
		_family = AF_INET;
	}
	else if (inet_pton(AF_INET6, ip.c_str(), &_sockaddr6.sin6_addr) == 1)
	{
		_sockaddr6.sin6_family = AF_INET6;
		_sockaddr6.sin6_port = htons(port);
		_family = AF_INET6;
	}
	else
	{
		throw SocketException("couldn't create socket address");
	}
}

SocketAddress::SocketAddress(sockaddr* socketAddress)
{
	bool isIPv4 = socketAddress->sa_family == AF_INET;
	char* buf = new char[isIPv4 ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN];
	void* addrStruct = isIPv4 ? 
		(void*)&((sockaddr_in*)socketAddress)->sin_addr : 
		(void*)&((sockaddr_in6*)socketAddress)->sin6_addr;

	_sockaddr4 = {};
	_sockaddr6 = {};

	if (!inet_ntop(socketAddress->sa_family, addrStruct, buf, INET_ADDRSTRLEN))
	{
		delete[] buf;
		throw SocketException("couldn't create socket address from sockaddr pointer");
	}

	if (isIPv4)
	{
		_sockaddr4 = *(sockaddr_in*)socketAddress;
		_port = ntohs(_sockaddr4.sin_port);
	}
	else
	{
		_sockaddr6 = *(sockaddr_in6*)socketAddress;
		_port = ntohs(_sockaddr6.sin6_port);
	}
	_ip = buf;
	_family = socketAddress->sa_family;
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
	return _family == AF_INET ? (sockaddr*)&_sockaddr4 : (sockaddr*)&_sockaddr6;
}
