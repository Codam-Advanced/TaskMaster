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

    /** Construct a new Address object.
     *
     * @param type The type of the address (IPv4, IPv6, UNIX).
     * @param address The address string (IP address or UNIX socket path).
     * @param port The port number (only for IPv4 and IPv6).
     */
    Address(Type type, const std::string& address, u16 port = 0);

    /** Get the underlying sockaddr structure.
     *
     * @return const sockaddr& The sockaddr structure representing the address.
     */
    const sockaddr& getSockAddr() const;

    /** Get the length of the sockaddr structure.
     *
     * @return socklen_t The length of the sockaddr structure.
     */
    socklen_t getSockAddrLen() const { return _addrlen; }

private:
    Type _type;

    sockaddr_storage _addr;
    socklen_t        _addrlen;
};
} // namespace taskmasterd
