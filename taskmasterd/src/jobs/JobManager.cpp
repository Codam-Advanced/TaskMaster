
#include "taskmasterd/include/jobs/JobManager.hpp"
#include "logger/include/Logger.hpp"
#include "taskmasterd/include/jobs/Job.hpp"
#include "taskmasterd/include/jobs/JobConfig.hpp"
#include <stdexcept>
#include <tuple>
#include <utility>
namespace taskmasterd
{

JobManager::JobManager(const std::string& config_path)
    : _config_path(config_path)
{
    std::unordered_map<std::string, JobConfig> nodes = JobConfig::getJobConfigs(config_path);

    for (const auto& [name, config] : nodes) {
        _jobs.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(config, *this));
    }
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
    _config = JobConfig::getJobConfigs(_config_path);

    // loop through jobs that are changed or removed
    for (auto& [name, job] : _jobs) {

        // if job is not found or has changed we will have to stop the old job.
        if (_config.find(name) == _config.end() || _config.at(name) != job.getConfig())
            job.stop();
    }

    // loop through the new config to add new jobs to the job manager
    for (auto& [name, config] : _config) {
        if (_jobs.find(name) == _jobs.end())
            _jobs.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(config, *this));
    }
}

void JobManager::onStop(const std::string& job_name)
{
    const JobConfig& job_config = _config.at(job_name);

    // if the config is the same as the manager current config nothing extra needs to be done
    if (job_config == _jobs.at(job_name).getConfig())
        return;

    // if job is not found anymore it has to be removed
    if (_config.find(job_name) != _config.end())
        return;

    _jobs.erase(job_name);
    _jobs.emplace(std::piecewise_construct, std::forward_as_tuple(job_name), std::forward_as_tuple(job_config, *this));
    if (job_config.autostart)
        _jobs.at(job_name).start();
}

Job& JobManager::findJob(const std::string& job_name)
{
    auto it = _jobs.find(job_name);

    if (it == _jobs.end())
        throw std::runtime_error("JobManager: Cannot find job: " + job_name);

    return it->second;
}

} // namespace taskmasterd