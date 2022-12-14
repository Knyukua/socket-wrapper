#pragma once
#include <exception>
#include <string>

class SocketException : public std::exception
{
public:
    SocketException(const char* msg);

    const char* what() const noexcept override;

private:
    std::string _msg;

    int getErrorCode();
};