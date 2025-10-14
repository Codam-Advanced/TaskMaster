#include <logger/include/Logger.hpp>

#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/ipc/Server.hpp>
#include <taskmasterd/include/jobs/Job.hpp>

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
        Server server(Socket::Type::UNIX, Address::UNIX("/tmp/taskmasterd.sock"));

        JobConfig config("myls", "/bin/ls ..");
        config.numprocs = 3;
        JobConfig config2("mysleep", "/bin/sleep 8");
        config2.numprocs = 2;

        Job job(config);
        Job job2(config2);

        job.start();
        job2.start();
        job2.stop();

        while (true) {
            EventManager::getInstance().handleEvents();
        }
    } catch (const std::exception& e) {
        LOG_FATAL("Exception: " + std::string(e.what()));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
