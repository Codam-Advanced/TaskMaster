#pragma once

#include <memory>
#include <vector>

#include <taskmasterd/include/core/Socket.hpp>
#include <taskmasterd/include/ipc/Client.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class Server : public Socket
{
public:
    using Clients = std::vector<std::unique_ptr<Client>>;

    /**
     * @brief Construct a new Server object.
     *
     * @param type The type of the socket (TCP, UDP, UNIX).
     * @param address The address to bind the server socket to.
     * @param backlog The maximum length of the queue of pending connections.
     */
    Server(Socket::Type type, const Address& address, i32 backlog = 5);
    virtual ~Server();

    /**
     * @brief Handle read events on the server socket.
     *
     * This method accepts new client connections and creates Client objects
     * for each accepted connection. It also cleans up disconnected clients.
     */
    void onAccept();

private:
    Clients _clients;
};
} // namespace taskmasterd
