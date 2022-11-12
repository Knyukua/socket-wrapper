#pragma once
#include <cstdint>
#include <string>

#if _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")
#elif __linux__
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#endif


class SocketAddress
{
private:
    friend class Socket;
    SocketAddress(sockaddr* socketAddress);

public:
    SocketAddress(std::string ip, uint16_t port);

    const std::string& ip() const;
    uint16_t port() const;
    int family() const;

private:	
    const sockaddr* getSockAddr() const;

    std::string _ip;
    uint16_t _port;
    int _family;

    sockaddr_in _sockaddr4;
    sockaddr_in6 _sockaddr6;
};