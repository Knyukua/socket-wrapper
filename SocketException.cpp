#include "SocketException.h"

SocketException::SocketException(const char* msg, int code)
{
	_msg = (std::string) "Socket exception: " + msg + "\nError code: " + std::to_string(code);
}

const char *SocketException::what() const noexcept
{
	return _msg.c_str();
}
