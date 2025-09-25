#pragma once

#include <memory>
#include <string>
#include <unistd.h>

#include <taskmasterd/include/core/EventHandler.hpp>
#include <taskmasterd/include/core/Timer.hpp>

namespace taskmasterd
{
class Process : public EventHandler
{
public:
    enum class State
    {
        STOPPED,  // The process has been stopped due to a stop request or has never been started.
        STARTING, // The process is starting due to a start request.
        RUNNING,  // The process is running.
        BACKOFF,  // The process entered the STARTING state but subsequently exited too quickly
                  // (before the time defined in startsecs) to move to the RUNNING state.
        STOPPING, // The process is stopping due to a stop request.
        EXITED,   // The process has exited after running.
        FATAL,    // The process could not be started successfully.
        UNKNOWN   // The process state is unknown. (programming error)
    };

    Process(const std::string& name, pid_t pgid);
    virtual ~Process() = default;

    void start(const std::string& cmd, char* const* argv, char* const* env);
    void stop();
    void kill();
    void restart();

    void handleRead() override;

    pid_t getPid() const { return _pid; }

private:
    std::string _name;
    pid_t       _pid;
    pid_t       _pgid;
    State       _state;

    std::unique_ptr<Timer> _killTimer;
};
} // namespace taskmasterd
