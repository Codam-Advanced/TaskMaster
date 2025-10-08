#include <logger/include/Logger.hpp>

#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/jobs/Job.hpp>

using namespace taskmasterd;

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    LOG_DEBUG("Starting taskmasterd...");

    try {
        EventManager::initialize();

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
            EventManager::getInstance()->handleEvents();
        }
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("Error: ") + e.what());
        return EXIT_FAILURE;
    }
}
