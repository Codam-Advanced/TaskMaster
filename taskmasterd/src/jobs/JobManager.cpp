
#include "taskmasterd/include/jobs/JobManager.hpp"
#include "logger/include/Logger.hpp"
#include "taskmasterd/include/core/EventManager.hpp"
#include "taskmasterd/include/jobs/Job.hpp"
#include "taskmasterd/include/jobs/JobConfig.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <utility>
namespace taskmasterd
{

JobManager::JobManager(const std::string& config_path)
    : _config_path(config_path)
{
    _config = JobConfig::getJobConfigs(config_path);

    for (const auto& [name, config] : _config) {
        _jobs.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(config, *this));
    }
}

JobManager::~JobManager()
{
    kill();

    // after we kill all jobs we should wait before we destory the object
    for (auto it = _jobs.begin(); it != _jobs.end(); it++) {
        if (it->second.getState() == Job::State::STOPPED || it->second.getState() == Job::State::EMPTY)
            continue;
        // continue to handle events so that the program doesn't get stuck
        EventManager::getInstance().handleEvents();
        update();
        it = _jobs.begin();
    }
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
            createJob(name);
    }

    // update the _jobs map
    update();
}

void JobManager::update()
{
    for (auto it = _jobs.begin(); it != _jobs.end();) {
        const std::string name = it->first;
        Job&              job  = it->second;

        if (job.removed()) {
            it = _jobs.erase(it);
            continue;
        }
        if (job.replaced()) {
            it = _jobs.erase(it);
            createJob(name);
            continue;
        }
        it++;
    }
}

void JobManager::onStop(const std::string job_name)
{
    Job& job = _jobs.at(job_name);

    // if the job is not in the config anymore we should shedule it to be removed
    if (_config.find(job_name) == _config.end())
        return job.remove();

    // if the config is different then our job we should schedule it to be replaced
    if (_config.at(job_name) != job.getConfig())
        return job.replace();
}

Job& JobManager::findJob(const std::string& job_name)
{
    auto it = _jobs.find(job_name);

    if (it == _jobs.end())
        throw std::runtime_error("JobManager: Cannot find job: " + job_name);

    return it->second;
}

void JobManager::createJob(const std::string& job_name)
{
    auto map = _jobs.emplace(std::piecewise_construct, std::forward_as_tuple(job_name), std::forward_as_tuple(_config.at(job_name), *this));

    // Was the job creation succesfull?
    if (!map.second)
        throw std::runtime_error("new Job :" + job_name + " Couldn't be created");

    if (_config.at(job_name).autostart)
        map.first->second.start();
}

} // namespace taskmasterd