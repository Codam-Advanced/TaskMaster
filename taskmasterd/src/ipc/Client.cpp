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
    EventManager::getInstance().registerEvent(*this, std::bind(&Client::handleRead, this), nullptr);

    LOG_INFO("New client connected with fd: " + std::to_string(_fd));
}

Client::~Client()
{
    if (this->isConnected()) {
        EventManager::getInstance().unregisterEvent(*this);
    }
}

void Client::handleRead()
{
    try {
        auto [bytes_read, message] = _proto_reader.read(*this);
        // If bytes_read is 0, the client has disconnected
        if (bytes_read == 0) {
            LOG_INFO("Client disconnected with fd: " + std::to_string(_fd));
            EventManager::getInstance().unregisterEvent(*this);
            this->close();
            return;
        }

        if (message.has_value()) {
            this->handleMessage(message.value());
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error reading from client fd " + std::to_string(_fd) + ": " + e.what());
        this->close();
        EventManager::getInstance().unregisterEvent(*this);
    }
}

void Client::handleMessage(proto::Command command)
{
    // Handle the received command
    LOG_INFO("Received command from client fd " + std::to_string(_fd) + ": " +
             command.DebugString());
    // TODO: Process the command as needed
}
} // namespace taskmasterd
