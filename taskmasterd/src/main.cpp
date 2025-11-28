#include "taskmasterd/include/jobs/JobManager.hpp"
#include <logger/include/Logger.hpp>

#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/ipc/Server.hpp>
#include <taskmasterd/include/jobs/Job.hpp>
#include <taskmasterd/include/jobs/JobConfig.hpp>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "taskmasterd"
#endif

using namespace taskmasterd;

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Logger::LogInterface::Initialize(PROGRAM_NAME, Logger::LogLevel::Debug, true);
    LOG_INFO("Starting " PROGRAM_NAME);

    try {
        JobManager manager("./../tastconfig.yaml");

        manager.start();

        Server server(ipc::Socket::Type::UNIX, ipc::Address::UNIX("/tmp/taskmasterd.sock"));

        while (true) {
            EventManager::getInstance().handleEvents();
        }
    } catch (const std::exception& e) {
        LOG_FATAL("Exception: " + std::string(e.what()));

        return EXIT_FAILURE;
    }

    return 0;
}
