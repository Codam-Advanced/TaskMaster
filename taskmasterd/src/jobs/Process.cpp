#include <taskmasterd/include/jobs/Process.hpp>

#include <stdexcept>
#include <unistd.h>

namespace taskmasterd
{
Process::Process(const std::string& name, pid_t pgid)
    : _name(name), _state(State::STOPPED), _pid(-1), _pgid(pgid)
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
}
} // namespace taskmasterd
