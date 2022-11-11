#include <WS2tcpip.h>
#include <regex>
#include "SocketAddress.h"

#include <iostream>

SocketAddress::SocketAddress(std::string ip, uint16_t port, AddressFamily family)
{
	std::regex ipRegex("/^(?>(?>([a-f0-9]{1,4})(?>:(?1)){7}|(?!(?:.*[a-f0-9](?>:|$)){8,})((?1)(?>:(?1)){0,6})?::(?2)?)|(?>(?>(?1)(?>:(?1)){5}:|(?!(?:.*[a-f0-9]:){6,})(?3)?::(?>((?1)(?>:(?1)){0,4}):)?)?(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])(?>\\.(?4)){3}))$/iD");

	if (std::regex_match(ip, ipRegex))
	{
		std::cout << "Match\n";
	}

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
