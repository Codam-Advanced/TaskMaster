#include <taskmasterd/include/jobs/Process.hpp>

#include <signal.h>
#include <stdexcept>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/core/EventManager.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
Process::Process(const std::string& name, pid_t pgid)
    : EventHandler(-1), _name(name), _pid(-1), _pgid(pgid), _state(State::STOPPED)
{
}

Process::Process(Process&& other) noexcept
    : EventHandler(std::move(other)), _name(std::move(other._name)), _pid(other._pid),
      _pgid(other._pgid), _state(other._state), _killTimer(std::move(other._killTimer))
{
    EventManager::getInstance()->updateEvent(this, EventType::READ);
}

void Process::start(const std::string& path, char* const* argv, char* const* env)
{
    _pid = fork();
    if (_pid == -1) {
        throw std::runtime_error("Fork failed for process '" + _name + "': " + strerror(errno));
    }

    if (_pid == 0) {
        // Child process
        setpgid(0, _pgid); // If pgid is 0, pid of the child process is used as pgid

        LOG_DEBUG("Executing process " + _name + " with command: " + path);

        


        if (execve(path.c_str(), argv, env) == -1) {
            perror("execve failed");
            exit(EXIT_FAILURE);
        }

        return;
    }

    // Parent Process
    _state = State::STARTING;
    LOG_DEBUG("Started process " + _name + " with PID " + std::to_string(_pid));
    if ((_fd = pidfd_open(_pid, 0)) == -1)
        throw std::runtime_error("Failed to open pidfd for process '" + _name +
                                 "': " + strerror(errno));
    EventManager::getInstance()->registerEvent(this, EventType::READ);
}

void Process::stop(i32 timeout)
{
    if (pidfd_send_signal(_fd, SIGTERM, NULL, 0) == -1)
        throw std::runtime_error("Failed to send SIGTERM to process: " + _name);

    LOG_DEBUG("Sent SIGTERM to process: " + _name);

    _state = State::STOPPING;

    // Set up a timer to send SIGKILL if the process does not stop in time
    _killTimer.reset(new Timer(timeout, [this]() {
        if (_state == State::STOPPING) {
            LOG_DEBUG("Process " + _name + " did not stop in time, sending SIGKILL");
            this->kill();
        }
    }));
    _killTimer->start();
}

void Process::kill()
{
    if (pidfd_send_signal(_fd, SIGKILL, NULL, 0) == -1)
        throw std::runtime_error("Failed to send SIGKILL to process: " + _name);

    LOG_DEBUG("Sent SIGKILL to process: " + _name);

    _state = State::STOPPING;
}

void Process::handleRead()
{
    i32   status;
    pid_t result = waitpid(_pid, &status, 0);
    if (result == -1)
        throw std::runtime_error("waitpid failed for process '" + _name + "': " + strerror(errno));

    _killTimer.reset();

    EventManager::getInstance()->unregisterEvent(this);
    if (WIFEXITED(status)) {
        if (_state == State::STOPPING) {
            LOG_DEBUG("Process " + _name + " stopped with status " +
                      std::to_string(WEXITSTATUS(status)));
            _state = State::STOPPED;
        } else {
            LOG_DEBUG("Process " + _name + " exited with status " +
                      std::to_string(WEXITSTATUS(status)));
            _state = State::EXITED;
        }
    } else if (WIFSIGNALED(status)) {
        LOG_DEBUG("Process " + _name + " terminated by signal " + std::to_string(WTERMSIG(status)));
        _state = State::STOPPED;
    } else {
        _state = State::UNKNOWN;
    }
}
} // namespace taskmasterd
