#pragma once

#include <optional>
#include <vector>

#include <logger/include/Logger.hpp>
#include <proto/taskmaster.pb.h>
#include <taskmasterd/include/core/Socket.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
template <typename T> class ProtoReader : public EventHandler
{
public:
    ProtoReader(EventHandler&& handler)
        : EventHandler(std::move(handler)), _buffer(), _message_size(std::nullopt)
    {
    }

    /** Handle read events on the fd.
     * This method reads data from the fd, reconstructs protobuf messages,
     * and calls handleMessage for each complete message received.
     */
    void handleRead() override
    {
        const usize BUFFER_SIZE = 4096;

        char    buffer[BUFFER_SIZE];
        ssize_t bytes_read = recv(_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_read > 0) {
            _buffer.insert(_buffer.end(), buffer, buffer + bytes_read);
            if (!_message_size.has_value() && _buffer.size() >= sizeof(i32)) {
                // Read the size of the message (first 4 bytes)
                i32 message_size;
                memcpy(&message_size, _buffer.data(), sizeof(i32));
                _message_size = ntohl(message_size);
            }

            if (_message_size.has_value() &&
                _buffer.size() >= sizeof(i32) + _message_size.value()) {
                // We have a complete message
                T message;
                if (!message.ParseFromArray(_buffer.data() + sizeof(i32), _message_size.value())) {
                    LOG_ERROR("Failed to parse message from buffer: " +
                              message.InitializationErrorString());
                }

                _buffer.erase(_buffer.begin(),
                              _buffer.begin() + sizeof(i32) + _message_size.value());
                _message_size.reset();

                if (message.IsInitialized())
                    this->handleMessage(message);
            }
        } else if (bytes_read == 0) {
            LOG_INFO("Client disconnected with fd: " + std::to_string(_fd));
            this->close();
        } else {
            LOG_ERROR("Error reading from client");
        }
    }

    /** Handle a complete protobuf message.
     * This is a pure virtual function that must be implemented by derived classes.
     *
     * @param message The complete protobuf message received.
     */
    virtual void handleMessage(T message) = 0;

private:
    std::vector<char>    _buffer;
    std::optional<usize> _message_size;
};
} // namespace taskmasterd
