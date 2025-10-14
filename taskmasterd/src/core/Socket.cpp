#include <taskmasterd/include/core/Socket.hpp>

#include <stdexcept>
#include <sys/socket.h>

namespace taskmasterd
{
Socket::Socket(Type type) : FileDescriptor(-1), _type(type)
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
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }
}

Socket::Socket(Type type, i32 fd) : FileDescriptor(fd), _type(type) {}

void Socket::bind(const Address& address)
{
    const i32 reuse = 1;
    // Enable address reuse
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(i32)) != 0) {
        throw std::runtime_error("Failed to set SO_REUSEADDR option: " +
                                 std::string(strerror(errno)));
    }

    if (::bind(_fd, &address.getSockAddr(), address.getSockAddrLen()) == -1) {
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }
}

void Socket::listen(i32 backlog)
{
    if (::listen(_fd, backlog) == -1) {
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }
}

Socket Socket::accept()
{
    // TODO: Store client address if needed
    i32 fd = ::accept(_fd, nullptr, nullptr);
    if (fd == -1) {
        throw std::runtime_error("Failed to accept connection: " + std::string(strerror(errno)));
    }

    Socket socket(_type, fd);

    return socket;
}
} // namespace taskmasterd
