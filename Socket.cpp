#include "Socket.h"

#if _WIN32
typedef int addrlen_t;

#elif __linux__
#include <errno.h>
#include <string.h>
#include <unistd.h>

typedef unsigned int addrlen_t;

constexpr int INVALID_SOCKET = -1;
constexpr int SOCKET_ERROR = -1;

constexpr auto closesocket = close;
#endif

// Create pointers to original functions to avoid naming conflicts
constexpr auto _bind 		= bind;
constexpr auto _listen 		= listen;
constexpr auto _accept 		= accept;
constexpr auto _connect 	= connect;
constexpr auto _send 		= send;
constexpr auto _sendto 		= sendto;
constexpr auto _recv 		= recv;
constexpr auto _recvfrom 	= recvfrom;

addrlen_t getAddrLen(const sockaddr* addr)
{
	if (addr->sa_family == AF_INET) return sizeof(sockaddr_in);
	else return sizeof(sockaddr_in6);
}

Socket::Socket(SOCKET sock)
{
	_socket = sock;
}

Socket::Socket()
{
	_socket = INVALID_SOCKET;
}

Socket::Socket(AddressFamily family, SocketType type)
{
	_socket = INVALID_SOCKET;
	create(family, type);
}

void Socket::bind(SocketAddress address)
{
	addrlen_t addrlen = getAddrLen(address.getSockAddr());
	if (_bind(_socket, address.getSockAddr(), addrlen) == SOCKET_ERROR)
	{
		throw SocketException("bind failed");
	}
}

void Socket::listen(int backlog)
{
	if (_listen(_socket, backlog) == SOCKET_ERROR)
	{
		throw SocketException("listen failed");
	}
}

std::pair<Socket, SocketAddress> Socket::accept()
{
	sockaddr* addr = _family == AF_INET ? (sockaddr*)new sockaddr_in : (sockaddr*)new sockaddr_in6;
	addrlen_t addrLen = _family == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
	SOCKET conn = _accept(_socket, addr, &addrLen);
	if (conn == INVALID_SOCKET)
	{
		throw SocketException("accept failed");
	}

	Socket connSocket(conn);
	connSocket._family = _family;
	SocketAddress address(addr);
	delete addr;

	return { connSocket, address };
}

void Socket::connect(SocketAddress address)
{
	addrlen_t addrlen = getAddrLen(address.getSockAddr());
	if (_connect(_socket, address.getSockAddr(), addrlen) == SOCKET_ERROR)
	{
		throw SocketException("connect failed");
	}
}

void Socket::send(std::string message)
{
	if (_send(_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR)
	{
		throw SocketException("send failed");
	}
}

void Socket::sendto(std::string message, SocketAddress address)
{
	addrlen_t addrlen = getAddrLen(address.getSockAddr());
	if (_sendto(_socket, message.c_str(), message.length(), 0, address.getSockAddr(), addrlen) == SOCKET_ERROR)
	{
		throw SocketException("sendto failed");
	}
}

std::string Socket::recv(int bufsize)
{	
	char* buf = new char[bufsize];
	memset(buf, '\0', bufsize);
	if (_recv(_socket, buf, bufsize, 0) <= 0)
	{
		throw SocketException("recv failed");
	}
	std::string ret = buf;
	delete[] buf;
	return ret;
}

std::pair<std::string, SocketAddress> Socket::recvfrom(int bufsize)
{	
	sockaddr from;
	addrlen_t fromlen = sizeof(from);
	char* buf = new char[bufsize];
	if (_recvfrom(_socket, buf, bufsize, 0, &from, &fromlen) <= 0)
	{
		delete[] buf;
		throw SocketException("recvfrom failed");
	}
	std::string ret = buf;
	delete[] buf;
	return { ret, SocketAddress(&from) };
}

void Socket::create(AddressFamily family, SocketType type)
{
	if (_socket != INVALID_SOCKET) return;

	_socket = socket((int)family, (int)type, 0);
	_family = (int)family;
	if (_socket == INVALID_SOCKET)
	{
		throw SocketException("socket creation failed");
	}
}

void Socket::close()
{
	closesocket(_socket);
	_socket = INVALID_SOCKET;
}
