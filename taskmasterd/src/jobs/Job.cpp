#include "logger/include/Logger.hpp"
#include "taskmasterd/include/jobs/JobConfig.hpp"
#include "taskmasterd/include/jobs/Process.hpp"
#include <bits/stdc++.h>
#include <memory>
#include <string>
#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/jobs/Job.hpp>
#include <taskmasterd/include/jobs/JobManager.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
Job::Job(const JobConfig& config, JobManager& manager)
    : _config(config)
    , _manager(manager)
    , _pgid(0)
{
    parseArguments(config);
    parseEnviroment(config);

    _state = State::EMPTY;
}

void Job::parseArguments(const JobConfig& config)
{
    // Prepare argv and env for execve
    _args = split_shell(config.cmd);
    _argv.reserve(_args.size() + 1);
    for (auto& arg : _args) {
        _argv.push_back(arg.c_str());
    }
    _argv.push_back(nullptr);
}

void Job::parseEnviroment(const JobConfig& config)
{

    _env.reserve(config.env.size() + 1);
    for (auto& [key, value] : _config.env) {
        std::string env_entry = key + "=" + value;
        _env.push_back(env_entry.c_str());
    }
    _env.push_back(nullptr);
}

void Job::start()
{
    LOG_DEBUG("Start called called for process" + std::to_string(static_cast<i32>(_state)));

    switch (_state) {
    case State::STOPPING:
        restartProcesses();
        break;
    case State::STOPPED:
        restartProcesses();
        break;
    case State::EMPTY:
        startProcesses();
        break;
    default:
        break;
    }

    _state = State::STARTING;
}

void Job::startProcesses()
{
    // We reserve the amount of processes for the vector to avoid then need to move
    if (_processes.size() == 0)
        _processes.reserve(_config.numprocs);

    for (i32 i = 0; i < _config.numprocs; i++) {
        std::string proc_name = _config.name + "_" + std::to_string(i);

        std::unique_ptr<Process>& proc = _processes.emplace_back(std::make_unique<Process>(proc_name, _pgid, *this));

        proc->start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        // Set the job's pgid to the first process's pid
        if (_pgid == 0) {
            _pgid = proc->getPid();
        }
    }

    _state = State::RUNNING;
}

void Job::restartProcesses()
{
    for (auto& proc : _processes) {

        proc->resetRestarts();
        if (proc->getState() == Process::State::STOPPED)
            proc->start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
    }
}

void Job::stop()
{
    if (_state == State::STOPPED)
        return _manager.onStop(_config.name);

    if (_state == State::STOPPING || _state == State::EMPTY)
        return;

    _state = State::STOPPING;

    for (auto& proc : _processes)
        proc->stop(_config.stop_time, _config.stop_signal);
}

bool Job::allProcessesInState(Process::State state)
{
    for (auto& proc : _processes) {
        if (proc->getState() != state)
            return false;
    }
    return true;
}

void Job::onExit(Process& proc, i32 status_code)
{
    if (proc.getRestarts() == _config.start_retries) {
        LOG_WARNING("Process stopped max retries reached " + proc.getName());
        return;
    }

    switch (_config.restart_policy) {
    case JobConfig::RestartPolicy::NEVER:
        break;
    case JobConfig::RestartPolicy::ALWAYS:
        proc.addRestart();
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        break;
    case JobConfig::RestartPolicy::ON_FAILURE:
        proc.addRestart();
        for (auto& code : _config.exit_codes) {
            // if the status code is known its not an unexpected exit
            if (code == status_code)
                break;
        }
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        break;
    }
}

void Job::onStop(Process& proc)
{
    switch (_state) {
    case State::STARTING:
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        if (allProcessesInState(Process::State::STARTING) || allProcessesInState(Process::State::RUNNING))
            _state = State::RUNNING;
        break;
    case State::STOPPING:
        if (!allProcessesInState(Process::State::STOPPED))
            break;
        _state = State::STOPPED;
        _manager.onStop(_config.name);
        break;
    default:
        LOG_DEBUG("on stop called for process" + proc.getName() + std::to_string(static_cast<i32>(_state)));
    }
}

} // namespace taskmasterd
