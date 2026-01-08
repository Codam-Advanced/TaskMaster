
#include "taskmasterd/include/jobs/JobManager.hpp"
#include "logger/include/Logger.hpp"
#include "taskmasterd/include/jobs/Job.hpp"
#include <stdexcept>
namespace taskmasterd
{

JobManager::JobManager(const std::string& config_path)
    : _config(config_path)
{
    reloadJobs(config_path);
}

JobManager::~JobManager()
{
    kill();
}

void JobManager::start()
{
    for (auto& [name, job] : _jobs) {
        if (job.getConfig().autostart) {
            job.start();
            LOG_INFO("Starting job: " + name);
        }
    }
}

proto::CommandResponse JobManager::start(const std::string& job_name)
{
    proto::CommandResponse res;
    Job& job = findJob(job_name);

    job.start();
    res.set_status(proto::CommandStatus::OK);
    res.set_message("Successfully put job " + job_name + " in starting state.");
    return res;
}

proto::CommandResponse JobManager::stop(const std::string& job_name)
{
    proto::CommandResponse res;
    Job& job = findJob(job_name);

    job.stop();
    res.set_status(proto::CommandStatus::OK);
    res.set_message("Successfully put job " + job_name + " in stopping state.");
    return res;
}

void JobManager::kill()
{
    for (auto& [name, job] : _jobs) {
        job.stop();
    }
}

proto::CommandResponse JobManager::restart(const std::string& job_name)
{
    proto::CommandResponse res;
    Job& job = findJob(job_name);

    job.stop();
    job.start();
    res.set_status(proto::CommandStatus::OK);
    res.set_message("Successfully put job " + job_name + " in restarting state.");
    return res;
}

void JobManager::reload()
{
    reloadJobs(_config);
    start();
}

proto::CommandResponse JobManager::reload(const std::string& config_path)
{
    proto::CommandResponse res;

    reloadJobs(config_path);
    start();
    res.set_status(proto::CommandStatus::OK);
    res.set_message("Successfully started a reload of config file " + config_path + ".");
    return res;
}

proto::CommandResponse JobManager::status()
{
    proto::CommandResponse res;
    std::stringstream      msg;

    res.set_status(proto::CommandStatus::OK);
    msg << "┌──────────────────────┬──────────────────────┬──────────────────────┐\n";
    msg << "│       Job Name:      │     Job Status:      │   Process Status:    │\n";
    msg << "├──────────────────────┼──────────────────────┼──────────────────────┤\n";
    for (auto& [_, job] : _jobs) {
        (void) job;
        // msg << job;
    }
    msg << "└──────────────────────┴──────────────────────┴──────────────────────┘\n";
    res.set_message(msg.str());
    return res;
}

proto::CommandResponse JobManager::status(const std::string& job_name)
{
    proto::CommandResponse res;
    std::stringstream      msg;
    Job&                   job = findJob(job_name);

    res.set_status(proto::CommandStatus::OK);
    msg << "┌──────────────────────┬──────────────────────┬──────────────────────┐\n";
    msg << "│       Job Name:      │     Job Status:      │   Process Status:    │\n";
    msg << "├──────────────────────┼──────────────────────┼──────────────────────┤\n";
    (void) job;
    // msg << job;
    msg << "└──────────────────────┴──────────────────────┴──────────────────────┘\n";
    res.set_message(msg.str());
    return res;
}

Job& JobManager::findJob(const std::string& job_name)
{
    auto it = _jobs.find(job_name);

    if (it == _jobs.end())
        throw std::runtime_error("JobManager: Cannot find job: " + job_name);

    return it->second;
}

void JobManager::reloadJobs(const std::string& config_path)
{
    if (!_jobs.empty()) {
        _jobs.clear();
    }

    std::unordered_map<std::string, JobConfig> nodes = JobConfig::getJobConfigs(config_path);

    for (const auto& [name, config] : nodes) {
        _jobs.emplace(name, config);
    }
}

} // namespace taskmasterd