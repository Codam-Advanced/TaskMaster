#include <ipc/include/Address.hpp>
#include <ipc/include/ProtoReader.hpp>
#include <ipc/include/ProtoWriter.hpp>
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
        continue;

    LOG_DEBUG("Successfully sent the command to the daemon")
}

using ResponseReader       = ipc::ProtoReader<proto::CommandResponse>;
using ResponseReaderReturn = std::pair<isize, std::optional<proto::CommandResponse>>;
bool awaitDaemonResponse(ipc::Socket& socket, proto::Command& command)
{
    ResponseReaderReturn res = ResponseReaderReturn(0, std::nullopt);
    ResponseReader       protoReader;

    while (!res.second.has_value())
        res = protoReader.read(socket);

    proto::CommandResponse& response = res.second.value();

    switch (response.status()) {
    case proto::CommandStatus::OK:
        if (response.message().size() != 0)
            std::cout << response.message() << std::endl;
        if (command.type() == proto::CommandType::TERMINATE)
            return true;
        return false;
    case proto::CommandStatus::ERROR:
        if (response.message().size() != 0)
            LOG_ERROR(response.message())
        return false;
    case proto::CommandStatus::TYPE_ERROR:
        if (response.message().size() != 0)
            LOG_ERROR(response.message())
        return false;
    case proto::CommandStatus::TOO_MANY_ARGUMENTS:
    case proto::CommandStatus::ARGUMENT_ERROR:
        if (response.message().size() != 0)
            LOG_WARNING(response.message())
        return false;
    default:
        throw std::runtime_error("Received an invalid response status");
    }
}

} // namespace taskmasterctl
