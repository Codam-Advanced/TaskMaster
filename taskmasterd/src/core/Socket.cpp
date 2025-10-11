#include <taskmasterd/include/core/Socket.hpp>

#include <stdexcept>
#include <sys/socket.h>

namespace taskmasterd
{
Socket::Socket(Type type) : EventHandler(-1), _type(type)
{
    switch (type) {
    case Type::TCP:
        _fd = socket(AF_INET, SOCK_STREAM, 0);
        break;
    case Type::UDP:
        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        break;
    case Type::UNIX:
        _fd = socket(AF_UNIX, SOCK_STREAM, 0);
        break;
    default:
        throw std::invalid_argument("Invalid socket type");
    }

    if (_fd == -1) {
        throw std::runtime_error("Failed to create socket");
    }
}

Socket::Socket(Type type, i32 fd) : EventHandler(fd), _type(type) {}

void Socket::bind(const Address& address)
{
    if (::bind(_fd, &address.getSockAddr(), address.getSockAddrLen()) == -1) {
        throw std::runtime_error("Failed to bind socket");
    }
}

void Socket::listen(i32 backlog)
{
    if (::listen(_fd, backlog) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }
}

Socket Socket::accept()
{
    // TODO: Store client address if needed
    i32 fd = ::accept(_fd, nullptr, nullptr);
    if (fd == -1) {
        throw std::runtime_error("Failed to accept connection");
    }

    Socket socket(_type, fd);

    return socket;
}
} // namespace taskmasterd
