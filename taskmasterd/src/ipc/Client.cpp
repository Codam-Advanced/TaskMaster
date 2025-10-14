#include <taskmasterd/include/ipc/Client.hpp>

#include <logger/include/Logger.hpp>
#include <proto/taskmaster.pb.h>
#include <taskmasterd/include/core/EventManager.hpp>

#define BUFFER_SIZE 4096

namespace taskmasterd
{
Client::Client(Socket&& socket)
    // : ProtoReader<proto::Command>(std::move(socket))
    : Socket(std::move(socket))
{
    // EventManager::getInstance()->registerEvent(this, EventType::READ);

    LOG_INFO("New client connected with fd: " + std::to_string(_fd));
}

// void Client::handleMessage(proto::Command command)
// {
//     // Handle the received command
//     LOG_INFO("Received command from client fd " + std::to_string(_fd) + ": " +
//              command.DebugString());
//     // TODO: Process the command as needed
// }
} // namespace taskmasterd
