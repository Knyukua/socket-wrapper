#include "Socket.h"
#include <iostream>

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
	create(family, type);
}

Socket::~Socket()
{
}

void Socket::bind(SocketAddress address)
{
	extern int __stdcall bind(SOCKET socket, const sockaddr* name, int namelen);
	if (bind(_socket, address.getSockAddr(), sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		throw SocketException("bind failed", WSAGetLastError());
	}
}

void Socket::listen(int backlog)
{
	extern int __stdcall listen(SOCKET socket, int backlog);
	if (listen(_socket, backlog) == SOCKET_ERROR)
	{
		throw SocketException("listen failed", WSAGetLastError());
	}
}

std::pair<Socket, SocketAddress> Socket::accept()
{
	extern SOCKET __stdcall accept(SOCKET s, sockaddr* addr, int* addrlen);
	sockaddr addr;
	int addrLen = sizeof(addr);
	SOCKET conn = accept(_socket, &addr, &addrLen);
	if (conn == INVALID_SOCKET)
	{
		throw SocketException("accept failed", WSAGetLastError());
	}

	return {Socket(conn), SocketAddress(&addr)};
}

void Socket::send(std::string message)
{
	extern int __stdcall send(SOCKET s, const char* buf, int len, int flags);
	int result = send(_socket, message.c_str(), message.length(), 0);
	if (result == SOCKET_ERROR)
	{
		throw SocketException("send failed", WSAGetLastError());
	}
}

void Socket::sendto(std::string message, SocketAddress address)
{
	extern int __stdcall sendto(SOCKET s, const char* buf, int len, int flags, const sockaddr* to, int tolen);
	int result = sendto(_socket, message.c_str(), message.length(), 0, address.getSockAddr(), sizeof(sockaddr));
	if (result == SOCKET_ERROR)
	{
		throw SocketException("sendto failed", WSAGetLastError());
	}
}

std::string Socket::recv(int bufsize)
{
	extern int __stdcall recv(SOCKET s, char* buf, int len, int flags);
	char* buf = new char[bufsize];
	memset(buf, '\0', bufsize);
	int result = recv(_socket, buf, bufsize, 0);
	if (result <= 0)
	{
		delete[] buf;
		throw SocketException("recv failed", WSAGetLastError());
	}
	std::string ret = buf;
	delete[] buf;
	return ret;
}

std::pair<std::string, SocketAddress> Socket::recvfrom(int bufsize)
{
	extern int __stdcall recvfrom(SOCKET s, char* buf, int len, int flags, sockaddr* from, int* fromlen);
	sockaddr from;
	int fromlen = sizeof(from);
	char* buf = new char[bufsize];
	int result = recvfrom(_socket, buf, bufsize, 0, &from, &fromlen);
	if (result <= 0)
	{
		delete[] buf;
		throw SocketException("recvfrom failed", WSAGetLastError());
	}
	std::string ret = buf;
	delete[] buf;
	return {ret, SocketAddress(&from)};
}

void Socket::create(AddressFamily family, SocketType type)
{
	if (_socket != INVALID_SOCKET) return;

	_socket = socket((int)family, (int)type, 0);
	if (_socket == INVALID_SOCKET)
	{
		throw SocketException("socket creation failed", WSAGetLastError());
	}
}

void Socket::close()
{
	closesocket(_socket);
	_socket = INVALID_SOCKET;
}
