#include "SocketException.h"

#if _WIN32
#include <WinSock2.h>
#elif __linux__
#include <errno.h>
#endif

SocketException::SocketException(const char* msg)
{
	_msg = (std::string)"Socket exception: " + msg + "\nError code: " + std::to_string(getErrorCode());
}

const char *SocketException::what() const noexcept
{
	return _msg.c_str();
}

int SocketException::getErrorCode()
{
#if _WIN32
	return WSAGetLastError();
#elif __linux
	return errno;
#endif
}
