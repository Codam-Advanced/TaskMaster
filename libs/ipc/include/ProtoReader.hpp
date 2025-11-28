#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <optional>
#include <sys/socket.h>
#include <vector>

#include <ipc/include/FileDescriptor.hpp>
#include <proto/taskmaster.pb.h>
#include <utils/include/utils.hpp>

namespace ipc
{
template <typename T> class ProtoReader
{
public:
    /**
     * @brief Read and parse a protobuf message from the given file descriptor.
     *
     * This method reads data from the file descriptor, accumulates it in an internal buffer,
     * and attempts to parse a complete protobuf message of type T. It expects the first
     * 4 bytes of the message to represent the size of the serialized protobuf message in
     * network byte order (big-endian).
     *
     * @param fd The file descriptor to read from.
     * @return A pair containing the number of bytes read and an optional parsed message or
     * std::nullopt if a complete message has not yet been received.
     */
    std::pair<isize, std::optional<T>> read(const ipc::FileDescriptor& fd)
    {
        const usize BUFFER_SIZE = 4096;

        char  buffer[BUFFER_SIZE];
        isize bytes_read = recv(fd.getFd(), buffer, BUFFER_SIZE, 0);
        if (bytes_read > 0) {
            // Append the newly read data to the internal buffer
            _buffer.insert(_buffer.end(), buffer, buffer + bytes_read);
            if (!_message_size.has_value() && _buffer.size() >= sizeof(i32)) {
                // Read the size of the message (first 4 bytes)
                i32 message_size;
                std::memcpy(&message_size, _buffer.data(), sizeof(i32));
                _message_size = ntohl(message_size);
            }

            if (_message_size.has_value() && _buffer.size() >= sizeof(i32) + _message_size.value()) {
                // We have a complete message
                T    message;
                bool success = message.ParseFromArray(_buffer.data() + sizeof(i32), _message_size.value());

                _buffer.erase(_buffer.begin(), _buffer.begin() + sizeof(i32) + _message_size.value());
                _message_size.reset();

                if (!success) {
                    throw std::runtime_error("Failed to parse protobuf message");
                }

                return {bytes_read, message};
            }
        } else if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            throw std::runtime_error("Failed to read from fd: " + std::string(strerror(errno)));
        }

        return {bytes_read, std::nullopt};
    }

private:
    std::vector<char>    _buffer;
    std::optional<usize> _message_size;
};
} // namespace ipc
