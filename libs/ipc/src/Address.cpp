#include <ipc/include/Address.hpp>

#include <stdexcept>
#include <string.h>

namespace taskmasterd
{
Address::Address(Type type, const std::string& address, u16 port) : _type(type)
{
    _addr.ss_family = static_cast<sa_family_t>(type);

    switch (type) {
    case Type::IPv4: {
        struct sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>(&_addr);
        if (inet_pton(AF_INET, address.c_str(), &addr_in->sin_addr) != 1) {
            throw std::invalid_argument("Invalid IPv4 address: " + address);
        }
        // Convert port to network byte order
        addr_in->sin_port = htons(port);
        _addrlen          = sizeof(struct sockaddr_in);
        break;
    }
    case Type::IPv6: {
        struct sockaddr_in6* addr_in6 = reinterpret_cast<struct sockaddr_in6*>(&_addr);
        if (inet_pton(AF_INET6, address.c_str(), &addr_in6->sin6_addr) != 1) {
            throw std::invalid_argument("Invalid IPv6 address: " + address);
        }
        // Convert port to network byte order
        addr_in6->sin6_port = htons(port);
        _addrlen            = sizeof(struct sockaddr_in6);
        break;
    }
    case Type::UNIX: {
        struct sockaddr_un* addr_un = reinterpret_cast<struct sockaddr_un*>(&_addr);
        if (address.length() >= sizeof(addr_un->sun_path)) {
            throw std::invalid_argument("UNIX socket path too long: " + address);
        }
        memset(addr_un->sun_path, 0, sizeof(addr_un->sun_path));
        memcpy(addr_un->sun_path, address.c_str(), address.length());
        _addrlen = offsetof(struct sockaddr_un, sun_path) + address.length() + 1;
        break;
    }
    default:
        throw std::invalid_argument("Invalid address type");
    }
}

const sockaddr& Address::getSockAddr() const
{
    return *reinterpret_cast<const struct sockaddr*>(&_addr);
}
} // namespace taskmasterd
