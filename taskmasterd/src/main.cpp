#include <logger/include/Logger.hpp>

#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/jobs/Job.hpp>
#include <taskmasterd/include/jobs/JobConfig.hpp>
#include <iostream>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "taskmasterd"
#endif

using namespace taskmasterd;

std::vector<Job> start()
{
    std::vector<Job> jobs;

    try 
    {
        std::unordered_map<std::string, JobConfig> nodes = JobConfig::getJobConfigs("../tastconfig.yaml");

        for (const auto& [name, config] : nodes) 
        {
            Job job(config);
            if (config.autostart)
            {
                LOG_INFO("Autostarting job: " + name);
                job.start();
            }
            jobs.push_back(std::move(job));
        }
    }
    catch (const std::exception& e) 
    {
        LOG_FATAL("ERROR: Unable to start jobs reason: " + std::string(e.what()));
    }

    return jobs;
}

void stop(std::vector<Job>& jobs)
{
    for (auto& job : jobs) 
    {
        job.stop();
    }
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Logger::LogInterface::Initialize(PROGRAM_NAME, Logger::LogLevel::Debug, true);
    LOG_INFO("Starting " PROGRAM_NAME);

    EventManager::initialize();

    std::vector<Job> jobs = start();
    stop(jobs);
    

    while (true) 
    {
        EventManager::getInstance()->handleEvents();
    }

    return 0;
}



