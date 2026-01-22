#include "taskmasterd/include/jobs/JobConfig.hpp"
#include "taskmasterd/include/jobs/Signal.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <taskmasterd/include/jobs/Process.hpp>
#include "taskmasterd/include/jobs/Job.hpp"

#include <signal.h>
#include <stdexcept>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/core/EventManager.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
Process::Process(const std::string& name, pid_t pgid, Job& job)
    : _name(name)
    , _pid(-1)
    , _pgid(pgid)
    , _state(State::STOPPED)
    , _restarts(0)
    , _job(job)
{
}

void Process::start(const std::string& path, char* const* argv, char* const* env, const JobConfig& config)
{
    // set a timeout that a process needs to stay alive to be a in a valid running state.
    _timer.reset(new Timer(config.start_time, std::bind(&Process::onStartTime, this)));

    _pid = fork();
    if (_pid == -1) {
        throw std::runtime_error("Fork failed for process '" + _name + "': " + strerror(errno));
    }

    if (_pid == 0) {
        // Child process
        setpgid(0, _pgid); // If pgid is 0, pid of the child process is used as pgid
        
        if (!config.err->empty()){
            LOG_DEBUG("Dupping stderr to path: " + config.err.value())
            dupPath(STDERR_FILENO, config.err.value());
        }
        if (!config.out->empty()){
            LOG_DEBUG("Dupping stdout to path: " + config.out.value())
            dupPath(STDOUT_FILENO, config.out.value());
        }

        if (chdir(config.working_dir.c_str()) != 0) {
            throw std::logic_error("Working Dir Error: " + std::string(strerror(errno)));
        }

        umask(config.umask);

        LOG_DEBUG("Executing process " + _name + " with command: " + path);

        if (execve(path.c_str(), argv, env) == -1) {
            perror("execve failed");
            exit(EXIT_FAILURE);
        }

        return;
    }

    // Parent Process
    _state = State::STARTING;
    _job.processStarted();
    _timer->start();

    LOG_DEBUG("Started process " + _name + " with PID " + std::to_string(_pid));
    if ((_fd = pidfd_open(_pid, 0)) == -1)
        throw std::runtime_error("Failed to open pidfd for process '" + _name + "': " + strerror(errno));

    EventManager::getInstance().registerEvent(*this, std::bind(&Process::onStateChange, this), nullptr);

}

void Process::stop(i32 timeout, Signals stop_signal)
{
    if (_state ==  Process::State::BACKOFF || _state == Process::State::EXITED)
    {
        _state = Process::State::STOPPED;
        return;
    }

    // get a shorter reference variable
    const JobConfig::SignalMap& sig_map = JobConfig::signals;

    // find the key from the value in the signal map
    auto it = std::find_if(sig_map.begin(), sig_map.end(),
    [stop_signal](const auto& pair) {
        return pair.second == stop_signal;
    });

    // send signal to pid fd with given stopsignal
    if (pidfd_send_signal(_fd, static_cast<i32>(stop_signal), NULL, 0) == -1)
        throw std::runtime_error("Failed to send " + it->first + " to process: " + _name);

    LOG_DEBUG("Sent " + it->first + " to process: " + _name);

    _state = State::STOPPING;

    // Set up a timer to send SIGKILL if the process does not stop in time
    _timer.reset(new Timer(timeout, [this]() {
        if (_state == State::STOPPING) {
            LOG_DEBUG("Process " + _name + " did not stop in time, sending SIGKILL");
            this->kill();
        }
    }));
    _timer->start();
}

void Process::kill()
{
    if (pidfd_send_signal(_fd, SIGKILL, NULL, 0) == -1)
        throw std::runtime_error("Failed to send SIGKILL to process: " + _name);

    LOG_DEBUG("Sent SIGKILL to process: " + _name);

    _state = State::STOPPING;
}

void Process::onStateChange()
{
    i32   status;

    pid_t result = waitpid(_pid, &status, 0);
    if (result == -1)
        throw std::runtime_error("waitpid failed for process '" + _name + "': " + strerror(errno));

    _timer.reset();
    _job.processStopped();
    EventManager::getInstance().unregisterEvent(*this);

    if (WIFEXITED(status))
        return onExit(status);

    if (WIFSIGNALED(status))
        return onForcedExit(status);

    _state = State::UNKNOWN;
}

void Process::onExit(i32 status)
{
    switch (_state) {
    case State::STOPPING:
        LOG_DEBUG("Process " + _name + " stopped with status " + std::to_string(WEXITSTATUS(status)));
        _state = State::STOPPED;
        _job.onStop(*this);
        break;
    case State::STARTING:
        LOG_WARNING("Process " + _name + " did not reach the start time " + std::to_string(WEXITSTATUS(status)));
        _state = State::BACKOFF;
        _job.onExit(*this, WEXITSTATUS(status));
        break;
    case State::RUNNING:
        LOG_DEBUG("Process " + _name + " exited with status " + std::to_string(WEXITSTATUS(status)));
        _state = State::EXITED;
        _job.onExit(*this, WEXITSTATUS(status));
        break;
    default:
        LOG_WARNING("Process " + _name + " stopped in a wierd state " + std::to_string(static_cast<int>(_state)));
    }
}

void Process::onForcedExit(i32 status)
{
    LOG_DEBUG("Process " + _name + " terminated by signal " + std::to_string(WTERMSIG(status)));
    _state = State::STOPPED;
    _job.onStop(*this);
}

void Process::onStartTime()
{
    LOG_INFO("Process: "  + _name + " succesfully surpasses the start time");
    _state = State::RUNNING;
}


void Process::dupPath(i32 std_input, const std::string& path)
{
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(std_input, fd);
}

} // namespace taskmasterd
