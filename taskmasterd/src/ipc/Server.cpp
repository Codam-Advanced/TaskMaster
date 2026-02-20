#include "proto/taskmaster.pb.h"
#include "taskmasterd/include/jobs/JobManager.hpp"
#include <taskmasterd/include/ipc/Server.hpp>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/core/EventManager.hpp>

#define PROVIDE_JOB        "Please provide a job to "
#define PROVIDE_SINGLE_JOB "Please provide one job to "
#define PROVIDE_STATUS \
    "Please provide one job to get the status from at a time.\n\
If you wish to see all jobs, request 'status' with no arguments."
#define PROVIDED_WHILST_TERMINATE "You have given arguments for the 'terminate' command, did you mean to 'stop'?"
#define PROVIDED_WHILST_RELOAD "You have given arguments for the 'reload' command\nThis command does not take a file path"\
" but instead reloads the default config file. Please run the 'reload' command without arguments."

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
    ipc::Socket clientSocket = this->accept();

    _clients.emplace_back(std::make_unique<Client>(std::move(clientSocket), *this));

    // Clean up disconnected clients
    _clients.erase(std::remove_if(_clients.begin(), _clients.end(), [](const std::unique_ptr<Client>& client) { return client->isConnected() == false; }), _clients.end());
}

static const char* commandTypeEnumToString(const proto::CommandType type)
{
    switch (type) {
    case proto::CommandType::START:
        return "start";
    case proto::CommandType::STOP:
        return "stop";
    case proto::CommandType::RESTART:
        return "restart";
    case proto::CommandType::STATUS:
        return "status";
    case proto::CommandType::RELOAD:
        return "reload";
    case proto::CommandType::TERMINATE:
        return "terminate";
    default:
        return "invalid";
    }
    return "invalid";
}

std::optional<proto::CommandResponse> Server::parseCommand(const proto::Command& cmd)
{
    proto::CommandResponse error_response;
    const std::string      cmd_str  = commandTypeEnumToString(cmd.type());
    const auto             arg_size = cmd.args().size();

    if (cmd.type() == proto::CommandType::START || cmd.type() == proto::CommandType::STOP || cmd.type() == proto::CommandType::RESTART) {
        if (arg_size == 0) {
            error_response.set_status(proto::CommandStatus::ARGUMENT_ERROR);
            error_response.set_message(PROVIDE_JOB + cmd_str + ".");
            return error_response;
        } else if (arg_size > 1) {
            error_response.set_status(proto::CommandStatus::TOO_MANY_ARGUMENTS);
            error_response.set_message(PROVIDE_SINGLE_JOB + cmd_str + " at a time.");
            return error_response;
        }
    } else if (cmd.type() == proto::CommandType::STATUS) {
        if (arg_size != 0 && arg_size != 1) {
            error_response.set_status(proto::CommandStatus::TOO_MANY_ARGUMENTS);
            error_response.set_message(PROVIDE_STATUS);
            return error_response;
        }
    } else if (cmd.type() == proto::CommandType::TERMINATE || cmd.type() == proto::CommandType::RELOAD) {
        if (arg_size >= 1) {
            error_response.set_status(proto::CommandStatus::TOO_MANY_ARGUMENTS);

            if (cmd.type() == proto::CommandType::TERMINATE)
                error_response.set_message(PROVIDED_WHILST_TERMINATE);
            else
                error_response.set_message(PROVIDED_WHILST_RELOAD);
            return error_response;
        }
    } else {
        std::string err_msg("Command type " + std::to_string(static_cast<i32>(cmd.type())) + " Not supported ");
        error_response.set_status(proto::CommandStatus::TYPE_ERROR);
        error_response.set_message(err_msg);
        LOG_WARNING(err_msg);
        return error_response;
    }

    // Passed the parsing
    return std::nullopt;
}

proto::CommandResponse Server::onCommand(proto::Command& cmd)
{
    // extract the global
    extern std::atomic<bool> g_running;
    proto::CommandResponse   response;

    auto res = parseCommand(cmd);
    if (res.has_value())
    {
        cmd.set_type(proto::CommandType::COMMAND_ERROR);
        return res.value();
    }

    switch (cmd.type()) {
    case proto::CommandType::START:
        return _manager.start(cmd.args(0));
    case proto::CommandType::STOP:
        return _manager.stop(cmd.args(0));
    case proto::CommandType::RESTART:
        return _manager.restart(cmd.args(0));
    case proto::CommandType::STATUS:
        if (cmd.args().size())
            return _manager.status(cmd.args(0));
        return _manager.status();
    case proto::CommandType::RELOAD:
        return _manager.reload();
    case proto::CommandType::TERMINATE:
        response.set_status(proto::CommandStatus::OK);
        response.set_message("Successfully started the termination sequence");
        return response;
    default:
        std::unreachable();
    }
}
} // namespace taskmasterd
