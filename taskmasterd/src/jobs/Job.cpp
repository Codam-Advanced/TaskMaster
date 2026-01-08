#include "logger/include/Logger.hpp"
#include "taskmasterd/include/jobs/JobConfig.hpp"
#include "taskmasterd/include/jobs/Process.hpp"
#include <bits/stdc++.h>
#include <memory>
#include <taskmasterd/include/core/EventManager.hpp>
#include <taskmasterd/include/jobs/Job.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
Job::Job(const JobConfig& config)
    : _config(config)
    , _pgid(0)
    , _stopped(0)
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
    switch (_state) {
    case State::RUNNING:
        break;

    case State::STOPPED:
        restartProcesses();
        break;

    case State::EMPTY:
        startProcesses();
        break;

    case State::RELOADING:
        break;
    }

    _state = State::RUNNING;
}

void Job::startProcesses()
{
    // We reserve the amount of processes for the vector to avoid then need to move
    // already started processes
    if (_processes.size() == 0)
        _processes.reserve(_config.numprocs);

    _stopped = 0;

    for (i32 i = 0; i < _config.numprocs; i++) {
        std::string proc_name = _config.name + "_" + std::to_string(i);

        std::unique_ptr<Process>& proc = _processes.emplace_back(std::make_unique<Process>(proc_name, _pgid, std::bind(&Job::onExit, this, std::placeholders::_1, std::placeholders::_2)));

        proc->start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        // Set the job's pgid to the first process's pid
        if (_pgid == 0) {
            _pgid = proc->getPid();
        }
    }
}

void Job::restartProcesses()
{
    for (i32 i = 0; i < _config.numprocs; i++) {
        Process::State state = _processes[i]->getState();

        _processes[i]->resetRestarts();
        if (state == Process::State::STOPPING) {
            _processes[i]->setOnStop([this, i]() { _processes[i]->start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config); });
        }
    }
}

void Job::stop()
{
    for (auto& proc : _processes) {
        if (proc->getState() == Process::State::RUNNING || proc->getState() == Process::State::STARTING)
            proc->stop(_config.stop_time, _config.stop_signal);
    }

    _state = State::STOPPED;
}

void Job::reload(const JobConfig& config)
{
    _state = State::RELOADING;

    // first stop all processes
    this->stop();

    // parse the new config variables
    parseArguments(config);
    parseEnviroment(config);

    for (auto& proc : _processes) {
        if (proc->getState() == Process::State::STOPPING) {
            proc->setOnStop([this, config]() {
                _stopped++;

                // We want to wait for all number processes to stop
                // so that we can clear the vector before creating the new processes.
                if (_stopped == _config.numprocs) {
                    _processes.clear();
                    _config = config;

                    _state = State::RUNNING;
                    this->startProcesses();
                }
            });
        } else {
            _stopped++;
        }
    }
}

void Job::onExit(Process& proc, i32 status_code)
{
    LOG_DEBUG("An process exited from job " + _config.name);
    if (_config.restart_policy == JobConfig::RestartPolicy::NEVER) {
        return;
    }
    if (proc.getRestarts() == _config.start_retries) {
        LOG_WARNING("Process stopped max retries reached " + proc.getName());
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
