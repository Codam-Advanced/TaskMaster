#include <taskmasterd/include/core/Globals.hpp>
#include <taskmasterd/include/ipc/Client.hpp>

#include <taskmasterd/include/ipc/Server.hpp>
#include <logger/include/Logger.hpp>
#include <proto/taskmaster.pb.h>
#include <taskmasterd/include/core/EventManager.hpp>

#define BUFFER_SIZE 4096

namespace taskmasterd
{
Client::Client(Socket&& socket, Server& server)
    // : ProtoReader<proto::Command>(std::move(socket))
    : Socket(std::move(socket))
    , _server(server)
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
        EventManager::getInstance().unregisterEvent(*this);
        this->close();
    }
}

void Client::handleWrite(proto::Command command)
{
    try {
        bool doneWriting = _proto_writer.write(*this);
        if (doneWriting) {
            // Stop polling for writes and start polling for reads again
            EventManager::getInstance().unregisterEvent(*this);
            EventManager::getInstance().registerEvent(*this, std::bind(&Client::handleRead, this), nullptr);
            _proto_writer.clear();

            if (command.type() == proto::CommandType::TERMINATE)
                g_running = false;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error writing to client fd: " + std::to_string(_fd) + ": " + e.what())
        EventManager::getInstance().unregisterEvent(*this);
        this->close();
    }
}

void Client::handleMessage(proto::Command command)
{
    // Handle the received command
    LOG_INFO("Received command from client fd " + std::to_string(_fd) + ": " + command.DebugString());

    // Stop reading new commands, we need to process the current command and write the response out first
    EventManager::getInstance().unregisterEvent(*this);

    // call the server callback
    proto::CommandResponse response = _server.onCommand(command);

    // Get ready to send the command response
    _proto_writer.init(response);

    EventManager::getInstance().registerEvent(*this, nullptr, std::bind(&Client::handleWrite, this, command));
}
} // namespace taskmasterd
