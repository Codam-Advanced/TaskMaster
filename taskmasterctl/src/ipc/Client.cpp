#include <ipc/include/Address.hpp>
#include <ipc/include/ProtoWriter.hpp>
#include <ipc/include/ProtoReader.hpp>
#include <logger/include/Logger.hpp>
#include <taskmasterctl/include/ipc/Client.hpp>

#include <proto/taskmaster.pb.h>

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
    ipc::ProtoWriter<proto::Command> writer;

    // Continue writing the proto::Command to the socket till it has successfully
    // sent over everything to the Daemon.
    writer.init(command);
    while (!writer.write(socket))
        continue ;

    LOG_DEBUG("Successfully sent the command to the daemon")
}

// TODO make nice
void awaitDaemonResponse(ipc::Socket& socket)
{
    ipc::ProtoReader<proto::CommandResponse> protoReader;

    while (true)
    {
        auto res = protoReader.read(socket);

        // Keep reading till we have the full response
        if (!res.second.has_value())
            continue ;
        proto::CommandResponse& response = res.second.value();

        if (response.status() != proto::CommandStatus::OK)
        {
            if (response.message().size() != 0)
                LOG_ERROR(response.message())
        }
        if (response.status() == proto::CommandStatus::OK)
        {
            if (response.message().size() != 0)
                std::cout << response.message() << std::endl;
        }

        return ;
    }
}

} // namespace taskmasterctl
