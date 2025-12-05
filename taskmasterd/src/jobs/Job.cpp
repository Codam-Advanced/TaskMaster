#include "logger/include/Logger.hpp"
#include "taskmasterd/include/jobs/JobConfig.hpp"
#include "taskmasterd/include/jobs/Process.hpp"
#include <taskmasterd/include/core/EventManager.hpp>
#include <bits/stdc++.h>
#include <memory>
#include <taskmasterd/include/jobs/Job.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
Job::Job(const JobConfig& config)
    : _config(config)
    , _pgid(0)
{
    // Prepare argv and env for execve
    _args = split_shell(_config.cmd);
    _argv.reserve(_args.size() + 1);
    for (auto& arg : _args) {
        _argv.push_back(arg.c_str());
    }
    _argv.push_back(nullptr);

    _env.reserve(_config.env.size() + 1);
    for (auto& [key, value] : _config.env) {
        std::string env_entry = key + "=" + value;
        _env.push_back(env_entry.c_str());
    }
    _env.push_back(nullptr);
}

void Job::start()
{
    // We reserve the amount of processes for the vector to avoid then need to move
    // already started processes
    if (_processes.size() == 0)
        _processes.reserve(_config.numprocs);

    for (i32 i = 0; i < _config.numprocs; i++) {

        if (_processes.size() > static_cast<size_t>(i)) {
            _processes[i]->start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
            continue;
        }

        std::string proc_name = _config.name + "_" + std::to_string(i);

        std::unique_ptr<Process>& proc = _processes.emplace_back(std::make_unique<Process>(proc_name, _pgid, std::bind(&Job::onExit, this, std::placeholders::_1, std::placeholders::_2)));

        proc->start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        // Set the job's pgid to the first process's pid
        if (_pgid == 0) {
            _pgid = proc->getPid();
        }
    }
}

void Job::stop()
{
    for (auto& proc : _processes) {
        if (proc->getState() == Process::State::RUNNING || proc->getState() == Process::State::STARTING)
            proc->stop(_config.stop_time, _config.stop_signal);
    }

    // We will need to wait for all proccesses to have terminated before returning there might be a chance of a start.
    // this will make the stop function blocking...
    for (auto it = _processes.begin(); it != _processes.end(); it++) {
        LOG_INFO("State of process when stopping" + std::to_string(static_cast<i32>((*it)->getState())))
        if ((*it)->getState() == Process::State::STOPPING)
        {
            // Is this good? we want to keep checking for event since otherwise this soft locks..
            // this is needed to update the stopping state of the processes
            EventManager::getInstance().handleEvents();

            it = _processes.begin();
        }
    }
}

void Job::onExit(Process& proc, i32 status_code)
{

    for (auto it = _processes.begin(); it != _processes.end(); it++) {
        if (it->get() != &proc)
            continue;

        _processes.erase(it);
        break;
    }

    LOG_DEBUG("An process exited from job " + _config.name);
    if (_config.restart_policy == JobConfig::RestartPolicy::NEVER) {
        return;
    }
    if (proc.getRestarts() == _config.start_retries) {
        LOG_INFO("Process stopped max retries reached " + proc.getName());
        return;
    }
    proc.addRestart();

    if (_config.restart_policy == JobConfig::RestartPolicy::ALWAYS) {
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
    } else if (_config.restart_policy == JobConfig::RestartPolicy::ON_FAILURE) {
        for (auto& code : _config.exit_codes) {
            if (code == status_code) {
                // if the status code is known its not an unexpected exit
                return;
            }
        }
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
    }
}

} // namespace taskmasterd
