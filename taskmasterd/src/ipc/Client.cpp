#include <taskmasterd/include/ipc/Client.hpp>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/core/EventManager.hpp>

#define BUFFER_SIZE 4096

namespace taskmasterd
{
Client::Client(Socket&& socket) : Socket(std::move(socket))
{
    EventManager::getInstance()->registerEvent(this, EventType::READ);

    LOG_INFO("New client connected with fd: " + std::to_string(_fd));
}

void Client::handleRead()
{
    char    buffer[BUFFER_SIZE];
    ssize_t bytes_read = recv(_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_read > 0) {
        _buffer.insert(_buffer.end(), buffer, buffer + bytes_read);
        LOG_DEBUG("Received " + std::to_string(bytes_read) +
                  " bytes from client with fd: " + std::to_string(_fd));
    } else if (bytes_read == 0) {
        LOG_INFO("Client disconnected with fd: " + std::to_string(_fd));
        EventManager::getInstance()->unregisterEvent(this);
        this->close();
    } else {
        LOG_ERROR("Error reading from client");
    }
}
} // namespace taskmasterd
