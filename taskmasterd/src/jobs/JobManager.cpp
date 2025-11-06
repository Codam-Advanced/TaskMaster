
#include "taskmasterd/include/jobs/JobManager.hpp"
#include "logger/include/Logger.hpp"
#include "taskmasterd/include/jobs/Job.hpp"
#include <exception>
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

void JobManager::start(const std::string& job_name)
{
    Job& job = findJob(job_name);

    job.start();
}

void JobManager::stop(const std::string& job_name)
{
    Job& job = findJob(job_name);

    job.stop();
}

void JobManager::kill()
{
    for (auto& [name, job] : _jobs) {
        job.stop();
    }
}

void JobManager::restart(const std::string& job_name)
{
    Job& job = findJob(job_name);

    job.stop();
    job.start();
}

void JobManager::reload()
{
    reloadJobs(_config);
    start();
}

void JobManager::reload(const std::string& config_path)
{
    reloadJobs(config_path);
    start();
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
        std::pair<std::string, Job> item(name, Job(config));

        _jobs.emplace(std::move(item));
    }
}

} // namespace taskmasterd