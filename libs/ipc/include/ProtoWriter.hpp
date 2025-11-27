#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <ipc/include/FileDescriptor.hpp>
#include <logger/include/Logger.hpp>
#include <proto/taskmaster.pb.h>
#include <utils/include/utils.hpp>

#define PROTOWRITER_MAX_WRITE_SIZE 4096

namespace ipc
{
template <typename T> class ProtoWriter
{
public:
    /**
     * @brief Initializer function expecting a ProtoBuf class that has to be written to a file
     * descriptor.
     *
     * @param toWrite A ProtoBuf generated class that contains the SerializeToString member
     * function.
     * @throw If serializing fails or if it has already been initialized a std::runtime_error will
     * be thrown.
     */
    void init(T& toWrite)
    {
        if (_serializedMessage.size() > 0)
            throw std::runtime_error(
                "Already initialized this protoWriter, finish writing the message or reset");

        if (!toWrite.SerializeToString(&_serializedMessage))
            throw std::runtime_error("Failed to serialize the message");
    }

    /**
     * @brief Writes at most PROTOWRITER_MAX_WRITE_SIZE amount of bytes at a time to the given file
     * descriptor.
     *
     * This method writes data to the given file descriptor, the data consists of a serialized
     * ProtoBuf message. The first write call will sent 4 bytes, which represent the size of the
     * serialized protobuf message in in network byte order (big-endian).
     *
     * This method needs to be called multiple times to write the full message.
     *
     * @param fd The file descriptor to write to.
     * @return A boolean representing the state,
     */
    bool write(const ipc::FileDescriptor& fd)
    {
        if (!_sizeWritten) {
            // Write the size of the message to the file descriptor.
            u32 size = htonl(_serializedMessage.size());
            if (send(fd.getFd(), &size, sizeof(size), MSG_NOSIGNAL) == -1)
                throw std::runtime_error(
                    "Failed to send the message size, is the socket still open?");
            _sizeWritten = true;
            LOG_DEBUG("Successfully sent the message size")
            return false;
        } else {
            if (static_cast<size_t>(_bytesWritten) == _serializedMessage.size()) {
                LOG_DEBUG("Message already fully sent")
                return true;
            }

            // Write the actual message to the file descriptor.
            const char* data      = _serializedMessage.data() + _bytesWritten;
            isize       writeSize = std::min(_serializedMessage.size() - _bytesWritten,
                                       static_cast<size_t>(PROTOWRITER_MAX_WRITE_SIZE));
            isize       bytesSent = send(fd.getFd(), data, writeSize, MSG_NOSIGNAL);
            if (bytesSent == -1)
                throw std::runtime_error(
                    "Failed to send the command message, is the socket still open?");
            _bytesWritten += bytesSent;

            if (static_cast<size_t>(_bytesWritten) == _serializedMessage.size()) {
                this->clear();
                LOG_DEBUG("Successfully sent an entire message")
                return true;
            }

            LOG_DEBUG("Successfully sent " + std::to_string(bytesSent) + " amount of bytes")
            return false;
        }
    }

    /**
     * @brief Clears the internal buffer and data.
     */
    void clear()
    {
        _serializedMessage.clear();
        _bytesWritten = 0;
        _sizeWritten  = false;
    }

private:
    std::string _serializedMessage;
    isize       _bytesWritten = 0;
    bool        _sizeWritten  = false;
};
} // namespace ipc
