#pragma once

#include <memory>
#include <string>
#include <unistd.h>

#include <taskmasterd/include/core/FileDescriptor.hpp>
#include <taskmasterd/include/core/Timer.hpp>

namespace taskmasterd
{
class Process : public FileDescriptor
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

    /**
     * @brief Construct a new Process object.
     *
     * @param name The name of the process.
     * @param pgid The process group ID. If 0, the child's PID will be used as PGID.
     */
    Process(const std::string& name, pid_t pgid);
    Process(Process&&) noexcept;
    virtual ~Process() = default;

    /**
     * @brief Start the process by forking and executing the specified command.
     *
     * @param path The path to the executable.
     * @param argv The argument list for the executable.
     * @param env The environment variables for the executable.
     */
    void start(const std::string& path, char* const* argv, char* const* env);

    /**
     * @brief Gracefully stop the process using SIGTERM.
     *
     * @param timeout The time in seconds to wait for the process to terminate gracefully
     *                before forcefully killing it. Default is 5 seconds.
     */
    void stop(i32 timeout = 5);

    /**
     * @brief Forcefully kill the process using SIGKILL.
     *
     * This method sends a SIGKILL signal to the process to terminate it immediately.
     */
    void kill();

    /**
     * @brief Callack for state changes.
     *
     * This method is called when the process state changes, such as when it exits.
     */
    void onStateChange();

    pid_t getPid() const { return _pid; }

private:
    std::string _name;
    pid_t       _pid;
    pid_t       _pgid;
    State       _state;

    std::unique_ptr<Timer> _killTimer;
};
} // namespace taskmasterd
