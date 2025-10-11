#pragma once

#include <vector>

#include <taskmasterd/include/core/Socket.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class Client : public Socket
{
public:
    /** Construct a new Client object.
     *
     * @param socket The connected socket representing the client.
     */
    Client(Socket&& socket);

    /** Handle read events from the client.
     *
     * This method reads data from the client socket and stores it in an internal buffer.
     * If the client disconnects, it closes the socket.
     */
    void handleRead() override;

    /** Check if the client is still connected.
     *
     * @return true if the client is connected, false otherwise.
     */
    bool isConnected() const { return _fd != -1; }

private:
    std::vector<u8> _buffer;
};
} // namespace taskmasterd
