#pragma once

#include <string>
#include <unistd.h>

namespace taskmasterd
{
class Process
{
public:
    enum class State
    {
        STOPPED,
        STARTING,
        RUNNING,
        BACKOFF,
        STOPPING,
        EXITED,
        FATAL,
        UNKNOWN
    };

    Process(const std::string& name, pid_t pgid);
    virtual ~Process() = default;

    void start(const std::string& cmd, char* const* argv, char* const* env);
    void stop();
    void restart();

    pid_t getPid() const { return _pid; }

private:
    std::string _name;
    pid_t       _pid;
    pid_t       _pgid;
    State       _state;
};
} // namespace taskmasterd
