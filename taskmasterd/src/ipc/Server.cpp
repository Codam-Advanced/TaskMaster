#include <taskmasterd/include/ipc/Server.hpp>

#include <algorithm>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/core/EventManager.hpp>

namespace taskmasterd
{
Server::Server(Socket::Type type, const Address& address, i32 backlog) : Socket(type)
{
    this->bind(address);
    this->listen(backlog);

    EventManager::getInstance()->registerEvent(this, EventType::READ);

    LOG_INFO("Server listening on fd: " + std::to_string(_fd));
}

void Server::handleRead()
{
    // Accept a new connection
    Socket client = this->accept();

    _clients.emplace_back(std::make_unique<Client>(std::move(client)));

    // Clean up disconnected clients
    _clients.erase(std::remove_if(_clients.begin(),
                                  _clients.end(),
                                  [](const std::unique_ptr<Client>& client) {
                                      return client->isConnected() == false;
                                  }),
                   _clients.end());
}
} // namespace taskmasterd
