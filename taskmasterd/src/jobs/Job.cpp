#include "logger/include/Logger.hpp"
#include "taskmasterd/include/jobs/JobConfig.hpp"
#include "taskmasterd/include/jobs/Process.hpp"
#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
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
    parseEnvironment(config);

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

void Job::parseEnvironment(const JobConfig& config)
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
}

void Job::restartProcesses()
{
    for (auto& proc : _processes) {

        proc->resetRestarts();
        if (proc->getState() == Process::State::STOPPED || proc->getState() == Process::State::EXITED || proc->getState() == Process::State::BACKOFF)
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

bool Job::allProcessesInStates(std::vector<Process::State> state)
{
    for (auto& proc : _processes) {
        if (std::find(state.begin(), state.end(), proc->getState()) == state.end())
            return false;
    }
    return true;
}

void Job::onExit(Process& proc, i32 status_code)
{
    if (proc.getRestarts() == _config.start_retries) {
        LOG_WARNING("Process stopped max retries reached " + proc.getName());
        if (allProcessesInStates({Process::State::EXITED, Process::State::BACKOFF, Process::State::STOPPED}))
            _state = State::STOPPED;
        return;
    }

    switch (_config.restart_policy) {
    case JobConfig::RestartPolicy::NEVER:
        break;
    case JobConfig::RestartPolicy::ALWAYS:
        proc.addRestart();
        _state = State::STARTING;
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        return;
    case JobConfig::RestartPolicy::ON_FAILURE:
        proc.addRestart();
        for (auto& code : _config.exit_codes) {
            // if the status code is known its not an unexpected exit
            if (code == status_code)
                break;
        }
        _state = State::STARTING;
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        return;
    }

    if (allProcessesInStates({Process::State::EXITED, Process::State::BACKOFF, Process::State::STOPPED}))
        _state = State::STOPPED;
}

static char const* processStateEnumToString(Process::State state)
{
    switch (state) {
    case Process::State::BACKOFF:
        return "BACKOFF";
    case Process::State::EXITED:
        return "EXITED";
    case Process::State::FATAL:
        return "FATAL";
    case Process::State::RUNNING:
        return "RUNNING";
    case Process::State::STARTING:
        return "STARTING";
    case Process::State::STOPPED:
        return "STOPPED";
    case Process::State::STOPPING:
        return "STOPPING";
    default:
        return "UNKNOWN";
    }
}

static char const* jobStateEnumToString(Job::State state)
{
    switch (state) {
    case Job::State::EMPTY:
        return "EMPTY";
    case Job::State::STARTING:
        return "STARTING";
    case Job::State::RUNNING:
        return "RUNNING";
    case Job::State::STOPPING:
        return "STOPPING";
    case Job::State::STOPPED:
        return "STOPPED";
    default:
        return "UNKNOWN";
    }
}

static std::string formatColumn(std::string name, bool should_concatenate, u32 left_width, u32 right_width)
{
    std::stringstream stream;

    if (should_concatenate)
        name = name.substr(0, 18) + "..";
    name.insert(0, left_width, ' ');
    name.insert(name.length(), right_width, ' ');
    stream << name << "|";
    return stream.str();
}

static void setFillerWidth(const std::string& name, u32& left_width, u32& right_width)
{
    const u32 max_size = 20;

    left_width  = std::max(1.0f, std::floor(float(max_size - name.size()) / 2) + 1);
    right_width = std::max(1.0f, std::ceil(float(max_size - name.size()) / 2) + 1);
}

std::ostream& operator<<(std::ostream& os, const Job& job)
{
    const u32   max_size      = 20;
    const u32   process_count = job.getProcessCount();
    std::string name          = job.getConfig().name;
    char const* jobStateStr;
    char const* processStateStr;
    u32         left_width;
    u32         right_width;

    os << "┌──────────────────────┬──────────────────────┬──────────────────────┬──────────────────────┐\n";
    os << "│       Job Name:      │     Job Status:      │    Proccess Name:    │   Process Status:    │\n";
    os << "├──────────────────────┼──────────────────────┼──────────────────────┼──────────────────────┤\n";

    // Start the entry
    os << "|";

    // Job name:
    setFillerWidth(name, left_width, right_width);
    os << formatColumn(name, name.size() > max_size, left_width, right_width);

    jobStateStr = jobStateEnumToString(job.getState());
    setFillerWidth(jobStateStr, left_width, right_width);
    os << formatColumn(jobStateStr, strlen(jobStateStr) > max_size, left_width, right_width);

    // Start next line, leaving the proccess name and status empty
    os << "                      |                      |\n";

    // Start proccess info printing
    os << "├──────────────────────┼──────────────────────┼──────────────────────┼──────────────────────┤\n";
    // Insert the state of every process in the stream:
    for (u32 i = 0; i < process_count; i++) {
        const std::unique_ptr<Process>& current_process = job.getProcess(i);
        Process::State                  processState    = current_process->getState();

        // Start the entry, leave the job name and status empty
        os << "|                      |                      |";

        // Insert the name
        name = current_process->getName();
        setFillerWidth(name, left_width, right_width);
        os << formatColumn(name, name.size() > max_size, left_width, right_width);

        // Insert process state
        processStateStr = processStateEnumToString(processState);
        setFillerWidth(processStateStr, left_width, right_width);
        os << formatColumn(processStateStr, strlen(processStateStr) > max_size, left_width, right_width);

        // Start next line
        os << "\n";
    }

    os << "└──────────────────────┴──────────────────────┴──────────────────────┴──────────────────────┘\n";

    return os;
}

void Job::onStop(Process& proc)
{
    switch (_state) {
    case State::STARTING:
        proc.start(_argv[0], const_cast<char* const*>(_argv.data()), const_cast<char* const*>(_env.data()), _config);
        break;
    case State::STOPPING:
        if (!allProcessesInStates({Process::State::STOPPED}))
            break;
        _state = State::STOPPED;
        _manager.onStop(_config.name);
        break;
    default:
        LOG_DEBUG("on stop called for process" + proc.getName() + std::to_string(static_cast<i32>(_state)));
    }
}

void Job::onProcessSurpassedStartTime()
{
    if (allProcessesInStates({Process::State::RUNNING})) {
        _state = State::RUNNING;
    }
}

} // namespace taskmasterd
