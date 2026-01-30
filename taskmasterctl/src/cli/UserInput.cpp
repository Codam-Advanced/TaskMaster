#include <logger/include/Logger.hpp>
#include <taskmasterctl/include/cli/UserInput.hpp>
#include <utils/include/utils.hpp>

#include <readline/history.h>
#include <readline/readline.h>

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
        commandArg = getToken(input);
        command.add_args(commandArg);
        LOG_DEBUG(commandArg + ": Added as argument")
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
    const char* validTypes[] = {"start", "stop", "restart", "status", "reload", "terminate"};
    std::string commandType  = toLower(getToken(input));

    for (size_t i = 0; i < (sizeof(validTypes) / sizeof(const char*)); i++) {
        if (commandType == validTypes[i]) {
            LOG_DEBUG(commandType + ": Added as command type")
            command.set_type(static_cast<proto::CommandType>(i));
            return true;
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
    char*       line;

    line = readline(PROGRAM_NAME ": ");
    if (!line)
        exit(0);

    if (strlen(line) > 0)
        add_history(line);

    input = line;
    free(line);

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
