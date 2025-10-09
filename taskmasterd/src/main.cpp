#include <logger/include/Logger.hpp>

#include <taskmasterd/include/core/EventManager.hpp>
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
        LOG_ERROR("Exception: " + std::string(e.what()));
        return EXIT_FAILURE;
    }

    try 
    {
        taskmasterd::JobConfig job("nginx", YAML::LoadFile("../tastconfig.yaml")["jobs"]);

        LOG_DEBUG(("Job name: " + job.name).c_str());
        LOG_DEBUG(("Job cmd: " + job.cmd).c_str());
        LOG_DEBUG(("Job working dir: " + job.working_dir).c_str());
        LOG_DEBUG(("Job numprocs: " + std::to_string(job.numprocs)).c_str());
        LOG_DEBUG(("Job umask: " + std::to_string(job.umask)).c_str());
        LOG_DEBUG(("Job autostart: " + std::to_string(job.autostart)).c_str());
        LOG_DEBUG(("Job exit codes: " + std::to_string(job.exit_codes[0])).c_str());
        LOG_DEBUG(("job env: " + job._env["STARTED_BY"]).c_str());

    }
    catch (const std::exception& e)
    {
        LOG_FATAL(e.what());
    }

    return 0;
}
