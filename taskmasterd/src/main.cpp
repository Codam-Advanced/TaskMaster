#include <logger/include/Logger.hpp>

#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/ipc/Server.hpp>
#include <taskmasterd/include/jobs/Job.hpp>
#include <taskmasterd/include/jobs/JobConfig.hpp>
#include <iostream>

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
        std::unordered_map<std::string, JobConfig> nodes = JobConfig::getJobConfigs("../tastconfig.yaml");
      
        Server server(Socket::Type::UNIX, Address::UNIX("/tmp/taskmasterd.sock"));
      
        for (const auto& [name, job] : nodes) {
            // print some debug info
            LOG_DEBUG(("Loaded job config:" + name).c_str());
            LOG_DEBUG(("Job name: " + job.name).c_str());
            LOG_DEBUG(("Job cmd: " + job.cmd).c_str());
            LOG_DEBUG(("Job working dir: " + job.working_dir).c_str());
            LOG_DEBUG(("Job numprocs: " + std::to_string(job.numprocs)).c_str());
            LOG_DEBUG(("Job umask: " + std::to_string(job.umask)).c_str());
            LOG_DEBUG(("Job autostart: " + std::to_string(job.autostart)).c_str());
            LOG_DEBUG(("Job exit codes: " + std::to_string(job.exit_codes[0])).c_str());
            LOG_DEBUG(("job env: " + (job.env.find("STARTED_BY") != job.env.end() ? job.env.at("STARTED_BY") : "Empty")).c_str());
        }

        Job job(nodes.begin()->second);
        Job job2((++nodes.begin())->second);

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

    return 0;
}
