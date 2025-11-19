#include <iostream>
#include <logger/include/Logger.hpp>
#include <taskmasterctl/include/cli/userInput.hpp>
#include <taskmasterctl/include/ipc/Client.hpp>

#include <ipc/include/FileDescriptor.hpp>
#include <proto/taskmaster.pb.h>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "taskmasterctl"
#endif

int main()
{
    Logger::LogInterface::Initialize(PROGRAM_NAME, Logger::LogLevel::Debug, true);

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
