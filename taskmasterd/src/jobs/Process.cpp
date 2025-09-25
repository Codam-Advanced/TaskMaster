#include <taskmasterd/include/jobs/Process.hpp>

#include <signal.h>
#include <stdexcept>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <logger/include/Logger.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
Process::Process(const std::string& name, pid_t pgid)
    : EventHandler(-1), _name(name), _state(State::STOPPED), _pid(-1), _pgid(pgid)
{
}

void Process::start(const std::string& cmd, char* const* argv, char* const* env)
{
    _pid = fork();
    if (_pid == -1) {
        throw std::runtime_error("Fork failed");
    }

    if (_pid == 0) {
        // Child process
        setpgid(0, _pgid); // If pgid is 0, pid of the child process is used as pgid
        if (execve(cmd.c_str(), argv, env) == -1) {
            // If execve fails
            perror("execve failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent Process
    _fd = syscall(SYS_pidfd_open, _pid, 0);
}

void Process::stop()
{
    if (syscall(SYS_pidfd_send_signal, _fd, SIGTERM, NULL, 0) == -1)
        throw std::runtime_error("Failed to send SIGTERM to process: " + _name);

    LOG_DEBUG("Sent SIGTERM to process: " + _name);

    _state = State::STOPPING;

    _killTimer.reset(new Timer(5, [this]() {
        if (_state == State::STOPPING) {
            LOG_DEBUG("Process " + _name + " did not stop in time, sending SIGKILL");
            this->kill();
        }
    }));
    _killTimer->start();
}

void Process::kill()
{
    if (syscall(SYS_pidfd_send_signal, _fd, SIGKILL, NULL, 0) == -1)
        throw std::runtime_error("Failed to send SIGKILL to process: " + _name);

    LOG_DEBUG("Sent SIGKILL to process: " + _name);

    _state = State::STOPPING;
}

void Process::handleRead()
{
    siginfo_t siginfo = {0};
    pid_t     result  = waitid(P_PIDFD, _fd, &siginfo, WEXITED | WNOHANG);
    if (result == -1) {
        throw std::runtime_error("waitpid failed for process: " + _name);
    } else if (result != 0) {
        i32 status = siginfo.si_status;
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
            _state = State::FATAL;
        } else {
            _state = State::UNKNOWN;
        }
    }
}
} // namespace taskmasterd
