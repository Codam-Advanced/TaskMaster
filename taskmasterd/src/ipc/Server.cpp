#include "proto/taskmaster.pb.h"
#include "taskmasterd/include/jobs/JobManager.hpp"
#include <taskmasterd/include/ipc/Server.hpp>

#include <algorithm>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/core/EventManager.hpp>

namespace taskmasterd
{
Server::Server(Socket::Type type, const ipc::Address& address, JobManager& manager, i32 backlog)
    : Socket(type)
    , _manager(manager)
{
    this->bind(address);
    this->listen(backlog);

    EventManager::getInstance().registerEvent(*this, std::bind(&Server::onAccept, this), nullptr);

    LOG_INFO("Server listening on fd: " + std::to_string(_fd));
    _manager.start();
}

Server::~Server()
{
    EventManager::getInstance().unregisterEvent(*this);
}

void Server::onAccept()
{
    // Accept a new connection
    ipc::Socket client = this->accept();

    Client::CommandCallback cb = [this](proto::Command cmd) { this->onCommand(cmd); };

    _clients.emplace_back(std::make_unique<Client>(std::move(client), std::move(cb)));

    // Clean up disconnected clients
    _clients.erase(std::remove_if(_clients.begin(), _clients.end(), [](const std::unique_ptr<Client>& client) { return client->isConnected() == false; }), _clients.end());
}

void Server::onCommand(proto::Command cmd)
{
    // extract the global
    extern std::atomic<bool> g_running;

    // TODO: handle the STATUS type
    switch (cmd.type()) {
    case proto::CommandType::START:
        _manager.start(cmd.args(0));
        break;
    case proto::CommandType::STOP:
        _manager.stop(cmd.args(0));
        break;
    case proto::CommandType::RESTART:
        _manager.restart(cmd.args(0));
        break;
    case proto::CommandType::RELOAD:
        _manager.reload();
        break;
    case proto::CommandType::TERMINATE:
        g_running = false;
        break;
    default:
        LOG_WARNING("Command type " + std::to_string(static_cast<i32>(cmd.type())) + " Not supported ");
    }
}
} // namespace taskmasterd
