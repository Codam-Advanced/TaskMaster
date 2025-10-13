#pragma once

#include <taskmasterd/include/core/Address.hpp>
#include <taskmasterd/include/core/FileDescriptor.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
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

    /** Construct a new Socket object.
     *
     * @param type The type of the socket (TCP, UDP, UNIX).
     */
    Socket(Type type);

    /** Construct a new Socket object from an existing file descriptor.
     *
     * @param type The type of the socket (TCP, UDP, UNIX).
     * @param fd The file descriptor of the existing socket.
     */
    Socket(Type type, i32 fd);

    /** Bind the socket to the specified address.
     *
     * @param address The address to bind the socket to.
     */
    void bind(const Address& address);

    /** Listen for incoming connections.
     *
     * @param backlog The maximum length of the queue of pending connections.
     */
    void listen(i32 backlog = 5);

    /** Accept a new connection.
     *
     * @return A new Socket object representing the accepted connection.
     */
    Socket accept();

protected:
    Type _type;
};
} // namespace taskmasterd
