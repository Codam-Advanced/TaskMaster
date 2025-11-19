#pragma once

#include <ipc/include/Address.hpp>
#include <ipc/include/FileDescriptor.hpp>
#include <utils/include/utils.hpp>

namespace ipc
{
class Socket : public FileDescriptor
{
public:
    enum class Type
    {
        TCP,
        UDP,
        UNIX
    };

    /**
     * @brief Construct a new Socket object.
     *
     * @param type The type of the socket (TCP, UDP, UNIX).
     */
    Socket(Type type);

    /**
     * @brief Construct a new Socket object from an existing file descriptor.
     *
     * @param type The type of the socket (TCP, UDP, UNIX).
     * @param fd The file descriptor of the existing socket.
     */
    Socket(Type type, i32 fd);

    /**
     * @brief Bind the socket to the specified address.
     *
     * @param address The address to bind the socket to.
     */
    void bind(const Address& address);

    /**
     * @brief Listen for incoming connections.
     *
     * @param backlog The maximum length of the queue of pending connections.
     */
    void listen(i32 backlog = 5);

    /**
     * @brief Accept a new connection.
     *
     * @return A new Socket object representing the accepted connection.
     */
    Socket accept();

protected:
    Type _type;
};
} // namespace taskmasterd
