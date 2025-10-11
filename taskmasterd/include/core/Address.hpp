#pragma once

#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

#include <utils/include/utils.hpp>

namespace taskmasterd
{
class Address
{
public:
    enum class Type : sa_family_t
    {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX
    };

    Address(Type type, const std::string& address, u16 port = 0);

    const sockaddr& getSockAddr() const;
    socklen_t       getSockAddrLen() const { return _addrlen; }

private:
    Type _type;

    sockaddr_storage _addr;
    socklen_t        _addrlen;
};
} // namespace taskmasterd
