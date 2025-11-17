#include <logger/include/Logger.hpp>
#include <taskmasterctl/include/cli/userInput.hpp>

#include <iostream>
#include <optional>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "taskmasterctl"
#endif

namespace taskmasterctl
{

/**
 * @brief Takes a string and snips the first token (something between spaces) and retuns it.
 */
static std::string getToken(std::string& input)
{
    std::size_t delimPos    = input.find(' ');
    std::string token       = input.substr(0, delimPos);
    std::size_t delimEndPos = input.find_first_not_of(' ', delimPos);
    input.erase(0, delimEndPos);
    return token;
}

static bool parseCommandArgs(std::string& input, proto::Command& command)
{
    std::string commandArg;

    while (!input.empty()) {
        LOG_DEBUG(input + ": Added as argument")
        commandArg = getToken(input);
        command.add_args(commandArg);
    }
    return true;
}

static std::string toLower(std::string&& input)
{
    std::string lowered;

    lowered.reserve(input.size());
    for (size_t i = 0; i < input.size(); i++) {
        lowered += tolower(input[i]);
    }
    return lowered;
}

static bool parseCommandType(std::string& input, proto::Command& command)
{
    std::string validTypes[6] = {"start", "stop", "restart", "status", "reload", "terminate"};
    std::string commandType  = toLower(getToken(input));

    for (size_t i = 0; i < 6; i++) {
        if (commandType == validTypes[i]) {
            LOG_DEBUG(commandType + ": Added as command type")
            switch (i) {
            case 0:
                command.set_type(proto::CommandType::START);
                return true;
            case 1:
                command.set_type(proto::CommandType::STOP);
                return true;
            case 2:
                command.set_type(proto::CommandType::RESTART);
                return true;
            case 3:
                command.set_type(proto::CommandType::STATUS);
                return true;
            case 4:
                command.set_type(proto::CommandType::RELOAD);
                return true;
            case 5:
                command.set_type(proto::CommandType::TERMINATE);
                return true;
            default:
                LOG_FATAL("Unreachable: unknown command type")
                exit(1);
            }
        }
    }
    return false;
}

static std::optional<proto::Command> parseInput(std::string& input)
{
    proto::Command command;

    if (!parseCommandType(input, command))
        return std::nullopt;
    if (!parseCommandArgs(input, command))
        return std::nullopt;
    return command;
}

/**
 * @brief Gets input from the user, if the standard in gets closed
 * exit will be called to terminate this program.
 */
static std::string getUserInput()
{
    std::string input;

    std::cout << PROGRAM_NAME << ": ";
    if (!std::getline(std::cin, input)) {
        LOG_DEBUG("getline failed, exiting")
        exit(0);
    }

    return input;
}

proto::Command getCommandFromUser()
{
    std::optional<proto::Command> command;
    std::string                   input;

    input   = getUserInput();
    command = parseInput(input);

    // If the parse input failed we try again till there is a passing result
    while (!command.has_value()) {
        input   = getUserInput();
        command = parseInput(input);
    }

    return command.value();
}

} // namespace taskmasterctl
