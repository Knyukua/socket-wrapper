#include "Socket.h"

#if __linux__
#include <errno.h>
#include <string.h>
#include <unistd.h>

constexpr int INVALID_SOCKET = -1;
constexpr int SOCKET_ERROR = -1;

constexpr auto* closesocket = close;
#endif // __linux__

// Create pointers to original functions to avoid naming conflicts
constexpr auto* _bind 		= bind;
constexpr auto* _listen 	= listen;
constexpr auto* _accept 	= accept;
constexpr auto* _connect 	= connect;
constexpr auto* _send 		= send;
constexpr auto* _sendto 	= sendto;
constexpr auto* _recv 		= recv;
constexpr auto* _recvfrom 	= recvfrom;

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

	if (_bind(_socket, address.getSockAddr(), sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		throw SocketException("bind failed", getLastErrorCode());
	}
}

void Socket::listen(int backlog)
{
	if (_listen(_socket, backlog) == SOCKET_ERROR)
	{
		throw SocketException("listen failed", getLastErrorCode());
	}
}

std::pair<Socket, SocketAddress> Socket::accept()
{
	sockaddr addr;
	unsigned int addrLen = sizeof(addr);
	SOCKET conn = _accept(_socket, &addr, &addrLen);
	if (conn == INVALID_SOCKET)
	{
		throw SocketException("accept failed", getLastErrorCode());
	}

	return { Socket(conn), SocketAddress(&addr) };
}

void Socket::connect(SocketAddress address)
{
	if (_connect(_socket, address.getSockAddr(), sizeof(sockaddr)) == SOCKET_ERROR)
	{
		throw SocketException("connect failed", getLastErrorCode());
	}
}

void Socket::send(std::string message)
{
	if (_send(_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR)
	{
		throw SocketException("send failed", getLastErrorCode());
	}
}

void Socket::sendto(std::string message, SocketAddress address)
{	
	if (_sendto(_socket, message.c_str(), message.length(), 0, address.getSockAddr(), sizeof(sockaddr)) == SOCKET_ERROR)
	{
		throw SocketException("sendto failed", getLastErrorCode());
	}
}

std::string Socket::recv(int bufsize)
{	
	char* buf = new char[bufsize];
	memset(buf, '\0', bufsize);
	if (_recv(_socket, buf, bufsize, 0) <= 0)
	{
		throw SocketException("recv failed", getLastErrorCode());
	}
	std::string ret = buf;
	delete[] buf;
	return ret;
}

std::pair<std::string, SocketAddress> Socket::recvfrom(int bufsize)
{	
	sockaddr from;
	unsigned int fromlen = sizeof(from);
	char* buf = new char[bufsize];
	if (_recvfrom(_socket, buf, bufsize, 0, &from, &fromlen) <= 0)
	{
		delete[] buf;
		throw SocketException("recvfrom failed", getLastErrorCode());
	}
	std::string ret = buf;
	delete[] buf;
	return { ret, SocketAddress(&from) };
}

void Socket::create(AddressFamily family, SocketType type)
{
	if (_socket != INVALID_SOCKET) return;

	_socket = socket((int)family, (int)type, 0);
	if (_socket == INVALID_SOCKET)
	{
		throw SocketException("socket creation failed", getLastErrorCode());
	}
}

void Socket::close()
{
	closesocket(_socket);
	_socket = INVALID_SOCKET;
}

int Socket::getLastErrorCode()
{
#if _WIN32
	return WSAGetLastError();
#elif __linux__
	return errno;
#endif
}
