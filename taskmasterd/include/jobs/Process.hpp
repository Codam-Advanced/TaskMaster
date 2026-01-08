#pragma once

#include "taskmasterd/include/jobs/Signal.hpp"
#include <memory>
#include <string>
#include <unistd.h>

#include <ipc/include/FileDescriptor.hpp>
#include <taskmasterd/include/core/Timer.hpp>
#include <taskmasterd/include/jobs/JobConfig.hpp>

namespace taskmasterd
{
class Process : public ipc::FileDescriptor
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
    Process(const std::string& name, pid_t pgid, std::function<void(Process&, i32)> callback);
    virtual ~Process() = default;

    /**
     * @brief Start the process by forking and executing the specified command.
     *
     * @param path The path to the executable.
     * @param argv The argument list for the executable.
     * @param env The environment variables for the executable.
     */
    void start(const std::string& path, char* const* argv, char* const* env, const JobConfig& config);

    /**
     * @brief Gracefully stop the process using SIGTERM.
     *
     * @param timeout The time in seconds to wait for the process to terminate gracefully
     *                before forcefully killing it. Default is 5 seconds.
     * @param stop_signal The signal to send to the program to call a stop
     */
    void stop(i32 timeout = 5, Signals stop_signal = Signals::TERM);

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

    State getState() const { return _state; }

    i32 getRestarts() const { return _restarts; }

    const std::string& getName() const { return _name; }

    void addRestart() { _restarts++; }  
    void resetRestarts() { _restarts = 0; }

private:
    /**
     * @brief Method is called once the process has exited
     *
     * this can either be by stopping the job gracefully or if it exits by itself.
     */
    void onExit(i32 status);

    /**
     * @brief Method is called once the process has been killed
     *
     * this happends once a process takes to long to exit gracefully.
     */
    void onForcedExit(i32 status);

    /**
     * @brief Method is called once the start timer has surpassed
     *
     * this will switch a program starting state into a running state
     */
    void onStartTime();

    /**
     * @brief Helper method to dup a path instead of a fd
     *
     * the reason is to safe line spaces 
     */
    void dupPath(i32 std_input, const std::string& path);

    std::string _name;
    pid_t       _pid;
    pid_t       _pgid;
    State       _state;
    i32         _restarts;

    std::function<void(Process&, i32)> _onExit;
    std::unique_ptr<Timer>             _timer;
};
} // namespace taskmasterd
