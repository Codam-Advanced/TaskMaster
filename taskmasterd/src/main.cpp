#include <logger/include/Logger.hpp>

#include <csignal>

#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/core/Globals.hpp>
#include <taskmasterd/include/ipc/Server.hpp>
#include <taskmasterd/include/jobs/Job.hpp>
#include <taskmasterd/include/jobs/JobConfig.hpp>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "taskmasterd"
#endif

using namespace taskmasterd;

void signalHandler(int signum)
{
    LOG_INFO("Received signal: " + to_string(static_cast<Signals>(signum)));

    g_running = false;
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);

    Logger::LogInterface::Initialize(PROGRAM_NAME, Logger::LogLevel::Debug, true);
    LOG_INFO("Starting " PROGRAM_NAME);

    try {
        JobManager manager("./../tastconfig.yaml");
        Server     server(ipc::Socket::Type::UNIX, ipc::Address::UNIX("/tmp/taskmasterd.sock"), manager);

        while (g_running) {
            EventManager::getInstance().handleEvents();
            manager.update();
        }

        LOG_INFO("Shutting down " PROGRAM_NAME);
    } catch (const std::exception& e) {
        LOG_FATAL("Exception: " + std::string(e.what()));

        return EXIT_FAILURE;
    }

    return 0;
}
