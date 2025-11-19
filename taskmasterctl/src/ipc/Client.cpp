#include <ipc/include/Address.hpp>
#include <logger/include/Logger.hpp>
#include <taskmasterctl/include/ipc/Client.hpp>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/taskmasterd.sock"

namespace taskmasterctl
{

ipc::Socket connectToDaemon()
{
    ipc::Address address(ipc::Address::Type::UNIX, SOCKET_PATH);
    ipc::Socket  socket(ipc::Socket::Type::UNIX);

    try {
        socket.connect(address);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to connect to the daemon, is it running?");
    }

    return socket;
}

void sendCommandToDaemon(ipc::Socket& socket, proto::Command& command)
{
    // Setup the serialized message to send to the daemon
    std::string serializedMessage;
    if (!command.SerializeToString(&serializedMessage))
        throw std::runtime_error("Failed to serialize the command");

    // Send the size of the message so the daemon knows how many bytes to expect
    uint32_t size = htonl(serializedMessage.size());
    if (send(socket.getFd(), &size, sizeof(size), 0) == -1)
        throw std::runtime_error("Failed to send the message size");
    LOG_DEBUG("Successfully sent the message size to the daemon")

    // Send the serialized command
    if (send(socket.getFd(), serializedMessage.data(), serializedMessage.size(), 0) == -1)
        throw std::runtime_error("Failed to send the command message");
    LOG_DEBUG("Successfully sent the command to the daemon")
}

void awaitDaemonResponse(ipc::Socket& socket)
{
    (void) socket;
    // TODO
}

} // namespace taskmasterctl
