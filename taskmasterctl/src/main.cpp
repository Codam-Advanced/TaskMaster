#include <iostream>
#include <ipc/include/FileDescriptor.hpp>
#include <logger/include/Logger.hpp>
#include <proto/taskmaster.pb.h>
#include <taskmasterctl/include/cli/UserInput.hpp>
#include <taskmasterctl/include/ipc/Client.hpp>

#include <readline/readline.h>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "taskmasterctl"
#endif

const char* commands[] = {"start", "stop", "restart", "status", "reload", "terminate", NULL};

static char* completer_generator(const char* text, int state)
{
    static int  list_index;
    static int  len;
    const char* name;

    // First call to the generator for a new prompt will have the state as zero
    if (state == 0) {
        list_index = 0;
        len        = strlen(text);
    }

    name = commands[list_index];
    while (name != NULL) {
        list_index++;
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
        name = commands[list_index];
    }

    return NULL;
}

/**
 * Supposed to return NULL on no match found, or an array of options following a NULL.
 * rl_completion_matches creates this array for us.
 */
static char** completer(const char* text, int start, int end)
{
    (void)start;
    (void)end;

    // Don't fall back on default completion if no match is found
    rl_attempted_completion_over = 1;

    return rl_completion_matches(text, completer_generator);
}

int main()
{
    Logger::LogInterface::Initialize(PROGRAM_NAME, Logger::LogLevel::Debug, true);
    rl_attempted_completion_function = &completer;

    try {
        ipc::Socket socket = taskmasterctl::connectToDaemon();
        while (true) {
            try {
                proto::Command command = taskmasterctl::getCommandFromUser();

                taskmasterctl::sendCommandToDaemon(socket, command);
                taskmasterctl::awaitDaemonResponse(socket);
            } catch (const std::exception& e) {
                LOG_ERROR(e.what())
                return 1;
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR(e.what())
        return 1;
    }

    return 0;
}
